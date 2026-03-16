// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:sync;

import std;

import stormkit.core;

import :vulkan;
import :structs;
import :device;

export namespace stormkit::gpu {
    class Fence;
    class Semaphore;

    namespace view {
        class Fence;
        using Semaphore = DeviceObject<gpu::Semaphore>;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<Fence> {
            using Of          = Fence;
            using ValueType = VkFence;
            using DeleterType = PFN_vkDestroyFence VolkDeviceTable::*;
            using ViewType    = view::Fence;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::FENCE;
        };

        template<>
        struct ObjectInfo<Semaphore> {
            using Of          = Semaphore;
            using ValueType = VkSemaphore;
            using DeleterType = PFN_vkDestroySemaphore VolkDeviceTable::*;
            using ViewType    = view::Semaphore;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::SEMAPHORE;
        };
    } // namespace meta

    class STORMKIT_GPU_API Fence: public OwnedByDevice<Fence> {
      public:
        enum class Status {
            SIGNALED,
            UNSIGNALED,
        };

        static auto create_signaled(view::Device device) noexcept -> Expected<Fence>;
        static auto allocate_signaled(view::Device device) noexcept -> Expected<Heap<Fence>>;
        ~Fence();

        Fence(const Fence&)                    = delete;
        auto operator=(const Fence&) -> Fence& = delete;

        Fence(Fence&&) noexcept;
        auto operator=(Fence&&) noexcept -> Fence&;

        auto status() const noexcept -> Expected<Status>;
        auto wait(const std::chrono::milliseconds& wait_for = std::chrono::milliseconds::max()) const noexcept
          -> Expected<Result>;
        auto reset() const noexcept -> Expected<void>;

        // clang-format off
  // private:
        // clang-format on
        Fence(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag, bool = false) noexcept -> Expected<void>;
    };

    namespace view {
        class STORMKIT_GPU_API Fence: public view::DeviceObject<gpu::Fence> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Fence>;
            using ValueType = ObjectInfo::ValueType;
            using ViewType    = ObjectInfo::ViewType;

            using view::DeviceObject<gpu::Fence>::DeviceObject;

            auto status() const noexcept -> Expected<gpu::Fence::Status>;
            auto wait(const std::chrono::milliseconds& wait_for = std::chrono::milliseconds::max()) const noexcept
              -> Expected<Result>;
            auto reset() const noexcept -> Expected<void>;
        };
    } // namespace view

    class STORMKIT_GPU_API Semaphore: public OwnedByDevice<Semaphore> {
      public:
        ~Semaphore();

        Semaphore(const Semaphore&)                    = delete;
        auto operator=(const Semaphore&) -> Semaphore& = delete;

        Semaphore(Semaphore&&) noexcept;
        auto operator=(Semaphore&&) noexcept -> Semaphore&;

        // clang-format off
  // private:
        // clang-format on
        Semaphore(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag) noexcept -> Expected<void>;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Fence::Fence(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<Fence> { std::move(device), &VolkDeviceTable::vkDestroyFence } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Fence::create_signaled(view::Device device) noexcept -> Expected<Fence> {
        return create(std::move(device), true);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Fence::allocate_signaled(view::Device device) noexcept -> Expected<Heap<Fence>> {
        return allocate(std::move(device), true);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Fence::~Fence() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Fence::Fence(Fence&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Fence::operator=(Fence&&) noexcept -> Fence& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Semaphore::Semaphore(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<Semaphore> { std::move(device), &VolkDeviceTable::vkDestroySemaphore } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Semaphore::~Semaphore() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Semaphore::Semaphore(Semaphore&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Semaphore::operator=(Semaphore&&) noexcept -> Semaphore& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Device::wait_for_fence(view::Fence fence, const std::chrono::milliseconds& timeout) const noexcept
      -> Expected<Result> {
        return wait_for_fences(as_views(std::move(fence)), true, timeout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Device::reset_fence(view::Fence fence) const noexcept -> Expected<void> {
        return reset_fences(as_views(std::move(fence)));
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::wait_for_fence(view::Fence fence, const std::chrono::milliseconds& timeout) const noexcept
          -> Expected<Result> {
            return wait_for_fences(as_views(std::move(fence)), true, timeout);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::reset_fence(view::Fence fence) const noexcept -> Expected<void> {
            return reset_fences(as_views(std::move(fence)));
        }
    } // namespace view
} // namespace stormkit::gpu
