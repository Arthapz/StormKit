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

import :vulkan;
import :base;

import :structs;
import :objects;

import :instance;

namespace stormkit::gpu {
    export template<typename Base>
    class SurfaceInterface final: public InstanceObject<Base> {
      public:
        using InstanceObject<Base>::InstanceObject;
        using InstanceObject<Base>::operator=;
        using TagType = SurfaceTag;
    };

    class STORMKIT_GPU_API SurfaceImplementation: public GpuObjectImplementation<SurfaceTag> {
      public:
        SurfaceImplementation(PrivateTag, view::Instance&&) noexcept;
        auto do_init(PrivateTag) noexcept -> Expected<void>;
        auto do_init(PrivateTag, const wsi::Window&) noexcept -> Expected<void>;
        ~SurfaceImplementation() noexcept;

        SurfaceImplementation(const SurfaceImplementation&) noexcept                    = delete;
        auto operator=(const SurfaceImplementation&) noexcept -> SurfaceImplementation& = delete;

        SurfaceImplementation(SurfaceImplementation&&) noexcept;
        auto operator=(SurfaceImplementation&&) noexcept -> SurfaceImplementation&;

#if false
        [[nodiscard]]
        static auto create_offscreen(view::Instance instance) noexcept 
          -> Expected<Surface>;
        [[nodiscard]]
        static auto allocate_offscreen(view::Instance instance) noexcept
          -> Expected<Heap<Surface>>;
#endif

        [[nodiscard]]
        static auto create_from_window(view::Instance instance, const wsi::Window& window) noexcept -> Expected<Surface>;
        [[nodiscard]]
        static auto allocate_from_window(view::Instance instance, const wsi::Window& window) noexcept -> Expected<Heap<Surface>>;

      private:
        using UseNamedConstructors::allocate;
        using UseNamedConstructors::create;
    };

    namespace view {
        class SurfaceImplementation: public GpuObjectViewImplementation<SurfaceTag> {
          public:
            using GpuObjectViewImplementation<SurfaceTag>::GpuObjectViewImplementation;
            using GpuObjectViewImplementation<SurfaceTag>::operator=;
        };
    } // namespace view
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SurfaceImplementation::SurfaceImplementation(PrivateTag, view::Instance&& instance) noexcept
        : GpuObjectImplementation { std::move(instance), auto(vkDestroySurfaceKHR) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SurfaceImplementation::~SurfaceImplementation() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SurfaceImplementation::SurfaceImplementation(SurfaceImplementation&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SurfaceImplementation::operator=(SurfaceImplementation&&) noexcept -> SurfaceImplementation& = default;

#if false
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE inline auto SurfaceImplementation::create_offscreen(view::Instance instance) noexcept
        -> Expected<Surface> {
        return UseNamedConstructors::create(std::move(instance));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE inline auto SurfaceImplementation::allocate_offscreen(view::Instance instance) noexcept
        -> Expected<Heap<Surface>> {
        return UseNamedConstructors::allocate(std::move(instance));
    }
#endif

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SurfaceImplementation::create_from_window(view::Instance instance, const wsi::Window& window) noexcept
      -> Expected<Surface> {
        return UseNamedConstructors::create(std::move(instance), window);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SurfaceImplementation::allocate_from_window(view::Instance instance, const wsi::Window& window) noexcept
      -> Expected<Heap<Surface>> {
        return UseNamedConstructors::allocate(std::move(instance), window);
    }
} // namespace stormkit::gpu
