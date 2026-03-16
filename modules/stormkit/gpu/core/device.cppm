// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:device;

import std;

import stormkit.core;

import :vulkan;
import :base;
import :physical_device;
import :structs;
import :instance;

namespace cmeta = stormkit::core::meta;

using namespace stormkit;

export namespace stormkit::gpu {
    namespace view {
        class Device;
        class Fence;
    } // namespace view

    class Device;

    namespace meta {
        template<>
        struct ObjectInfo<Device> {
            using Of          = Device;
            using ElementType = VkDevice;
            using DeleterType = PFN_vkDestroyDevice VolkDeviceTable::*;
            using ViewType    = view::Device;
            using OwnedBy     = PhysicalDevice;

            static constexpr auto DEBUG_TYPE = DebugObjectType::DEVICE;
        };
    } // namespace meta

    class STORMKIT_GPU_API Device: public OwnedByPhysicalDevice<Device> {
      public:
        struct QueueEntry {
            u32       id;
            u32       count;
            QueueFlag flags = QueueFlag {};
        };

        struct CreateInfo {
            bool enable_swapchain  = true;
            bool enable_raytracing = false;
        };

        ~Device() noexcept;

        Device(const Device&)                    = delete;
        auto operator=(const Device&) -> Device& = delete;

        Device(Device&&) noexcept;
        auto operator=(Device&&) noexcept -> Device&;

        auto wait_idle() const noexcept -> Expected<void>;

        auto wait_for_fences(std::span<const view::Fence>     fences,
                             bool                             wait_all = true,
                             const std::chrono::milliseconds& timeout  = std::chrono::milliseconds::max()) const noexcept
          -> Expected<Result>;
        auto wait_for_fence(view::Fence                      fence,
                            const std::chrono::milliseconds& timeout = std::chrono::milliseconds::max()) const noexcept
          -> Expected<Result>;

        auto reset_fences(std::span<const view::Fence> fences) const noexcept -> Expected<void>;
        auto reset_fence(view::Fence fence) const noexcept -> Expected<void>;

        template<meta::IsOwnedOrView T>
        auto set_object_name(const T& object, std::string_view name) const -> Expected<void>;

        auto set_object_name(u64 object, DebugObjectType type, std::string_view name) const -> Expected<void>;

        [[nodiscard]]
        auto queue_entries() const noexcept -> const std::vector<QueueEntry>&;

        [[nodiscard]]
        auto device_table() const noexcept -> const VolkDeviceTable&;

        [[nodiscard]]
        auto allocator() const noexcept -> vk::Observer<VmaAllocator>;

        Device(PrivateTag, view::PhysicalDevice) noexcept;
        auto do_init(PrivateTag, const CreateInfo& = { true, false }) noexcept -> Expected<void>;

      private:
        std::vector<QueueEntry> m_queue_entries;

        VolkDeviceTable         m_vk_device_table    = {};
        VmaVulkanFunctions      m_vma_function_table = {};
        vk::Owned<VmaAllocator> m_vma_allocator      = { vmaDestroyAllocator };
    };

    namespace view {
        class STORMKIT_GPU_API Device: public PhysicalDeviceObject<gpu::Device> {
          public:
            Device(const gpu::Device& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Device> T>
            Device(const T& of) noexcept;
            ~Device() noexcept;

            Device(const Device&) noexcept;
            auto operator=(const Device&) noexcept -> Device&;

            Device(Device&&) noexcept;
            auto operator=(Device&&) noexcept -> Device&;

            auto wait_idle() const noexcept -> Expected<void>;

            auto wait_for_fences(std::span<const view::Fence>     fences,
                                 bool                             wait_all = true,
                                 const std::chrono::milliseconds& timeout  = std::chrono::milliseconds::max()) const noexcept
              -> Expected<Result>;
            auto wait_for_fence(view::Fence                      fence,
                                const std::chrono::milliseconds& timeout = std::chrono::milliseconds::max()) const noexcept
              -> Expected<Result>;

            auto reset_fences(std::span<const view::Fence> fences) const noexcept -> Expected<void>;
            auto reset_fence(view::Fence fence) const noexcept -> Expected<void>;

            template<meta::IsOwnedOrView T>
            auto set_object_name(const T& object, std::string_view name) const -> Expected<void>;

            auto set_object_name(u64 object, DebugObjectType type, std::string_view name) const -> Expected<void>;

            [[nodiscard]]
            auto queue_entries() const noexcept -> const std::vector<gpu::Device::QueueEntry>&;

            [[nodiscard]]
            auto device_table() const noexcept -> const VolkDeviceTable&;

            [[nodiscard]]
            auto allocator() const noexcept -> vk::Observer<VmaAllocator>;

          private:
            std::vector<gpu::Device::QueueEntry> m_queue_entries;

            VolkDeviceTable            m_vk_device_table;
            vk::Observer<VmaAllocator> m_vma_allocator;
        };

        template<typename T>
        class DeviceObject: public PhysicalDeviceObject<T> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<T>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            DeviceObject(const T& child) noexcept;
            template<cmeta::IsContainerOrPointerOf<T> U>
            DeviceObject(const U& child) noexcept;
            ~DeviceObject() noexcept;

            DeviceObject(const DeviceObject&) noexcept;
            auto operator=(const DeviceObject&) noexcept -> DeviceObject&;

            DeviceObject(DeviceObject&&) noexcept;
            auto operator=(DeviceObject&&) noexcept -> DeviceObject&;

            [[nodiscard]]
            auto device() const noexcept -> const view::Device&;

          protected:
            Device m_device;
        };
    } // namespace view

    template<typename T>
    class OwnedByDevice: public OwnedByPhysicalDevice<T> {
      public:
        using ObjectInfo  = typename meta::ObjectInfo<T>;
        using ElementType = ObjectInfo::ElementType;
        using DeleterType = ObjectInfo::DeleterType;
        using ViewType    = ObjectInfo::ViewType;

        ~OwnedByDevice() noexcept;

        OwnedByDevice(const OwnedByDevice&)                    = delete;
        auto operator=(const OwnedByDevice&) -> OwnedByDevice& = delete;

        OwnedByDevice(OwnedByDevice&&) noexcept;
        auto operator=(OwnedByDevice&&) noexcept -> OwnedByDevice&;

        [[nodiscard]]
        auto device() const noexcept -> const view::Device&;

      protected:
        using Parent = OwnedByPhysicalDevice<T>;
        OwnedByDevice(view::Device&& device, DeleterType&& deleter_ptr) noexcept;

        view::Device m_device;
    };

    namespace vk {
        STORMKIT_GPU_API auto imgui_vk_loader(const char* func_name, void*) noexcept -> PFN_vkVoidFunction;
    }

    template<typename FormatContext>
    auto format_as(const Device::QueueEntry& physical_device, FormatContext& ctx) noexcept -> decltype(ctx.out());

    namespace monadic {
        template<QueueFlag flag, QueueFlag... no_flag>
        constexpr auto find_queue() noexcept -> decltype(auto);
    } // namespace monadic
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Device::Device(PrivateTag, view::PhysicalDevice physical_device) noexcept
        : OwnedByPhysicalDevice<Device> { std::move(physical_device), &VolkDeviceTable::vkDestroyDevice } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Device::~Device() noexcept {
        if (m_deleter_ptr != nullptr and m_vk_handle != VK_NULL_HANDLE)
            vk::call(m_vk_device_table.*Parent::m_deleter_ptr, m_vk_handle, nullptr);
        m_vk_handle = VK_NULL_HANDLE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Device::Device(Device&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Device::operator=(Device&&) noexcept -> Device& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    template<meta::IsOwnedOrView T>
    inline auto Device::set_object_name(const T& object, std::string_view name) const -> Expected<void> {
        if (not vkSetDebugUtilsObjectNameEXT) return {};

        auto&& vk_object = vk::to_vk(object);
        return set_object_name(std::bit_cast<u64>(static_cast<const void*>(vk_object)),
                               meta::ObjectInfo<T>::DEBUG_TYPE,
                               std::move(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Device::queue_entries() const noexcept -> const std::vector<QueueEntry>& {
        return m_queue_entries;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Device::device_table() const noexcept -> const VolkDeviceTable& {
        return m_vk_device_table;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Device::allocator() const noexcept -> vk::Observer<VmaAllocator> {
        return m_vma_allocator;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Device::Device(const gpu::Device& of) noexcept
            : PhysicalDeviceObject<gpu::Device> { of },
              m_queue_entries { of.queue_entries() },
              m_vk_device_table { of.device_table() },
              m_vma_allocator { of.allocator() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Device> T>
        STORMKIT_FORCE_INLINE
        inline Device::Device(const T& of) noexcept
            : PhysicalDeviceObject<gpu::Device> { of },
              m_queue_entries { of->queue_entries() },
              m_vk_device_table { of->device_table() },
              m_vma_allocator { of->allocator() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Device::~Device() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Device::Device(const Device&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::operator=(const Device&) noexcept -> Device& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Device::Device(Device&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::operator=(Device&&) noexcept -> Device& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::IsOwnedOrView T>
    STORMKIT_FORCE_INLINE
        inline auto Device::set_object_name(const T& object, std::string_view name) const -> Expected<void> {
            if (not vkSetDebugUtilsObjectNameEXT) return {};

            auto&& vk_object = vk::to_vk(object);
            return set_object_name(std::bit_cast<u64>(static_cast<const void*>(vk_object)), T::DEBUG_TYPE, std::move(name));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::queue_entries() const noexcept -> const std::vector<gpu::Device::QueueEntry>& {
            return m_queue_entries;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::device_table() const noexcept -> const VolkDeviceTable& {
            return m_vk_device_table;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Device::allocator() const noexcept -> vk::Observer<VmaAllocator> {
            return m_vma_allocator;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline DeviceObject<T>::DeviceObject(const T& child) noexcept
            : PhysicalDeviceObject<T> { child }, m_device { child.device() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<typename T>
        template<cmeta::IsContainerOrPointerOf<T> U>
        STORMKIT_FORCE_INLINE
        inline DeviceObject<T>::DeviceObject(const U& child) noexcept
            : PhysicalDeviceObject<T> { child }, m_device { child->device() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline DeviceObject<T>::~DeviceObject() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline DeviceObject<T>::DeviceObject(const DeviceObject&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto DeviceObject<T>::operator=(const DeviceObject&) noexcept -> DeviceObject& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline DeviceObject<T>::DeviceObject(DeviceObject&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto DeviceObject<T>::operator=(DeviceObject&&) noexcept -> DeviceObject& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto DeviceObject<T>::device() const noexcept -> const Device& {
            return m_device;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByDevice<T>::OwnedByDevice(view::Device&& device, DeleterType&& deleter_ptr) noexcept
        : Parent { clone(device.physical_device()), std::move(deleter_ptr) }, m_device { std::move(device) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByDevice<T>::~OwnedByDevice() noexcept {
        if constexpr (cmeta::SameAs<DeleterType, void (*)(VkDevice, ElementType, const VkAllocationCallbacks*)>) {
            const auto& device_table = device().device_table();
            auto&       vk_handle    = Parent::m_vk_handle;
            auto&       deleter_ptr  = Parent::m_deleter_ptr;
            if (deleter_ptr != nullptr and vk_handle != VK_NULL_HANDLE)
                vk::call(device_table.*Parent::deleter_ptr, vk_handle, nullptr);
            vk_handle = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByDevice<T>::OwnedByDevice(OwnedByDevice&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto OwnedByDevice<T>::operator=(OwnedByDevice&&) noexcept -> OwnedByDevice& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto OwnedByDevice<T>::device() const noexcept -> const view::Device& {
        return m_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename FormatContext>
    inline auto format_as(const Device::QueueEntry& queue, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[QueueEntry: .id = {}, .count = {}, .flags = {}]", queue.id, queue.count, queue.flags);
    }

    namespace monadic {
        /////////////////////////////////////
        /////////////////////////////////////
        template<QueueFlag flag, QueueFlag... no_flag>
        constexpr auto find_queue() noexcept -> decltype(auto) {
            return [](const auto& family) static noexcept {
                return core::check_flag_bit(family.flags, flag) and (not core::check_flag_bit(family.flags, no_flag) and ...);
            };
        }
    } // namespace monadic

} // namespace stormkit::gpu
