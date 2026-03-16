// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
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

import :base;
import :vulkan;
import :structs;

export namespace stormkit::gpu {
    class PhysicalDevice;

    class Instance;
    class InstanceObject;

    namespace view {
        using Instance = View<Instance>;
    }

    namespace meta {
        template<>
        struct ObjectInfo<Instance> {
            using Of          = Instance;
            using ElementType = VkInstance;
            using DeleterType = PFN_vkDestroyInstance;
            using ViewType    = view::Instance;

            static constexpr auto DEBUG_TYPE = DebugObjectType::INSTANCE;
        };
    } // namespace meta

    class STORMKIT_GPU_API Instance: public Owned<Instance> {
      public:
        ~Instance();

        Instance(const Instance&)                    = delete;
        auto operator=(const Instance&) -> Instance& = delete;

        Instance(Instance&&) noexcept;
        auto operator=(Instance&&) noexcept -> Instance&;

        [[nodiscard]]
        auto physical_devices() const noexcept -> const std::vector<PhysicalDevice>&;

        // clang-format off
   // private:
        // clang-format on
        explicit Instance(PrivateTag) noexcept;
        auto do_init(PrivateTag, std::string = "", bool = (STORMKIT_BUILD_TYPE == "DEBUG")) noexcept -> Expected<void>;

      private:
        auto do_load_instance() noexcept -> Expected<void>;
        auto do_retrieve_physical_devices() noexcept -> Expected<void>;

        std::vector<std::string>    m_extensions;
        std::vector<PhysicalDevice> m_physical_devices;
    };

    namespace view {
        template<typename T>
        class InstanceObject: public View<T> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<T>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            InstanceObject(const T& child) noexcept;
            template<cmeta::IsContainerOrPointerOf<T> U>
            InstanceObject(const U& child) noexcept;
            ~InstanceObject() noexcept;

            InstanceObject(const InstanceObject&) noexcept;
            auto operator=(const InstanceObject&) noexcept -> InstanceObject&;

            InstanceObject(InstanceObject&&) noexcept;
            auto operator=(InstanceObject&&) noexcept -> InstanceObject&;

            [[nodiscard]]
            auto instance() const noexcept -> const Instance&;

          protected:
            Instance m_instance;
        };
    } // namespace view

    template<typename T>
    class OwnedByInstance: public Owned<T> {
      public:
        using ObjectInfo  = typename meta::ObjectInfo<T>;
        using ElementType = ObjectInfo::ElementType;
        using DeleterType = ObjectInfo::DeleterType;
        using ViewType    = ObjectInfo::ViewType;

        ~OwnedByInstance() noexcept;

        OwnedByInstance(const OwnedByInstance&)                    = delete;
        auto operator=(const OwnedByInstance&) -> OwnedByInstance& = delete;

        OwnedByInstance(OwnedByInstance&&) noexcept;
        auto operator=(OwnedByInstance&&) noexcept -> OwnedByInstance&;

        [[nodiscard]]
        auto instance() const noexcept -> const view::Instance&;

      protected:
        using Parent = Owned<T>;

        OwnedByInstance(view::Instance&&, DeleterType&&) noexcept;

        view::Instance m_instance;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Instance::physical_devices() const noexcept -> const std::vector<PhysicalDevice>& {
        return m_physical_devices;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline InstanceObject<T>::InstanceObject(const T& child) noexcept
            : View<T> { child }, m_instance { child.instance() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        template<cmeta::IsContainerOrPointerOf<T> U>
        STORMKIT_FORCE_INLINE
        inline InstanceObject<T>::InstanceObject(const U& child) noexcept
            : View<T> { child }, m_instance { child->instance() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline InstanceObject<T>::~InstanceObject() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline InstanceObject<T>::InstanceObject(const InstanceObject&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto InstanceObject<T>::operator=(const InstanceObject&) noexcept -> InstanceObject& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        STORMKIT_FORCE_INLINE
        inline InstanceObject<T>::InstanceObject(InstanceObject&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto InstanceObject<T>::operator=(InstanceObject&&) noexcept -> InstanceObject& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto InstanceObject<T>::instance() const noexcept -> const view::Instance& {
            return m_instance;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByInstance<T>::OwnedByInstance(view::Instance&& instance, DeleterType&& deleter_ptr) noexcept
        : Parent { std::move(deleter_ptr) }, m_instance { std::move(instance) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByInstance<T>::~OwnedByInstance() noexcept {
        if constexpr (cmeta::SameAs<DeleterType, void (*)(VkInstance, ElementType, const VkAllocationCallbacks*)>) {
            auto& vk_handle   = Parent::m_vk_handle;
            auto& deleter_ptr = Parent::m_deleter_ptr;
            if (deleter_ptr != nullptr and vk_handle != VK_NULL_HANDLE) vk::call(deleter_ptr, m_instance, vk_handle, nullptr);
            vk_handle = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline OwnedByInstance<T>::OwnedByInstance(OwnedByInstance&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto OwnedByInstance<T>::operator=(OwnedByInstance&&) noexcept -> OwnedByInstance& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto OwnedByInstance<T>::instance() const noexcept -> const view::Instance& {
        return m_instance;
    }
} // namespace stormkit::gpu
