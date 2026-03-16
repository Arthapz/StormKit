// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.resource:image;

import std;

import stormkit.core;
import stormkit.image;
import stormkit.gpu.core;

namespace cmeta    = stormkit::core::meta;
namespace cmonadic = stormkit::core::monadic;

export namespace stormkit::gpu {
    class Sampler;
    class ImageView;
    class Image;

    namespace view {
        class Sampler;
        class ImageView;
        class Image;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<Sampler> {
            using Of          = Sampler;
            using ElementType = VkSampler;
            using DeleterType = PFN_vkDestroySampler VolkDeviceTable::*;
            using ViewType    = view::Sampler;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::SAMPLER;
        };

        template<>
        struct ObjectInfo<ImageView> {
            using Of          = ImageView;
            using ElementType = VkImageView;
            using DeleterType = PFN_vkDestroyImageView VolkDeviceTable::*;
            using ViewType    = view::ImageView;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::IMAGE_VIEW;
        };

        template<>
        struct ObjectInfo<Image> {
            using Of          = Image;
            using ElementType = VkImage;
            using DeleterType = PFN_vkDestroyImage VolkDeviceTable::*;
            using ViewType    = view::Image;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::IMAGE;
        };
    } // namespace meta

    class STORMKIT_GPU_API Sampler: public OwnedByDevice<Sampler> {
      public:
        struct Settings {
            Filter mag_filter = Filter::LINEAR;
            Filter min_filter = Filter::LINEAR;

            SamplerAddressMode address_mode_u = SamplerAddressMode::REPEAT;
            SamplerAddressMode address_mode_v = SamplerAddressMode::REPEAT;
            SamplerAddressMode address_mode_w = SamplerAddressMode::REPEAT;

            bool enable_anisotropy = false;
            f32  max_anisotropy    = 0.f;

            BorderColor border_color = BorderColor::INT_OPAQUE_BLACK;

            bool unnormalized_coordinates = false;

            bool             compare_enable    = false;
            CompareOperation compare_operation = CompareOperation::ALWAYS;

            SamplerMipmapMode mipmap_mode  = SamplerMipmapMode::LINEAR;
            f32               mip_lod_bias = 0.f;

            f32 min_lod = 0.f;
            f32 max_lod = 0.f;
        };

        ~Sampler();

        Sampler(const Sampler&)                    = delete;
        auto operator=(const Sampler&) -> Sampler& = delete;

        Sampler(Sampler&&) noexcept;
        auto operator=(Sampler&&) noexcept -> Sampler&;

        [[nodiscard]]
        auto settings() const noexcept -> const Settings&;

        // clang-format off
  // private:
        // clang-format on
        Sampler(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag, const Settings&) noexcept -> Expected<void>;

      private:
        Settings m_settings = {};
    };

    namespace view {
        class STORMKIT_GPU_API Sampler: public view::DeviceObject<gpu::Sampler> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Sampler>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            Sampler(const gpu::Sampler& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Sampler> T>
            Sampler(const T& of) noexcept;
            ~Sampler() noexcept;

            Sampler(const Sampler&) noexcept;
            auto operator=(const Sampler&) noexcept -> Sampler&;

            Sampler(Sampler&&) noexcept;
            auto operator=(Sampler&&) noexcept -> Sampler&;

            [[nodiscard]]
            auto settings() const noexcept -> const gpu::Sampler::Settings&;

          private:
            gpu::Sampler::Settings m_settings;
        };
    } // namespace view

    class STORMKIT_GPU_API ImageView: public OwnedByDevice<ImageView> {
      public:
        ~ImageView();

        ImageView(const ImageView&)                    = delete;
        auto operator=(const ImageView&) -> ImageView& = delete;

        ImageView(ImageView&&) noexcept;
        auto operator=(ImageView&&) noexcept -> ImageView&;

        [[nodiscard]]
        auto type() const noexcept -> ImageViewType;
        [[nodiscard]]
        auto subresource_range() const noexcept -> const ImageSubresourceRange&;

        // clang-format off
  // private:
        // clang-format on
        ImageView(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag, view::Image, ImageViewType = ImageViewType::T2D, const ImageSubresourceRange& = {}) noexcept
          -> Expected<void>;

      private:
        ImageViewType         m_type              = {};
        ImageSubresourceRange m_subresource_range = {};
    };

    namespace view {
        class STORMKIT_GPU_API ImageView: public view::DeviceObject<gpu::ImageView> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::ImageView>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            ImageView(const gpu::ImageView& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::ImageView> T>
            ImageView(const T& of) noexcept;
            ~ImageView() noexcept;

            ImageView(const ImageView&) noexcept;
            auto operator=(const ImageView&) noexcept -> ImageView&;

            ImageView(ImageView&&) noexcept;
            auto operator=(ImageView&&) noexcept -> ImageView&;

            [[nodiscard]]
            auto type() const noexcept -> ImageViewType;
            [[nodiscard]]
            auto subresource_range() const noexcept -> const ImageSubresourceRange&;

          private:
            ImageViewType         m_type              = {};
            ImageSubresourceRange m_subresource_range = {};
        };
    } // namespace view

    class STORMKIT_GPU_API Image: public OwnedByDevice<Image> {
      public:
        struct CreateInfo {
            math::uextent3     extent;
            PixelFormat        format     = PixelFormat::RGBA8_UNORM;
            u32                layers     = 1u;
            u32                mip_levels = 1u;
            ImageType          type       = ImageType::T2D;
            ImageCreateFlag    flags      = ImageCreateFlag::NONE;
            SampleCountFlag    samples    = SampleCountFlag::C1;
            ImageUsageFlag     usages     = ImageUsageFlag::SAMPLED | ImageUsageFlag::TRANSFER_DST | ImageUsageFlag::TRANSFER_SRC;
            ImageTiling        tiling     = ImageTiling::OPTIMAL;
            MemoryPropertyFlag properties = MemoryPropertyFlag::DEVICE_LOCAL;
        };

        static auto from_existing(view::Device device, const CreateInfo& create_info, VkImage image) noexcept -> Image;
        ~Image();

        Image(const Image&)                    = delete;
        auto operator=(const Image&) -> Image& = delete;

        Image(Image&&) noexcept;
        auto operator=(Image&&) noexcept -> Image&;

        [[nodiscard]]
        auto extent() const noexcept -> const math::uextent3&;
        [[nodiscard]]
        auto format() const noexcept -> PixelFormat;
        [[nodiscard]]
        auto type() const noexcept -> ImageType;
        [[nodiscard]]
        auto samples() const noexcept -> SampleCountFlag;
        [[nodiscard]]
        auto layers() const noexcept -> u32;
        [[nodiscard]]
        auto faces() const noexcept -> u32;
        [[nodiscard]]
        auto mip_levels() const noexcept -> u32;
        [[nodiscard]]
        auto usages() const noexcept -> ImageUsageFlag;
        [[nodiscard]]
        auto allocation() const noexcept -> vk::Observer<VmaAllocation>;

        // clang-format off
  // private:
        // clang-format on
        Image(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag, const CreateInfo&) noexcept -> Expected<void>;

      private:
        bool m_no_delete = false;

        math::uextent3  m_extent     = { 0, 0, 0 };
        PixelFormat     m_format     = {};
        u32             m_layers     = 0;
        u32             m_faces      = 0;
        u32             m_mip_levels = 0;
        ImageType       m_type       = {};
        ImageCreateFlag m_flags      = {};
        SampleCountFlag m_samples    = {};
        ImageUsageFlag  m_usages     = {};

        vk::Owned<VmaAllocation> m_vma_allocation = { cmonadic::discard() };

        friend class view::Image;
    };

    namespace view {
        class STORMKIT_GPU_API Image: public view::DeviceObject<gpu::Image> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Image>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            Image(const gpu::Image& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Image> T>
            Image(const T& of) noexcept;
            ~Image() noexcept;

            Image(const Image&) noexcept;
            auto operator=(const Image&) noexcept -> Image&;

            Image(Image&&) noexcept;
            auto operator=(Image&&) noexcept -> Image&;

            [[nodiscard]]
            auto extent() const noexcept -> const math::uextent3&;
            [[nodiscard]]
            auto format() const noexcept -> PixelFormat;
            [[nodiscard]]
            auto type() const noexcept -> ImageType;
            [[nodiscard]]
            auto samples() const noexcept -> SampleCountFlag;
            [[nodiscard]]
            auto layers() const noexcept -> u32;
            [[nodiscard]]
            auto faces() const noexcept -> u32;
            [[nodiscard]]
            auto mip_levels() const noexcept -> u32;
            [[nodiscard]]
            auto usages() const noexcept -> ImageUsageFlag;
            [[nodiscard]]
            auto allocation() const noexcept -> vk::Observer<VmaAllocation>;

          private:
            math::uextent3 m_extent     = { 0, 0, 0 };
            PixelFormat    m_format     = {};
            u32            m_layers     = 0;
            u32            m_faces      = 0;
            u32            m_mip_levels = 0;
            ImageType      m_type       = {};
            STORMKIT_PUSH_WARNINGS
#pragma clang diagnostic ignored "-Wunused-private-field"
            ImageCreateFlag m_flags = {};
            STORMKIT_POP_WARNINGS
            SampleCountFlag m_samples = {};
            ImageUsageFlag  m_usages  = {};

            vk::Observer<VmaAllocation> m_vma_allocation = VK_NULL_HANDLE;
        };
    } // namespace view

    struct ImageMemoryBarrier {
        AccessFlag src;
        AccessFlag dst;

        ImageLayout old_layout;
        ImageLayout new_layout;

        u32 src_queue_family_index = QUEUE_FAMILY_IGNORED;
        u32 dst_queue_family_index = QUEUE_FAMILY_IGNORED;

        const Image&          image;
        ImageSubresourceRange range;
    };

    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const Image::CreateInfo& value) noexcept -> Ret;
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Sampler::Sampler(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<Sampler> { std::move(device), &VolkDeviceTable::vkDestroySampler } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Sampler::~Sampler() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Sampler::Sampler(Sampler&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Sampler::operator=(Sampler&&) noexcept -> Sampler& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Sampler::settings() const noexcept -> const Settings& {
        return m_settings;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Sampler::Sampler(const gpu::Sampler& of) noexcept
            : view::DeviceObject<gpu::Sampler> { of }, m_settings { of.settings() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Sampler> T>
        STORMKIT_FORCE_INLINE
        inline Sampler::Sampler(const T& of) noexcept
            : view::DeviceObject<gpu::Sampler> { of }, m_settings { of->settings() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Sampler::~Sampler() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Sampler::Sampler(const Sampler&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Sampler::operator=(const Sampler&) noexcept -> Sampler& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Sampler::Sampler(Sampler&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Sampler::operator=(Sampler&&) noexcept -> Sampler& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Sampler::settings() const noexcept -> const gpu::Sampler::Settings& {
            return m_settings;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline ImageView::ImageView(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<ImageView> { std::move(device), &VolkDeviceTable::vkDestroyImageView } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline ImageView::~ImageView() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline ImageView::ImageView(ImageView&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto ImageView::operator=(ImageView&&) noexcept -> ImageView& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto ImageView::type() const noexcept -> ImageViewType {
        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto ImageView::subresource_range() const noexcept -> const ImageSubresourceRange& {
        return m_subresource_range;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline ImageView::ImageView(const gpu::ImageView& of) noexcept
            : view::DeviceObject<gpu::ImageView> { of }, m_type { of.type() }, m_subresource_range { of.subresource_range() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::ImageView> T>
        STORMKIT_FORCE_INLINE
        inline ImageView::ImageView(const T& of) noexcept
            : view::DeviceObject<gpu::ImageView> { of }, m_type { of.type() }, m_subresource_range { of.subresource_range() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline ImageView::~ImageView() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline ImageView::ImageView(const ImageView&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto ImageView::operator=(const ImageView&) noexcept -> ImageView& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline ImageView::ImageView(ImageView&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto ImageView::operator=(ImageView&&) noexcept -> ImageView& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto ImageView::type() const noexcept -> ImageViewType {
            return m_type;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto ImageView::subresource_range() const noexcept -> const ImageSubresourceRange& {
            return m_subresource_range;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Image::Image(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<Image> { std::move(device), &VolkDeviceTable::vkDestroyImage } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Image::~Image() {
        if (not m_no_delete) [[unlikely]]
            if (m_vk_handle != VK_NULL_HANDLE) {
                const auto& device = this->device();
                vk::call(device.device_table().*m_deleter_ptr, device, m_vk_handle, nullptr);
            }

        m_vk_handle = VK_NULL_HANDLE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Image::Image(Image&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::operator=(Image&&) noexcept -> Image& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::extent() const noexcept -> const math::uextent3& {
        return m_extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::format() const noexcept -> PixelFormat {
        return m_format;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::type() const noexcept -> ImageType {
        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::samples() const noexcept -> SampleCountFlag {
        return m_samples;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::layers() const noexcept -> u32 {
        return m_layers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::faces() const noexcept -> u32 {
        return m_faces;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::mip_levels() const noexcept -> u32 {
        return m_mip_levels;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::usages() const noexcept -> ImageUsageFlag {
        return m_usages;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Image::allocation() const noexcept -> vk::Observer<VmaAllocation> {
        EXPECTS(m_vma_allocation != VK_NULL_HANDLE);
        return m_vma_allocation;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::from_existing(view::Device device, const CreateInfo& create_info, VkImage vk_image) noexcept -> Image {
        auto image         = Image { PrivateTag {}, std::move(device) };
        image.m_extent     = create_info.extent;
        image.m_format     = create_info.format;
        image.m_layers     = create_info.layers;
        image.m_faces      = 1;
        image.m_mip_levels = create_info.mip_levels;
        image.m_type       = create_info.type;
        image.m_flags      = create_info.flags;
        image.m_samples    = create_info.samples;
        image.m_usages     = create_info.usages;

        image.m_vma_allocation = { core::monadic::noop() };
        image.m_vk_handle      = std::move(vk_image);
        image.m_no_delete      = true;

        return image;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Image::Image(const gpu::Image& of) noexcept
            : view::DeviceObject<gpu::Image> { of },
              m_extent { of.extent() },
              m_format { of.format() },
              m_layers { of.layers() },
              m_faces { of.faces() },
              m_mip_levels { of.mip_levels() },
              m_type { of.type() },
              m_flags { of.m_flags },
              m_samples { of.samples() },
              m_usages { of.usages() } {
            if (not of.m_no_delete) m_vma_allocation = of.allocation();
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Image> T>
        STORMKIT_FORCE_INLINE
        inline Image::Image(const T& of) noexcept
            : view::DeviceObject<gpu::Image> { of },
              m_extent { of->extent() },
              m_format { of->format() },
              m_layers { of->layers() },
              m_faces { of->faces() },
              m_mip_levels { of->mip_levels() },
              m_type { of->type() },
              m_flags { of->m_flags },
              m_samples { of->samples() },
              m_usages { of->usages() },
              m_vma_allocation { of->allocation() } {
            if (not of->m_no_delete) m_vma_allocation = of->allocation();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Image::~Image() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Image::Image(const Image&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::operator=(const Image&) noexcept -> Image& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Image::Image(Image&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::operator=(Image&&) noexcept -> Image& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::extent() const noexcept -> const math::uextent3& {
            return m_extent;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::format() const noexcept -> PixelFormat {
            return m_format;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::type() const noexcept -> ImageType {
            return m_type;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::samples() const noexcept -> SampleCountFlag {
            return m_samples;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::layers() const noexcept -> u32 {
            return m_layers;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::faces() const noexcept -> u32 {
            return m_faces;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::mip_levels() const noexcept -> u32 {
            return m_mip_levels;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::usages() const noexcept -> ImageUsageFlag {
            return m_usages;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Image::allocation() const noexcept -> vk::Observer<VmaAllocation> {
            EXPECTS(m_vma_allocation != VK_NULL_HANDLE);
            return m_vma_allocation;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const Image::CreateInfo& create_info) noexcept -> Ret {
        return hash(create_info.extent,
                    create_info.format,
                    create_info.layers,
                    create_info.mip_levels,
                    create_info.type,
                    create_info.flags,
                    create_info.samples,
                    create_info.usages,
                    create_info.tiling,
                    create_info.properties);
    }
} // namespace stormkit::gpu
