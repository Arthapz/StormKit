// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:physical_device;

import std;

import stormkit.core;

import :base;
import :vulkan;
import :structs;
import :instance;

export namespace stormkit::gpu {
    namespace view {
        class PhysicalDevice;
    }

    class PhysicalDevice;
    class PhysicalDeviceObject;

    namespace meta {
        template<>
        struct ObjectInfo<PhysicalDevice> {
            using Of          = PhysicalDevice;
            using ValueType = VkPhysicalDevice;
            using DeleterType = decltype(monadic::noop());
            using ViewType    = view::PhysicalDevice;
            using OwnedBy     = Instance;

            static constexpr auto DEBUG_TYPE = DebugObjectType::PHYSICAL_DEVICE;
        };
    } // namespace meta

    class STORMKIT_GPU_API PhysicalDevice: public OwnedByInstance<PhysicalDevice> {
      public:
        ~PhysicalDevice();

        PhysicalDevice(const PhysicalDevice&)                    = delete;
        auto operator=(const PhysicalDevice&) -> PhysicalDevice& = delete;

        PhysicalDevice(PhysicalDevice&&) noexcept;
        auto operator=(PhysicalDevice&&) noexcept -> PhysicalDevice&;

        [[nodiscard]]
        auto check_extension_support(std::string_view extension) const noexcept -> bool;
        [[nodiscard]]
        auto check_extension_support(std::span<const std::string_view> extensions) const noexcept -> bool;
        [[nodiscard]]
        auto check_extension_support(std::span<const CZString> extensions) const noexcept -> bool;

        [[nodiscard]]
        auto info() const noexcept -> const PhysicalDeviceInfo&;
        [[nodiscard]]
        auto capabilities() const noexcept -> const RenderCapabilities&;
        [[nodiscard]]
        auto memory_types() const noexcept -> std::span<const MemoryPropertyFlag>;
        [[nodiscard]]
        auto queue_families() const noexcept -> std::span<const QueueFamily>;
        [[nodiscard]]
        auto extensions() const noexcept -> std::span<const std::string>;
        [[nodiscard]]
        auto formats_properties() const noexcept -> std::span<const std::pair<PixelFormat, FormatProperties>>;

        // clang-format off
   // private:
        // clang-format on
        PhysicalDevice(PrivateTag, view::Instance&&) noexcept;
        auto do_init(PrivateTag, VkPhysicalDevice&&) noexcept -> void;

      private:
        struct Data {
            PhysicalDeviceInfo device_info;
            RenderCapabilities capabilities;
        };

        Heap<Data>                                            m_data;
        std::vector<MemoryPropertyFlag>                       m_memory_types;
        std::vector<QueueFamily>                              m_queue_families;
        std::vector<std::string>                              m_extensions;
        std::vector<std::pair<PixelFormat, FormatProperties>> m_format_properties;

        friend class view::PhysicalDevice;
    };

    namespace view {
        class STORMKIT_GPU_API PhysicalDevice: public InstanceObject<gpu::PhysicalDevice> {
          public:
            PhysicalDevice(const gpu::PhysicalDevice& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::PhysicalDevice> T>
            PhysicalDevice(const T& of) noexcept;
            ~PhysicalDevice() noexcept;

            PhysicalDevice(const PhysicalDevice&) noexcept;
            auto operator=(const PhysicalDevice&) noexcept -> PhysicalDevice&;

            PhysicalDevice(PhysicalDevice&&) noexcept;
            auto operator=(PhysicalDevice&&) noexcept -> PhysicalDevice&;

            [[nodiscard]]
            auto check_extension_support(std::string_view extension) const noexcept -> bool;
            [[nodiscard]]
            auto check_extension_support(std::span<const std::string_view> extensions) const noexcept -> bool;
            [[nodiscard]]
            auto check_extension_support(std::span<const CZString> extensions) const noexcept -> bool;

            [[nodiscard]]
            auto info() const noexcept -> const PhysicalDeviceInfo&;
            [[nodiscard]]
            auto capabilities() const noexcept -> const RenderCapabilities&;
            [[nodiscard]]
            auto memory_types() const noexcept -> std::span<const MemoryPropertyFlag>;
            [[nodiscard]]
            auto queue_families() const noexcept -> std::span<const QueueFamily>;
            [[nodiscard]]
            auto extensions() const noexcept -> std::span<const std::string>;
            [[nodiscard]]
            auto formats_properties() const noexcept -> std::span<const std::pair<PixelFormat, FormatProperties>>;

          private:
            ref<const gpu::PhysicalDevice::Data> m_data;
            std::span<const MemoryPropertyFlag>  m_memory_types;

            std::span<const QueueFamily>                              m_queue_families;
            std::span<const std::string>                              m_extensions;
            std::span<const std::pair<PixelFormat, FormatProperties>> m_format_properties;
        };

        template<typename T>
        class PhysicalDeviceObject: public InstanceObject<T> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<T>;
            using ValueType = ObjectInfo::ValueType;
            using ViewType    = ObjectInfo::ViewType;

            PhysicalDeviceObject(const T& child) noexcept;
            template<cmeta::IsContainerOrPointerOf<T> U>
            PhysicalDeviceObject(const U& child) noexcept;
            ~PhysicalDeviceObject() noexcept;

            PhysicalDeviceObject(const PhysicalDeviceObject&) noexcept;
            auto operator=(const PhysicalDeviceObject&) noexcept -> PhysicalDeviceObject&;

            PhysicalDeviceObject(PhysicalDeviceObject&&) noexcept;
            auto operator=(PhysicalDeviceObject&&) noexcept -> PhysicalDeviceObject&;

            [[nodiscard]]
            auto physical_device() const noexcept -> const PhysicalDevice&;

          protected:
            PhysicalDevice m_physical_device;
        };
    } // namespace view

    template<typename T>
    class OwnedByPhysicalDevice: public OwnedByInstance<T> {
      public:
        using ObjectInfo  = typename meta::ObjectInfo<T>;
        using ValueType = ObjectInfo::ValueType;
        using DeleterType = ObjectInfo::DeleterType;
        using ViewType    = ObjectInfo::ViewType;

        ~OwnedByPhysicalDevice() noexcept;

        OwnedByPhysicalDevice(const OwnedByPhysicalDevice&)                    = delete;
        auto operator=(const OwnedByPhysicalDevice&) -> OwnedByPhysicalDevice& = delete;

        OwnedByPhysicalDevice(OwnedByPhysicalDevice&&) noexcept;
        auto operator=(OwnedByPhysicalDevice&&) noexcept -> OwnedByPhysicalDevice&;

        [[nodiscard]]
        auto physical_device() const noexcept -> const view::PhysicalDevice&;

      protected:
        using Parent = OwnedByInstance<T>;

        OwnedByPhysicalDevice(view::PhysicalDevice&&, DeleterType&&) noexcept;

        view::PhysicalDevice m_physical_device;
    };

    [[nodiscard]]
    STORMKIT_GPU_API auto score_physical_device(view::PhysicalDevice physical_device) noexcept -> u64;

    template<typename FormatContext>
    auto format_as(view::PhysicalDevice physical_device, FormatContext& ctx) noexcept -> decltype(ctx.out());
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PhysicalDevice::PhysicalDevice(PrivateTag, view::Instance&& instance) noexcept
        : OwnedByInstance<PhysicalDevice> { std::move(instance), monadic::noop() } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PhysicalDevice::~PhysicalDevice() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PhysicalDevice::PhysicalDevice(PhysicalDevice&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::operator=(PhysicalDevice&&) noexcept -> PhysicalDevice& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::info() const noexcept -> const PhysicalDeviceInfo& {
        return m_data->device_info;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::capabilities() const noexcept -> const RenderCapabilities& {
        return m_data->capabilities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::memory_types() const noexcept -> std::span<const MemoryPropertyFlag> {
        return m_memory_types;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::queue_families() const noexcept -> std::span<const QueueFamily> {
        return m_queue_families;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::extensions() const noexcept -> std::span<const std::string> {
        return m_extensions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDevice::formats_properties() const noexcept -> std::span<const std::pair<PixelFormat, FormatProperties>> {
        return m_format_properties;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDevice::PhysicalDevice(const gpu::PhysicalDevice& of) noexcept
            : InstanceObject<gpu::PhysicalDevice> { of },
              m_data { as_ref(of.m_data) },
              m_memory_types { of.m_memory_types },
              m_queue_families { of.m_queue_families },
              m_extensions { of.m_extensions },
              m_format_properties { of.m_format_properties } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::PhysicalDevice> T>
        STORMKIT_FORCE_INLINE
        inline PhysicalDevice::PhysicalDevice(const T& of) noexcept
            : InstanceObject<gpu::PhysicalDevice> { of },
              m_data { as_ref(of->m_data) },
              m_memory_types { of->m_memory_types },
              m_queue_families { of->m_queue_families },
              m_extensions { of->m_extensions },
              m_format_properties { of->m_format_properties } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDevice::~PhysicalDevice() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDevice::PhysicalDevice(const PhysicalDevice&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::operator=(const PhysicalDevice& other) noexcept -> PhysicalDevice& {
            if (&other == this) [[unlikely]]
                return *this;

            m_data              = as_ref(other.m_data);
            m_memory_types      = other.m_memory_types;
            m_queue_families    = other.m_queue_families;
            m_extensions        = other.m_extensions;
            m_format_properties = other.m_format_properties;

            InstanceObject<gpu::PhysicalDevice>::operator=(other);

            return *this;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDevice::PhysicalDevice(PhysicalDevice&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::operator=(PhysicalDevice&&) noexcept -> PhysicalDevice& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::info() const noexcept -> const PhysicalDeviceInfo& {
            return m_data->device_info;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::capabilities() const noexcept -> const RenderCapabilities& {
            return m_data->capabilities;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::memory_types() const noexcept -> std::span<const MemoryPropertyFlag> {
            return m_memory_types;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::queue_families() const noexcept -> std::span<const QueueFamily> {
            return m_queue_families;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::extensions() const noexcept -> std::span<const std::string> {
            return m_extensions;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDevice::formats_properties() const noexcept
          -> std::span<const std::pair<PixelFormat, FormatProperties>> {
            return m_format_properties;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceObject<T>::PhysicalDeviceObject(const T& child) noexcept
            : InstanceObject<T> { child }, m_physical_device { child.physical_device() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        template<cmeta::IsContainerOrPointerOf<T> U>
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceObject<T>::PhysicalDeviceObject(const U& child) noexcept
            : InstanceObject<T> { child }, m_physical_device { child->physical_device() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceObject<T>::~PhysicalDeviceObject() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceObject<T>::PhysicalDeviceObject(const PhysicalDeviceObject&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto PhysicalDeviceObject<T>::operator=(const PhysicalDeviceObject&) noexcept -> PhysicalDeviceObject& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceObject<T>::PhysicalDeviceObject(PhysicalDeviceObject&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto PhysicalDeviceObject<T>::operator=(PhysicalDeviceObject&&) noexcept -> PhysicalDeviceObject& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto PhysicalDeviceObject<T>::physical_device() const noexcept -> const PhysicalDevice& {
            return m_physical_device;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByPhysicalDevice<T>::OwnedByPhysicalDevice(view::PhysicalDevice&& physical_device,
                                                           DeleterType&&          deleter_ptr) noexcept
        : Parent { clone(physical_device.instance()), std::move(deleter_ptr) }, m_physical_device { std::move(physical_device) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByPhysicalDevice<T>::~OwnedByPhysicalDevice() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByPhysicalDevice<T>::OwnedByPhysicalDevice(OwnedByPhysicalDevice&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto OwnedByPhysicalDevice<T>::operator=(OwnedByPhysicalDevice&&) noexcept -> OwnedByPhysicalDevice& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto OwnedByPhysicalDevice<T>::physical_device() const noexcept -> const view::PhysicalDevice& {
        return m_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename FormatContext>
    inline auto format_as(view::PhysicalDevice physical_device, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        const auto& info = physical_device.info();
        return std::format_to(ctx.out(),
                              "PhysicalDevice[name: {}, vendor: {}, id: {}, vulkan: {}.{}.{}, driver version: "
                              "{}.{}.{}]",
                              info.device_name,
                              info.vendor_name,
                              info.device_id,
                              info.api_major_version,
                              info.api_minor_version,
                              info.api_patch_version,
                              info.driver_major_version,
                              info.driver_minor_version,
                              info.driver_patch_version);
    }
} // namespace stormkit::gpu
