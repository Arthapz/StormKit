// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/core/Span.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>

#include "ConvertFormatShaders.hpp"

using namespace storm;
using namespace storm::render;

#define DELETER [](auto handle, const auto &device) { device->deallocateVmaAllocation(handle); }

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(const Device &device, TextureType type, TextureCreateFlag flags)
    : m_device { &device }, m_type { type }, m_flags { flags }, m_vma_texture_memory { DELETER,
                                                                                       *m_device } {
}

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(const Device &device,
                 core::Extentu extent,
                 render::PixelFormat format,
                 vk::Image image)
    : Texture(device) {
    m_extent             = std::move(extent);
    m_format             = std::move(format);
    m_non_owning_texture = std::move(image);
}

/////////////////////////////////////
/////////////////////////////////////
Texture::~Texture() = default;

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(Texture &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Texture &Texture::operator=(Texture &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromImage(image::Image &image,
                            PixelFormat storage_format,
                            SampleCountFlag samples,
                            core::UInt32 mip_levels,
                            core::UInt32 layers,
                            TextureUsage usage) {
    STORM_EXPECTS(!m_non_owning_texture);

    const auto format = [&image]() {
        switch (image.channels()) {
            case 1: return render::PixelFormat::R8_UNorm;
            case 2: return render::PixelFormat::RG8_UNorm;
            case 3: return render::PixelFormat::RGB8_UNorm;
            case 4: return render::PixelFormat::RGBA8_UNorm;
        }

        return render::PixelFormat::Undefined;
    }();

    loadFromMemory(image.data(),
                   { image.extent().width, image.extent().height },
                   format,
                   storage_format,
                   samples,
                   mip_levels,
                   layers,
                   usage);
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromMemory(
    storm::core::span<const core::Byte> data,
    core::Extentu extent,
    [[maybe_unused]] render::PixelFormat load_format, // TODO convert to storage format
    render::PixelFormat storage_format,
    SampleCountFlag samples,
    core::UInt32 mip_levels,
    core::UInt32 layers,
    TextureUsage usage) {
    STORM_EXPECTS(!m_non_owning_texture);

    createTextureData(extent, storage_format, samples, mip_levels, layers, usage);

    auto staging_buffer =
        m_device->createStagingBuffer(gsl::narrow_cast<core::ArraySize>(std::size(data)));
    staging_buffer.upload<const core::Byte>(data);

    auto fence = m_device->createFence();

    auto command_buffer = m_device->graphicsQueue().createCommandBuffer();
    command_buffer.begin(true);
    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Undefined,
                                           TextureLayout::Transfer_Dst_Optimal);
    command_buffer.copyBufferToTexture(staging_buffer, *this);
    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Transfer_Dst_Optimal,
                                           TextureLayout::Shader_Read_Only_Optimal);
    command_buffer.end();
    command_buffer.build();

    m_device->graphicsQueue().submit(core::makeConstObserversArray(command_buffer),
                                     {},
                                     {},
                                     core::makeObserver(fence));
    m_device->waitForFence(fence);
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::createTextureData(core::Extentu extent,
                                render::PixelFormat format,
                                render::SampleCountFlag samples,
                                core::UInt32 mip_levels,
                                core::UInt32 layers,
                                render::TextureUsage usage) {
    STORM_EXPECTS(m_mip_levels > 0);
    STORM_EXPECTS(m_layers > 0);

    m_samples    = samples;
    m_mip_levels = mip_levels;
    m_layers     = layers;
    m_format     = format;
    m_extent     = extent / layers;

    const auto create_info =
        vk::ImageCreateInfo {}
            .setImageType(toVK(m_type))
            .setFormat(toVK(m_format))
            .setExtent(VkExtent3D { .width = m_extent.w, .height = m_extent.h, .depth = 1 })
            .setMipLevels(m_mip_levels)
            .setArrayLayers(m_layers)
            .setSamples(toVKBits(m_samples))
            .setTiling(vk::ImageTiling::eOptimal)
            .setUsage(toVK(usage))
            .setSharingMode(vk::SharingMode::eExclusive)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFlags(toVK(m_flags));

    m_vk_texture = m_device->createVkImage(create_info);

    const auto requirements = m_device->getVkImageMemoryRequirements(*m_vk_texture);
    const auto alloc_info =
        VmaAllocationCreateInfo { .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

    m_vma_texture_memory.reset(m_device->allocateVmaAllocation(alloc_info, requirements));

    m_device->bindVmaImageMemory(m_vma_texture_memory, *m_vk_texture);
}

/////////////////////////////////////
/////////////////////////////////////
TextureView Texture::createView(TextureViewType type,
                                TextureSubresourceRange subresource_range) const noexcept {
    return TextureView { *m_device, *this, type, std::move(subresource_range) };
}

/////////////////////////////////////
/////////////////////////////////////
TextureViewOwnedPtr Texture::createViewPtr(TextureViewType type,
                                           TextureSubresourceRange subresource_range) const {
    return std::make_unique<TextureView>(*m_device, *this, type, std::move(subresource_range));
}
