// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:instance;

import std;

import stormkit.core;
import stormkit.wsi;

import :vulkan;
import :base;
import :structs;

namespace cmonadic = stormkit::core::monadic;

namespace stormkit::gpu {
    class physical_device;

    class instance_interface {
      public:
        [[nodiscard]]
        auto supported_extensions(this const auto& self) noexcept -> dynarray<string>;

        [[nodiscard]]
        auto supported_layers(this const auto& self) noexcept -> dynarray<string>;

        [[nodiscard]]
        auto loaded_extensions(this const auto& self) noexcept -> dynarray<string>;

        [[nodiscard]]
        auto loaded_layers(this const auto& self) noexcept -> dynarray<string>;

        [[nodiscard]]
        auto physical_devices(this const auto& self) noexcept -> dynarray<physical_device>;
    };

    struct physical_device_interface {
        [[nodiscard]]
        auto check_extension_support(string_view extension) const noexcept -> bool;

        [[nodiscard]]
        auto check_extension_support(array_view<const string_view> extensions) const noexcept
          -> std::optional<hash_set<string_view>>;

        [[nodiscard]]
        auto check_extension_support(array_view<const czstring> extensions) const noexcept
          -> std::optional<hash_set<string_view>>;

        [[nodiscard]]
        auto info(this const auto& self) noexcept -> physical_device_info;

        [[nodiscard]]
        auto capabilities(this const auto& self) noexcept -> capabilities;

        [[nodiscard]]
        auto memory_properties(this const auto& self) noexcept -> dynarray<memory_property_flag>;

        [[nodiscard]]
        auto queue_families(this const auto& self) noexcept -> dynarray<queue_family>;

        [[nodiscard]]
        auto extensions(this const auto& self) noexcept -> dynarray<string>;

        [[nodiscard]]
        auto format_properties(this const auto& self) noexcept -> dynarray<std::pair<pixel_format, format_properties>>;
    };

    using physical_device_trait = gpu_object_trait<VkPhysicalDevice, physical_device_interface>;
    using instance_trait        = gpu_object_trait<VkInstance, instance_interface>;

    export {
        class STORMKIT_GPU_API physical_device: public owned<physical_device_trait> {
          private:
            [[nodiscard]]
            static auto create(VkPhysicalDevice&&) noexcept -> expected<physical_device>;

            auto do_init(VkPhysicalDevice&&) noexcept -> expected<void>;
        };

        class STORMKIT_GPU_API instance: public owned<instance_trait> {
          public:
            struct create_info {
                string_view application_name         = "";
                u32         application_version      = vk::make_version(0, 0, 0);
                bool        enable_validation_layers = (config::build_type == "DEBUG");
            };

            ~instance() noexcept;

            [[nodiscard]]
            static auto create(const create_info& info = {}) noexcept -> expected<instance>;
            [[nodiscard]]
            static auto allocate(const create_info& info = {}) noexcept -> expected<heap_ptr<instance>>;

          private:
            auto do_init(const create_info&) noexcept -> expected<void>;

            auto do_load_instance() noexcept -> expected<void>;
            auto do_retrieve_physical_devices() noexcept -> expected<void>;
        };

        namespace view {
            class physical_device: public core::view<physical_device_trait> {
              public:
                physical_device(gpu::physical_device&&) = delete;
                physical_device(const gpu::physical_device& device STORMKIT_LIFETIMEBOUND) noexcept;

              private:
                ref_ptr<const physical_device_info>                          device_info_;
                ref_ptr<const render_capabilities>                           capabilities_;
                array_view<const MemoryPropertyFlag>                         memory_types_;
                array_view<const queue_family>                               queue_families_;
                array_view<const string>                                     extensions_;
                array_view<const std::pair<pixel_format, format_properties>> format_properties_;
            };

            class instance: public core::view<instance_trait> {
              public:
                instance(gpu::instance&&) = delete;
                instance(const gpu::instance& instance STORMKIT_LIFETIMEBOUND) noexcept;

              private:
                array_view<const string> supported_extensions_;
                array_view<const string> loaded_extensions_;
                array_view<const string> loaded_extensions_;

                array_view<const gpu::physical_device> physical_devices_;
            };
        } // namespace view

        [[nodiscard]]
        STORMKIT_GPU_API auto score_physical_device(view::physical_device physical_device) noexcept -> u64;

        template<typename FormatContext>
        auto format_as(view::physical_device physical_device, FormatContext& ctx) noexcept -> decltype(ctx.out());
    }
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Interface>
    STORMKIT_FORCE_INLINE
    inline auto InstanceObject<Interface>::instance() const noexcept -> view::Instance {
        return Interface::owner();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::check_extension_support(string_view extension) const noexcept -> bool {
        return stdr::any_of(extensions(), [extension](const auto& e) { return e == extension; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    inline auto PhysicalDeviceInterface<Base>::check_extension_support(array_view<const string_view> extensions) const noexcept
      -> std::optional<hash_set<string_view>> {
        auto required_extensions = hash_set<string_view> { stdr::begin(extensions), stdr::end(extensions) };

        for (const auto& extension : this->extensions()) required_extensions.erase(extension);

        if (not required_extensions.empty()) return required_extensions;

        return std::nullopt;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    inline auto PhysicalDeviceInterface<Base>::check_extension_support(array_view<const czstring> extensions) const noexcept
      -> std::optional<hash_set<string_view>> {
        const auto ext = transform(extensions, cmonadic::init<string_view>());
        return check_extension_support(ext);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::info() const noexcept -> const physical_device_info& {
        return Base::m_data->device_info;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::capabilities() const noexcept -> const render_capabilities& {
        return Base::m_data->capabilities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::memory_types() const noexcept -> array_view<const MemoryPropertyFlag> {
        return Base::m_memory_types;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::queue_families() const noexcept -> array_view<const queue_family> {
        return Base::m_queue_families;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::extensions() const noexcept -> array_view<const string> {
        return Base::m_extensions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceInterface<Base>::formats_properties() const noexcept
      -> array_view<const std::pair<pixel_format, format_properties>> {
        return Base::m_format_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto InstanceInterface<Base>::extensions() const noexcept -> array_view<const string> {
        return Base::m_extensions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto InstanceInterface<Base>::physical_devices() const noexcept -> array_view<const PhysicalDevice> {
        return Base::m_physical_devices;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline InstanceImplementation::InstanceImplementation(PrivateTag) noexcept
        : GpuObjectImplementation { auto(vkDestroyInstance) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline InstanceImplementation::~InstanceImplementation() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline InstanceImplementation::InstanceImplementation(InstanceImplementation&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto InstanceImplementation::operator=(InstanceImplementation&&) noexcept -> InstanceImplementation& = default;

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline InstanceImplementation::InstanceImplementation(const gpu::Instance& of) noexcept
            : GpuObjectViewImplementation { of }, m_extensions { of.extensions() }, m_physical_devices { of.physical_devices() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Instance> TContainerOrPointer>
        STORMKIT_FORCE_INLINE
        inline InstanceImplementation::InstanceImplementation(const TContainerOrPointer& of) noexcept
            : InstanceImplementation { *of } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline InstanceImplementation::InstanceImplementation(const gpu::InstanceImplementation& of) noexcept
            : GpuObjectViewImplementation { of }, m_extensions { of.m_extensions }, m_physical_devices { of.m_physical_devices } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline InstanceImplementation::~InstanceImplementation() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline InstanceImplementation::InstanceImplementation(const InstanceImplementation&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto InstanceImplementation::operator=(const InstanceImplementation&) noexcept
          -> InstanceImplementation& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline InstanceImplementation::InstanceImplementation(InstanceImplementation&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto InstanceImplementation::operator=(InstanceImplementation&&) noexcept -> InstanceImplementation& = default;
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PhysicalDeviceImplementation::PhysicalDeviceImplementation(PrivateTag, view::Instance&& instance) noexcept
        : GpuObjectImplementation { std::move(instance), monadic::noop() } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PhysicalDeviceImplementation::~PhysicalDeviceImplementation() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PhysicalDeviceImplementation::PhysicalDeviceImplementation(PhysicalDeviceImplementation&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceImplementation::operator=(PhysicalDeviceImplementation&&) noexcept
      -> PhysicalDeviceImplementation& = default;

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceImplementation::PhysicalDeviceImplementation(const gpu::PhysicalDevice& of) noexcept
            : GpuObjectViewImplementation { of },
              m_data { as_ref(of.m_data) },
              m_memory_types { of.m_memory_types },
              m_queue_families { of.m_queue_families },
              m_extensions { of.m_extensions },
              m_format_properties { of.m_format_properties } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::PhysicalDevice> TContainerOrPointer>
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceImplementation::PhysicalDeviceImplementation(const TContainerOrPointer& of) noexcept
            : PhysicalDeviceImplementation { *of } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceImplementation::~PhysicalDeviceImplementation() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceImplementation::PhysicalDeviceImplementation(const PhysicalDeviceImplementation&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDeviceImplementation::operator=(const PhysicalDeviceImplementation& other) noexcept
          -> PhysicalDeviceImplementation& {
            if (&other == this) [[unlikely]]
                return *this;

            GpuObjectViewImplementation::operator=(other);

            m_data              = as_ref(other.m_data);
            m_memory_types      = other.m_memory_types;
            m_queue_families    = other.m_queue_families;
            m_extensions        = other.m_extensions;
            m_format_properties = other.m_format_properties;

            return *this;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PhysicalDeviceImplementation::PhysicalDeviceImplementation(PhysicalDeviceImplementation&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PhysicalDeviceImplementation::operator=(PhysicalDeviceImplementation&&) noexcept
          -> PhysicalDeviceImplementation& = default;
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Interface>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceObject<Interface>::instance() const noexcept -> view::Instance {
        return Interface::owner().instance();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Interface>
    STORMKIT_FORCE_INLINE
    inline auto PhysicalDeviceObject<Interface>::physical_device() const noexcept -> view::PhysicalDevice {
        return Interface::owner();
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
