// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Strings.hpp>

#include <storm/window/Window.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
	class STORM_PUBLIC Instance {
	  public:
        Instance();
		~Instance();

		Instance(Instance &&);
		Instance &operator=(Instance &&);

        SurfaceOwnedPtr createSurface(const window::Window &window) const;

        const render::PhysicalDevice &pickPhysicalDevice() const noexcept;
		const render::PhysicalDevice &
			pickPhysicalDevice(const Surface &surface) noexcept;

        inline core::span<const PhysicalDeviceOwnedPtr> physicalDevices();

        inline VkInstance vkInstance() const noexcept;
        inline operator VkInstance() const noexcept;

#if defined(STORM_OS_WINDOWS)
		VkSurfaceKHR createVkSurface(
			const VkWin32SurfaceCreateInfoKHR &create_info) const noexcept;
#elif defined(STORM_OS_MACOS)
		VkSurfaceKHR createVkSurface(
			const VkMacOSSurfaceCreateInfoMVK &create_info) const noexcept;
#elif defined(STORM_OS_LINUX)
		VkSurfaceKHR
			createVkSurface(const VkXcbSurfaceCreateInfoKHR &create_info) const
			noexcept;
#endif
	  private:
		void createInstance() noexcept;
		void createDebugReportCallback();
		void retrievePhysicalDevices() noexcept;

		bool checkValidationLayerSupport(bool enable_validation) noexcept;

		std::vector<PhysicalDeviceOwnedPtr> m_physical_devices;

		VkInstance m_instance				 = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_messenger = VK_NULL_HANDLE;
	};
} // namespace storm::render

#include "Instance.inl"
