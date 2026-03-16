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
import :instance;

export namespace stormkit::gpu {
    class DebugCallback;

    namespace view {
        using DebugCallback = View<DebugCallback>;
    }

    namespace meta {
        template<>
        struct ObjectInfo<DebugCallback> {
            using Of          = DebugCallback;
            using ElementType = VkDebugUtilsMessengerEXT;
            using DeleterType = PFN_vkDestroyDebugUtilsMessengerEXT;
            using ViewType    = view::DebugCallback;
            using OwnedBy     = Instance;

            static constexpr auto DEBUG_TYPE = DebugObjectType::DEBUG_UTILS_MESSENGER;
        };
    } // namespace meta

    class STORMKIT_GPU_API DebugCallback: public OwnedByInstance<DebugCallback> {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::DEBUG_UTILS_MESSENGER;

        using Closure = PFN_vkDebugUtilsMessengerCallbackEXT;

        ~DebugCallback();

        DebugCallback(const DebugCallback&)                    = delete;
        auto operator=(const DebugCallback&) -> DebugCallback& = delete;

        DebugCallback(DebugCallback&&) noexcept;
        auto operator=(DebugCallback&&) noexcept -> DebugCallback&;

        // clang-format off
   // private:
        // clang-format on
        DebugCallback(PrivateTag, view::Instance) noexcept;
        auto do_init(PrivateTag, Closure, void* = nullptr) noexcept -> Expected<void>;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DebugCallback::DebugCallback(PrivateTag, view::Instance instance) noexcept
        : OwnedByInstance<DebugCallback> { std::move(instance), auto(vkDestroyDebugUtilsMessengerEXT) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DebugCallback::~DebugCallback() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DebugCallback::DebugCallback(DebugCallback&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DebugCallback::operator=(DebugCallback&&) noexcept -> DebugCallback& = default;

} // namespace stormkit::gpu
