// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Strings.hpp>

#include <storm/window/Window.hpp>

#include <storm/render/core/DynamicLoader.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORM_PUBLIC Instance {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Instance;

        Instance(std::string app_name = "");
        ~Instance();

        Instance(Instance &&);
        Instance &operator=(Instance &&);

        Surface createSurface(const window::Window &window) const;
        SurfaceOwnedPtr createSurfacePtr(const window::Window &window) const;

        const render::PhysicalDevice &pickPhysicalDevice() const noexcept;
        const render::PhysicalDevice &pickPhysicalDevice(const Surface &surface) noexcept;

        inline core::span<const PhysicalDeviceOwnedPtr> physicalDevices();

        inline const DynamicLoader &loader() const noexcept;
        inline vk::Instance vkInstance() const noexcept;
        inline operator vk::Instance() const noexcept;
        inline vk::Instance vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

#if defined(STORM_OS_WINDOWS)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::Win32SurfaceCreateInfoKHR &create_info) const noexcept;
#elif defined(STORM_OS_MACOS)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::MacOSSurfaceCreateInfoMVK &create_info) const noexcept;
#elif defined(STORM_OS_LINUX)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::XcbSurfaceCreateInfoKHR &create_info) const noexcept;
#elif defined(STORM_OS_IOS)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::IOSSurfaceCreateInfoMVK &create_info) const noexcept;
#endif
      private:
        void createInstance() noexcept;
        void createDebugReportCallback();
        void retrievePhysicalDevices() noexcept;

        bool checkValidationLayerSupport(bool enable_validation) noexcept;
        bool checkExtensionSupport(const gsl::czstring<> extention) const noexcept;
        bool checkExtensionSupport(core::span<const gsl::czstring<>> extentions) const noexcept;

        std::vector<PhysicalDeviceOwnedPtr> m_physical_devices;

        DynamicLoader m_loader;
        vk::UniqueInstance m_vk_instance;
        vk::UniqueDebugUtilsMessengerEXT m_vk_messenger;

        std::string m_app_name;

        std::vector<std::string> m_extensions;
    };
} // namespace storm::render

#include "Instance.inl"
