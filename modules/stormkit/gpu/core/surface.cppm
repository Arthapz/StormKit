// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:surface;

import std;

import stormkit.core;
import stormkit.wsi;

import :base;
import :vulkan;
import :structs;
import :instance;

export namespace stormkit::gpu {
    class Surface;

    namespace view {
        using Surface = InstanceObject<gpu::Surface>;
    }

    namespace meta {
        template<>
        struct ObjectInfo<Surface> {
            using Of          = Surface;
            using ValueType   = VkSurfaceKHR;
            using DeleterType = PFN_vkDestroySurfaceKHR;
            using ViewType    = view::Surface;
            using OwnedBy     = Instance;

            static constexpr auto DEBUG_TYPE = DebugObjectType::SURFACE;
        };
    } // namespace meta

    class STORMKIT_GPU_API Surface: public OwnedByInstance<Surface> {
      public:
        ~Surface();

        Surface(const Surface&)                    = delete;
        auto operator=(const Surface&) -> Surface& = delete;

        Surface(Surface&&) noexcept;
        auto operator=(Surface&&) noexcept -> Surface&;

#if false
        [[nodiscard]]
        static auto create_offscreen(view::Instance instance) noexcept -> Expected<Surface>;
        [[nodiscard]]
        static auto allocate_offscreen(view::Instance instance) noexcept
            -> Expected<Heap<Surface>>;
#endif

        [[nodiscard]]
        static auto create_from_window(view::Instance instance, const wsi::Window& window) noexcept -> Expected<Surface>;
        [[nodiscard]]
        static auto allocate_from_window(view::Instance instance, const wsi::Window& window) noexcept -> Expected<Heap<Surface>>;

        // clang-format off
   // private:
        // clang-format on
        Surface(PrivateTag, view::Instance) noexcept;
        auto do_init(PrivateTag) noexcept -> Expected<void>;
        auto do_init(PrivateTag, const wsi::Window&) noexcept -> Expected<void>;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Surface::Surface(PrivateTag, view::Instance instance) noexcept
        : OwnedByInstance<Surface> { std::move(instance), auto(vkDestroySurfaceKHR) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Surface::~Surface() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Surface::Surface(Surface&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Surface::operator=(Surface&&) noexcept -> Surface& = default;

#if false
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE inline auto Surface::create_offscreen(view::Instance instance) noexcept
        -> Expected<Surface> {
        return create(std::move(instance));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE inline auto Surface::allocate_offscreen(view::Instance instance) noexcept
        -> Expected<Heap<Surface>> {
        return allocate(std::move(instance));
    }
#endif

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Surface::create_from_window(view::Instance instance, const wsi::Window& window) noexcept -> Expected<Surface> {
        return create(std::move(instance), window);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Surface::allocate_from_window(view::Instance instance, const wsi::Window& window) noexcept
      -> Expected<Heap<Surface>> {
        return allocate(std::move(instance), window);
    }
} // namespace stormkit::gpu
