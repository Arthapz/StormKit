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
    }
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
void Texture::loadFromKTX(const std::filesystem::path &filepath) {
    auto image = gli::load(filepath.string());

    const auto faces      = static_cast<core::UInt32>(image.faces());
    const auto mip_count  = static_cast<core::UInt32>(image.levels());
    const auto format     = toPixelFormat(image.format());
    const auto tex_extent = core::Extenti { image.extent().x, image.extent().y, image.extent().z }
                                .convertTo<core::Extentu>();

    createTextureData(tex_extent, format, { .mip_levels = mip_count, .layers = faces });

    auto staging_buffer = m_device->createStagingBuffer(image.size());
    staging_buffer.upload<core::Byte>(
        { reinterpret_cast<core::Byte *>(image.data()), image.size() });

    auto copy_regions = std::vector<BufferTextureCopy> {};
    copy_regions.reserve(faces * mip_count);

    auto offset = 0u;
    for (auto face = 0u; face < faces; ++face) {
        for (auto level = 0u; level < mip_count; ++level) {
            const auto extent =
                core::Extentu { tex_extent.w >> level, tex_extent.h >> level, tex_extent.d };

            copy_regions.emplace_back(BufferTextureCopy {
                .buffer_offset      = offset,
                .subresource_layers = { .mip_level = level, .base_array_layer = face },
                .extent             = extent,
            });

            offset += image.size(level);
        }
    }

    auto subresource_range =
        TextureSubresourceRange { .level_count = m_mip_levels, .layer_count = m_layers };

    auto fence          = m_device->createFence();
    auto command_buffer = m_device->graphicsQueue().createCommandBuffer();
    command_buffer.begin(true);
    command_buffer.beginDebugRegion("KTX Upload Texture", core::RGBColorDef::Lime<float>);
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
    command_buffer.end();
    command_buffer.build();
    command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromImage(image::Image &image, LoadOperation op) {
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
                   { .width = image.extent().width, .height = image.extent().height },
                   format,
                   std::move(op));
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadLayersFromImages(std::vector<image::ImageConstObserverPtr> data,
                                   core::Extentu layer_extent,
                                   LoadOperation op) {
    auto bytes = std::vector<core::ByteConstSpan> {};
    bytes.reserve(std::size(data));

    for (const auto &img : data) bytes.emplace_back(std::data(*img));

    loadLayersFromMemory(std::move(bytes), layer_extent, std::move(op));
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromMemory(
    storm::core::span<const core::Byte> data,
    core::Extentu extent,
    [[maybe_unused]] render::PixelFormat load_format, // TODO convert to storage format
    LoadOperation op) {
    STORM_EXPECTS(!m_non_owning_texture);

    createTextureData(extent,
                      op.storage_format,
                      { .samples    = op.samples,
                        .mip_levels = op.mip_levels,
                        .layers     = 1,
                        .usage      = op.usage });

    auto staging_buffer =
        m_device->createStagingBuffer(gsl::narrow_cast<core::ArraySize>(std::size(data)));
    staging_buffer.upload<const core::Byte>(data);

    auto fence = m_device->createFence();

    const auto subresource_range = TextureSubresourceRange { .level_count = op.mip_levels };

    auto command_buffer = m_device->graphicsQueue().createCommandBuffer();
    command_buffer.begin(true);
    command_buffer.beginDebugRegion("Upload Texture", core::RGBColorDef::Lime<float>);
    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Undefined,
                                           TextureLayout::Transfer_Dst_Optimal,
                                           subresource_range);
    command_buffer.copyBufferToTexture(staging_buffer, *this);

    auto layout = TextureLayout::Transfer_Dst_Optimal;
    if (op.generate_mip_map) {
        command_buffer.transitionTextureLayout(*this,
                                               TextureLayout::Transfer_Dst_Optimal,
                                               TextureLayout::Transfer_Src_Optimal);
        layout = TextureLayout::Transfer_Src_Optimal;

        generateMipmap(command_buffer, op.mip_levels);
    }
    command_buffer.transitionTextureLayout(*this,
                                           layout,
                                           TextureLayout::Shader_Read_Only_Optimal,
                                           subresource_range);

    command_buffer.endDebugRegion();
    command_buffer.end();
    command_buffer.build();
    command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadLayersFromMemory(std::vector<core::ByteConstSpan> data,
                                   core::Extentu layer_extent,
                                   LoadOperation op) {
    STORM_EXPECTS(!m_non_owning_texture);

    createTextureData(layer_extent,
                      op.storage_format,
                      { .samples    = op.samples,
                        .mip_levels = op.mip_levels,
                        .layers     = gsl::narrow_cast<core::UInt32>(std::size(data)),
                        .usage      = op.usage });

    auto offsets          = std::vector<core::UOffset> {};
    const auto total_size = [&data, &offsets]() {
        auto s = core::ByteCount { 0u };
        for (const auto &d : data) {
            offsets.emplace_back(s);
            s += std::size(d);
        }

        return s;
    }();

    auto staging_buffer = m_device->createStagingBuffer(total_size);
    auto i              = 0u;

    for (const auto offset : offsets) {
        staging_buffer.upload<const core::Byte>(data[i++], offset);
    }

    auto fence = m_device->createFence();

    auto copy_command_buffer = m_device->graphicsQueue().createCommandBuffer();
    copy_command_buffer.begin(true);
    copy_command_buffer.beginDebugRegion("Upload Texture", core::RGBColorDef::Lime<float>);

    i = 0u;
    for (const auto offset : offsets) {
        const auto subresource_range =
            TextureSubresourceRange { .level_count = op.mip_levels, .base_array_layer = i };

        copy_command_buffer.transitionTextureLayout(*this,
                                                    TextureLayout::Undefined,
                                                    TextureLayout::Transfer_Dst_Optimal,
                                                    subresource_range);

        auto copy = render::BufferTextureCopy { .buffer_offset      = offset,
                                                .subresource_layers = { .base_array_layer = i },
                                                .extent             = layer_extent };

        copy_command_buffer.copyBufferToTexture(staging_buffer, *this, { copy });

        auto layout = TextureLayout::Transfer_Dst_Optimal;
        if (op.generate_mip_map) {
            const auto first_subresource_range = TextureSubresourceRange { .base_array_layer = i };
            copy_command_buffer.transitionTextureLayout(*this,
                                                        TextureLayout::Transfer_Dst_Optimal,
                                                        TextureLayout::Transfer_Src_Optimal,
                                                        first_subresource_range);
            layout = TextureLayout::Transfer_Src_Optimal;

            generateMipmap(copy_command_buffer, op.mip_levels, i);
        }
        copy_command_buffer.transitionTextureLayout(*this,
                                                    layout,
                                                    TextureLayout::Shader_Read_Only_Optimal,
                                                    subresource_range);

        ++i;
    }

    copy_command_buffer.endDebugRegion();
    copy_command_buffer.end();
    copy_command_buffer.build();
    copy_command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::createTextureData(core::Extentu extent,
                                render::PixelFormat format,
                                CreateOperation op) {
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
