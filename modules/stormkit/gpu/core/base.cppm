// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:base;

import std;

import stormkit.core;

import :meta;
import :structs;
import :vulkan;

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    STORMKIT_GPU_API
    auto initialize_backend() -> expected<void>;

    template<typename T, typename Interface>
    struct gpu_object_trait {
        using element_type   = T;
        using interface_type = Interface;

        static constexpr auto null_handle = VK_NULL_HANDLE;
        static constexpr auto deleter     = monadic::discard();
    };

    struct gpu_object_interface {
        [[nodiscard]]
        auto native_handle(this const auto& self) noexcept -> decltype(auto);
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
} // namespace stormkit::gpu
