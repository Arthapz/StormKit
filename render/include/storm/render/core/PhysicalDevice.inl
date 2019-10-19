// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    vk::SurfaceCapabilitiesKHR
        PhysicalDevice::queryVkSurfaceCapabilities(const Surface &surface) const noexcept {
        CHECK_VK_ERROR_VALUE(m_vk_physical_device.getSurfaceCapabilitiesKHR(surface), capabilities);

        return capabilities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    std::vector<vk::SurfaceFormatKHR>
        PhysicalDevice::queryVkSurfaceFormats(const Surface &surface) const noexcept {
        CHECK_VK_ERROR_VALUE(m_vk_physical_device.getSurfaceFormatsKHR(surface), format);

        return format;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    std::vector<vk::PresentModeKHR>
        PhysicalDevice::queryVkPresentModes(const Surface &surface) const noexcept {
        CHECK_VK_ERROR_VALUE(m_vk_physical_device.getSurfacePresentModesKHR(surface),
                             present_modes);

        return present_modes;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const PhysicalDeviceInfo &PhysicalDevice::info() const noexcept { return m_device_info; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const RenderCapabilities &PhysicalDevice::capabilities() const noexcept {
        return m_capabilities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PhysicalDevice::MemoryPropertiesSpan PhysicalDevice::memoryProperties() const noexcept {
        return m_memory_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<const QueueFamily> PhysicalDevice::queueFamilies() const noexcept {
        return m_queue_families;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::UniqueDevice
        PhysicalDevice::createVkDevice(const vk::DeviceCreateInfo &create_info) const {
        CHECK_VK_ERROR_VALUE(m_vk_physical_device.createDeviceUnique(create_info), device);

        return device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const vk::PhysicalDeviceMemoryProperties &PhysicalDevice::vkMemoryProperties() const
        noexcept {
        return m_vk_memory_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::PhysicalDevice PhysicalDevice::vkPhysicalDevice() const noexcept {
        return m_vk_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PhysicalDevice::operator vk::PhysicalDevice() const noexcept {
        return m_vk_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::PhysicalDevice PhysicalDevice::vkHandle() const noexcept {
        return vkPhysicalDevice();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::FormatProperties PhysicalDevice::vkGetFormatProperties(vk::Format format) const
        noexcept {
        return m_vk_physical_device.getFormatProperties(format);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 PhysicalDevice::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPhysicalDevice());
    }
} // namespace storm::render
