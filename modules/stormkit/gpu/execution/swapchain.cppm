// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.execution:swapchain;

import std;

import stormkit.core;
import stormkit.gpu.core;
import stormkit.gpu.resource;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    class SwapChain;

    namespace view {
        class SwapChain;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<SwapChain> {
            using Of          = SwapChain;
            using ValueType = VkSwapchainKHR;
            using DeleterType = PFN_vkDestroySwapchainKHR VolkDeviceTable::*;
            using ViewType    = view::SwapChain;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::SWAPCHAIN;
        };
    } // namespace meta

    class STORMKIT_GPU_API SwapChain: public OwnedByDevice<SwapChain> {
      public:
        using ImageID = u32;

        struct NextImage {
            Result  result;
            ImageID id;
        };

        ~SwapChain() noexcept;

        SwapChain(const SwapChain&)                    = delete;
        auto operator=(const SwapChain&) -> SwapChain& = delete;

        SwapChain(SwapChain&&) noexcept;
        auto operator=(SwapChain&&) noexcept -> SwapChain&;

        [[nodiscard]]
        auto pixel_format() const noexcept -> PixelFormat;
        [[nodiscard]]
        auto images() const noexcept -> std::span<const Image>;
        auto acquire_next_image(std::chrono::nanoseconds wait, view::Semaphore image_available) const noexcept
          -> Expected<NextImage>;

        // clang-format off
  // private:
        // clang-format on
        SwapChain(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, view::Surface&&, const math::uextent2&, VkSwapchainKHR = VK_NULL_HANDLE) noexcept
          -> Expected<void>;

      private:
        math::uextent2 m_extent;
        PixelFormat    m_pixel_format;
        u32            m_image_count;

        std::vector<Image> m_images;
    };

    namespace view {
        class STORMKIT_GPU_API SwapChain: public DeviceObject<gpu::SwapChain> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::SwapChain>;
            using ValueType = ObjectInfo::ValueType;
            using ViewType    = ObjectInfo::ViewType;

            SwapChain(const gpu::SwapChain& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::SwapChain> T>
            SwapChain(const T& of) noexcept;
            ~SwapChain() noexcept;

            SwapChain(const SwapChain&) noexcept;
            auto operator=(const SwapChain&) noexcept -> SwapChain&;

            SwapChain(SwapChain&&) noexcept;
            auto operator=(SwapChain&&) noexcept -> SwapChain&;

            [[nodiscard]]
            auto pixel_format() const noexcept -> PixelFormat;
            [[nodiscard]]
            auto images() const noexcept -> std::span<const gpu::Image>;
            auto acquire_next_image(std::chrono::nanoseconds wait, Semaphore image_available) const noexcept
              -> Expected<gpu::SwapChain::NextImage>;

          private:
            math::uextent2 m_extent;
            PixelFormat    m_pixel_format;
            u32            m_image_count;

            std::span<const gpu::Image> m_images;
        };
    } // namespace view
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    STORMKIT_FORCE_INLINE
    inline SwapChain::SwapChain(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<SwapChain> { std::move(device), &VolkDeviceTable::vkDestroySwapchainKHR } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SwapChain::~SwapChain() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SwapChain::SwapChain(SwapChain&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SwapChain::operator=(SwapChain&&) noexcept -> SwapChain& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SwapChain::pixel_format() const noexcept -> PixelFormat {
        return m_pixel_format;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SwapChain::images() const noexcept -> std::span<const Image> {
        return m_images;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline SwapChain::SwapChain(const gpu::SwapChain& of) noexcept
            : view::DeviceObject<gpu::SwapChain> { of }, m_pixel_format { of.pixel_format() }, m_images { of.images() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::SwapChain> T>
        STORMKIT_FORCE_INLINE
        inline SwapChain::SwapChain(const T& of) noexcept
            : view::DeviceObject<gpu::SwapChain> { of }, m_pixel_format { of->pixel_format() }, m_images { of->images() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline SwapChain::~SwapChain() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline SwapChain::SwapChain(const SwapChain&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto SwapChain::operator=(const SwapChain&) noexcept -> SwapChain& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline SwapChain::SwapChain(SwapChain&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto SwapChain::operator=(SwapChain&&) noexcept -> SwapChain& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto SwapChain::pixel_format() const noexcept -> PixelFormat {
            return m_pixel_format;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto SwapChain::images() const noexcept -> std::span<const gpu::Image> {
            return m_images;
        }
    } // namespace view
} // namespace stormkit::gpu
