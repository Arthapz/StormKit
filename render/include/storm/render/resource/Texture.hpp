// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/image/Image.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/TextureView.hpp>

namespace storm::render {
    class STORM_PUBLIC Texture: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Image;

        Texture(const Device &device,
                TextureType type        = TextureType::T2D,
                TextureCreateFlag flags = TextureCreateFlag::None);
        Texture(const Device &device,
                core::Extentu extent,
                render::PixelFormat format,
                vk::Image image);
        ~Texture();

        Texture(Texture &&);
        Texture &operator=(Texture &&);

        struct LoadOperation {
            SampleCountFlag samples = SampleCountFlag::C1_BIT;
            TextureUsage usage =
                TextureUsage::Sampled | TextureUsage::Transfert_Dst | TextureUsage::Transfert_Src;
        };

        struct CreateOperation {
            SampleCountFlag samples = SampleCountFlag::C1_BIT;
            core::UInt32 mip_levels = 1;
            core::UInt32 layers     = 1;
            TextureUsage usage =
                TextureUsage::Sampled | TextureUsage::Transfert_Dst | TextureUsage::Transfert_Src;
        };

        void loadFromImage(const image::Image &image,
                           std::optional<LoadOperation> op = std::nullopt);

        void loadFromImage(render::CommandBuffer &cmb,
                           const image::Image &image,
                           std::optional<LoadOperation> op = std::nullopt);

        void loadFromMemory(std::vector<core::ByteConstSpan> data,
                            core::Extentu layer_extent,
                            core::UInt8 layers              = 1u,
                            std::optional<LoadOperation> op = std::nullopt);

        void loadFromMemory(render::CommandBuffer &cmb,
                            std::vector<core::ByteConstSpan> data,
                            core::Extentu layer_extent,
                            core::UInt8 layers              = 1u,
                            std::optional<LoadOperation> op = std::nullopt);

        void createTextureData(core::Extentu extent,
                               PixelFormat format,
                               std::optional<CreateOperation> op = std::nullopt);

        TextureView createView(TextureViewType type                      = TextureViewType::T2D,
                               TextureSubresourceRange subresource_range = {}) const noexcept;
        TextureViewOwnedPtr createViewPtr(TextureViewType type = TextureViewType::T2D,
                                          TextureSubresourceRange subresource_range = {}) const;

        void generateMipmap(render::CommandBuffer &cmb,
                            core::UInt32 mip_level,
                            core::UInt32 base_array_layer = 0u);

        inline core::Extentu extent() const noexcept;
        inline PixelFormat format() const noexcept;
        inline TextureType type() const noexcept;
        inline SampleCountFlag samples() const noexcept;
        inline core::UInt32 mipLevels() const noexcept;
        inline core::UInt32 layers() const noexcept;

        inline const Device &device() const noexcept;

        inline vk::Image vkImage() const noexcept;
        inline operator vk::Image() const noexcept;
        inline vk::Image vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        void fillMemory(render::CommandBuffer &command_buffer,
                        core::ByteConstSpan data,
                        core::Extentu layer_extent,
                        core::UInt8 mip_level,
                        core::UInt8 layers);

        DeviceConstObserverPtr m_device;

        core::Extentu m_extent = { { 0 }, { 0 } };
        PixelFormat m_format   = PixelFormat::Undefined;
        TextureType m_type;
        TextureCreateFlag m_flags;

        SampleCountFlag m_samples = SampleCountFlag::C1_BIT;
        core::UInt32 m_mip_levels = 1;
        core::UInt32 m_layers     = 1;

        RAIIVmaAllocation m_vma_texture_memory;
        RAIIVkImage m_vk_texture;
        vk::Image m_non_owning_texture = VK_NULL_HANDLE;
    };
} // namespace storm::render

#include "Texture.inl"
