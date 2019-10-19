// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/PhysicalDeviceInfo.hpp>
#include <storm/render/core/QueueFamily.hpp>
#include <storm/render/core/RenderCapabilities.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
	class STORM_PUBLIC PhysicalDevice : public core::NonCopyable {
	  public:
		explicit PhysicalDevice(VkPhysicalDevice physical_device,
								const Instance &context);
        ~PhysicalDevice();

		PhysicalDevice(PhysicalDevice &&);
		PhysicalDevice &operator=(PhysicalDevice &&);

        bool checkExtensionSupport(gsl::czstring<> extension) const noexcept;
        bool checkExtensionSupport(
            storm::core::span<const gsl::czstring<>> extensions) const noexcept;
        DeviceOwnedPtr createLogicalDevice() const;
        void checkIfPresentSupportIsEnabled(const Surface &surface) noexcept;

        VkSurfaceCapabilitiesKHR
            queryVkSurfaceCapabilities(const Surface &surface) const noexcept;
        std::vector<VkSurfaceFormatKHR>
            queryVkSurfaceFormats(const Surface &surface) const noexcept;
        std::vector<VkPresentModeKHR>
            queryVkPresentModes(const Surface &surface) const noexcept;

        inline const PhysicalDeviceInfo &info() const noexcept;
        inline const RenderCapabilities &capabilities() const noexcept;
        inline storm::core::span<const QueueFamily> queueFamilies() const
            noexcept;

        inline VkDevice
            createVkDevice(const VkDeviceCreateInfo &create_info) const;

        inline VkPhysicalDeviceMemoryProperties vkMemoryProperties() const
            noexcept;

        inline VkPhysicalDevice vkPhysicalDevice() const noexcept;
        inline operator VkPhysicalDevice() const noexcept;

      private:
        InstanceConstObserverPtr m_instance;

        PhysicalDeviceInfo m_device_info;
        RenderCapabilities m_capabilities;

        std::vector<QueueFamily> m_queue_families;

        VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
        VkPhysicalDeviceMemoryProperties m_vk_memory_properties;

        std::vector<std::string> m_extensions;
    };
} // namespace storm::render

#include "PhysicalDevice.inl"
