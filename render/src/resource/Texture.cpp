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

#include <gli/load.hpp>

using namespace storm;
using namespace storm::render;

#define DELETER [](auto handle, const auto &device) { device->deallocateVmaAllocation(handle); }

constexpr auto toPixelFormat(gli::format format) {
    switch (format) {
        case gli::FORMAT_RGBA16_SFLOAT_PACK16: return render::PixelFormat::RGBA16F;
        default: STORM_ENSURES(false);
    }
}

constexpr auto toPixelFormat(image::Image::Format format) noexcept {
    switch (format) {
        case image::Image::Format::R8_SNorm: return PixelFormat::R8_SNorm;
        case image::Image::Format::R8_UNorm: return PixelFormat::R8_UNorm;
        case image::Image::Format::RG8_SNorm: return PixelFormat::RG8_SNorm;
        case image::Image::Format::RG8_UNorm: return PixelFormat::RG8_UNorm;
        case image::Image::Format::R8I: return PixelFormat::R8I;
        case image::Image::Format::R8U: return PixelFormat::R8U;
        case image::Image::Format::RG8I: return PixelFormat::RG8I;
        case image::Image::Format::RG8U: return PixelFormat::RG8U;
        case image::Image::Format::RGB8_SNorm: return PixelFormat::RGB8_SNorm;
        case image::Image::Format::RGB8_UNorm: return PixelFormat::RGB8_UNorm;
        case image::Image::Format::BGR8_UNorm: return PixelFormat::BGR8_UNorm;
        case image::Image::Format::RGB8I: return PixelFormat::RGB8I;
        case image::Image::Format::RGB8U: return PixelFormat::RGB8U;
        case image::Image::Format::RGBA8_SNorm: return PixelFormat::RGBA8_SNorm;
        case image::Image::Format::RGBA8_UNorm: return PixelFormat::RGBA8_UNorm;
        case image::Image::Format::RGBA16_SNorm: return PixelFormat::RGBA16_SNorm;
        case image::Image::Format::BGRA8_UNorm: return PixelFormat::BGRA8_UNorm;
        case image::Image::Format::sRGB8: return PixelFormat::sRGB8;
        case image::Image::Format::sBGR8: return PixelFormat::sBGR8;
        case image::Image::Format::sRGBA8: return PixelFormat::sRGBA8;
        case image::Image::Format::sBGRA8: return PixelFormat::sBGRA8;

        case image::Image::Format::R16_SNorm: return PixelFormat::R16_SNorm;
        case image::Image::Format::R16_UNorm: return PixelFormat::R16_UNorm;
        case image::Image::Format::R16I: return PixelFormat::R16I;
        case image::Image::Format::R16U: return PixelFormat::R16U;
        case image::Image::Format::RG16_SNorm: return PixelFormat::RG16_SNorm;
        case image::Image::Format::RG16_UNorm: return PixelFormat::RG16_UNorm;
        case image::Image::Format::RG16I: return PixelFormat::RG16I;
        case image::Image::Format::RG16U: return PixelFormat::RG16U;
        case image::Image::Format::RG16F: return PixelFormat::RG16F;
        case image::Image::Format::RGB16I: return PixelFormat::RGB16I;
        case image::Image::Format::RGB16U: return PixelFormat::RGB16U;
        case image::Image::Format::RGB16F: return PixelFormat::RGB16F;
        case image::Image::Format::RGBA16I: return PixelFormat::RGBA16I;
        case image::Image::Format::RGBA16U: return PixelFormat::RGBA16U;
        case image::Image::Format::RGBA16F: return PixelFormat::RGBA16F;
        case image::Image::Format::R16F: return PixelFormat::R16F;

        case image::Image::Format::R32I: return PixelFormat::R32I;
        case image::Image::Format::R32U: return PixelFormat::R32U;
        case image::Image::Format::R32F: return PixelFormat::R32F;
        case image::Image::Format::RG32I: return PixelFormat::RG32I;
        case image::Image::Format::RG32U: return PixelFormat::RG32U;
        case image::Image::Format::RG32F: return PixelFormat::RG32F;
        case image::Image::Format::RGB16_SNorm: return PixelFormat::RGB16_SNorm;
        case image::Image::Format::RGB32I: return PixelFormat::RGB32I;
        case image::Image::Format::RGB32U: return PixelFormat::RGB32U;
        case image::Image::Format::RGB32F: return PixelFormat::RGB32F;
        case image::Image::Format::RGBA8I: return PixelFormat::RGBA8I;
        case image::Image::Format::RGBA8U: return PixelFormat::RGBA8U;
        case image::Image::Format::RGBA32I: return PixelFormat::RGBA32I;
        case image::Image::Format::RGBA32U: return PixelFormat::RGBA32U;
        case image::Image::Format::RGBA32F: return PixelFormat::RGBA32F;

        default: return PixelFormat::Undefined;
    }

    return PixelFormat::Undefined;
}

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
void Texture::loadFromImage(const image::Image &image, std::optional<LoadOperation> op) {
    const auto total_size = [&image]() {
        auto size = core::ArraySize { 0u };
        for (auto i = 0u; i < image.mipLevels(); ++i) size += image.size(i);

        return size;
    }();

    auto staging_buffer = m_device->createStagingBuffer(total_size);

    auto fence = m_device->createFence();
    auto command_buffer =
        m_device->graphicsQueue().createCommandBuffer(CommandBufferLevel::Primary);

    command_buffer.begin(true);

    loadFromImage(image, command_buffer, staging_buffer, 0u, std::move(op));

    command_buffer.end();
    command_buffer.build();
    command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromImage(const image::Image &image,
                            render::CommandBuffer &command_buffer,
                            render::HardwareBuffer &staging_buffer,
                            core::UOffset offset,
                            std::optional<LoadOperation> _op) {
    STORM_EXPECTS(!m_non_owning_texture);

    auto _offset = 0u;
    for (auto i = 0u; i < image.mipLevels(); ++i) {
        staging_buffer.upload<core::Byte>(image.data(i), _offset);

        _offset += image.size(i);
    }

    const auto &image_extent = image.extent();

    const auto &op = _op.value_or(LoadOperation {});

    createTextureData({ .width = image_extent.width, .height = image_extent.height, .depth = 1u },
                      toPixelFormat(image.format()),
                      CreateOperation { .samples    = op.samples,
                                        .mip_levels = image.mipLevels(),
                                        .layers     = image_extent.depth,
                                        .usage      = op.usage });

    auto mip_offset = 0u;
    for (auto mip_level = 0u; mip_level < image.mipLevels(); ++mip_level) {
        const auto &image_extent = image.extent(mip_level);
        const auto _extent       = core::Extentu { .width  = image_extent.width,
                                             .height = image_extent.height,
                                             .depth  = 1u };

        fillMemory(image_extent.width * image_extent.height * image.channelCount() *
                       image.bytesPerChannel(),
                   _extent,
                   mip_level,
                   image_extent.depth,
                   command_buffer,
                   staging_buffer,
                   offset + mip_offset);

        mip_offset += image.size(mip_level);
    }
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::createTextureData(core::Extentu extent,
                                render::PixelFormat format,
                                std::optional<CreateOperation> _op) {
    auto op = _op.value_or(CreateOperation {});

    STORM_EXPECTS(op.mip_levels > 0);
    STORM_EXPECTS(op.layers > 0);

    m_samples    = op.samples;
    m_mip_levels = op.mip_levels;
    m_layers     = op.layers;
    m_format     = format;
    m_extent     = extent;

    const auto create_info =
        vk::ImageCreateInfo {}
            .setImageType(toVK(m_type))
            .setFormat(toVK(m_format))
            .setExtent(VkExtent3D { .width = m_extent.w, .height = m_extent.h, .depth = 1 })
            .setMipLevels(m_mip_levels)
            .setArrayLayers(m_layers)
            .setSamples(toVKBits(m_samples))
            .setTiling(vk::ImageTiling::eOptimal)
            .setUsage(toVK(op.usage))
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

/////////////////////////////////////
/////////////////////////////////////
void Texture::generateMipmap(render::CommandBuffer &cmb,
                             core::UInt32 mip_level,
                             core::UInt32 base_array_layer) {
    cmb.beginDebugRegion("Generate Mipmap", core::RGBColorDef::Maroon<float>);
    for (auto i = 1u; i < mip_level; ++i) {
        auto mip_subresource_range = TextureSubresourceRange {
            .base_mip_level   = i,
            .base_array_layer = base_array_layer,
        };
        auto region = render::BlitRegion { .source =
                                               render::TextureSubresourceLayers {
                                                   .mip_level        = i - 1u,
                                                   .base_array_layer = base_array_layer,
                                               },
                                           .destination =
                                               render::TextureSubresourceLayers {
                                                   .mip_level        = i,
                                                   .base_array_layer = base_array_layer,
                                               },
                                           .source_offset      = { core::Offset3u {},
                                                              core::Offset3u {
                                                                  .x = m_extent.width >> (i - 1u),
                                                                  .y = m_extent.height >> (i - 1u),
                                                                  .z = 1,
                                                              } },
                                           .destination_offset = { core::Offset3u {},
                                                                   core::Offset3u {
                                                                       .x = m_extent.width >> (i),
                                                                       .y = m_extent.height >> (i),
                                                                       .z = 1,
                                                                   } } };

        cmb.blitTexture(*this,
                        *this,
                        render::TextureLayout::Transfer_Src_Optimal,
                        render::TextureLayout::Transfer_Dst_Optimal,
                        { std::move(region) },
                        render::Filter::Linear);

        cmb.transitionTextureLayout(*this,
                                    TextureLayout::Transfer_Dst_Optimal,
                                    TextureLayout::Transfer_Src_Optimal,
                                    mip_subresource_range);
    }
    cmb.endDebugRegion();
}

void Texture::fillMemory(core::ArraySize layer_size,
                         core::Extentu layer_extent,
                         core::UInt8 mip_level,
                         core::UInt8 layers,
                         CommandBuffer &command_buffer,
                         render::HardwareBuffer &staging_buffer,
                         core::UOffset staging_offset) {
    auto copy_regions = std::vector<BufferTextureCopy> {};
    copy_regions.reserve(layers);

    auto offset = staging_offset;
    for (auto layer = 0u; layer < layers; ++layer) {
        copy_regions.emplace_back(BufferTextureCopy {
            .buffer_offset      = offset,
            .subresource_layers = { .mip_level = mip_level, .base_array_layer = layer },
            .extent             = layer_extent,
        });

        offset += layer_size;
    }

    auto subresource_range =
        TextureSubresourceRange { .level_count = m_mip_levels, .layer_count = m_layers };

    command_buffer.beginDebugRegion(fmt::format("Upload Texture mip {}", mip_level),
                                    core::RGBColorDef::Lime<float>);
    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Undefined,
                                           TextureLayout::Transfer_Dst_Optimal,
                                           subresource_range);

    command_buffer.copyBufferToTexture(staging_buffer, *this, std::move(copy_regions));

    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Transfer_Dst_Optimal,
                                           TextureLayout::Shader_Read_Only_Optimal,
                                           subresource_range);

    command_buffer.endDebugRegion();
}
