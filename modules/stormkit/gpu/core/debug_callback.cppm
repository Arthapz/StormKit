// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:debug_callback;

import std;

import stormkit.core;

import :base;
import :vulkan;
import :structs;
import :objects;
import :instance;

namespace stormkit::gpu {
    export template<typename Base>
    class DebugCallbackInterface final: public InstanceObject<Base> {
      public:
        using InstanceObject<Base>::InstanceObject;
        using InstanceObject<Base>::operator=;
        using TagType = DebugCallbackTag;
    };

    class STORMKIT_GPU_API DebugCallbackImplementation: public GpuObjectImplementation<DebugCallbackTag> {
      public:
        using Closure = PFN_vkDebugUtilsMessengerCallbackEXT;

        DebugCallbackImplementation(PrivateTag, view::Instance&&) noexcept;
        auto do_init(PrivateTag, Closure messenger_closure, void* user_data = nullptr) noexcept -> Expected<void>;
        ~DebugCallbackImplementation() noexcept;

        DebugCallbackImplementation(const DebugCallbackImplementation&) noexcept                    = delete;
        auto operator=(const DebugCallbackImplementation&) noexcept -> DebugCallbackImplementation& = delete;

        DebugCallbackImplementation(DebugCallbackImplementation&&) noexcept;
        auto operator=(DebugCallbackImplementation&&) noexcept -> DebugCallbackImplementation&;
    };

    namespace view {
        class DebugCallbackImplementation: public GpuObjectViewImplementation<DebugCallbackTag> {
          public:
            using GpuObjectViewImplementation<DebugCallbackTag>::GpuObjectViewImplementation;
            using GpuObjectViewImplementation<DebugCallbackTag>::operator=;
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
    inline DebugCallbackImplementation::DebugCallbackImplementation(PrivateTag, view::Instance&& instance) noexcept
        : GpuObjectImplementation { std::move(instance), auto(vkDestroyDebugUtilsMessengerEXT) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DebugCallbackImplementation::~DebugCallbackImplementation() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DebugCallbackImplementation::DebugCallbackImplementation(DebugCallbackImplementation&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DebugCallbackImplementation::operator=(DebugCallbackImplementation&&) noexcept
      -> DebugCallbackImplementation& = default;
} // namespace stormkit::gpu
