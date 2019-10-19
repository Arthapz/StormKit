// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    inline const PhysicalDeviceInfo &PhysicalDevice::info() const noexcept {
        return m_device_info;
    }
    inline const RenderCapabilities &PhysicalDevice::capabilities() const
        noexcept {
        return m_capabilities;
    }
    inline storm::core::span<const QueueFamily>
        PhysicalDevice::queueFamilies() const noexcept {
        return m_queue_families;
    }

    inline VkDevice PhysicalDevice::createVkDevice(
        const VkDeviceCreateInfo &create_info) const {
        auto vk_device = VkDevice{};

        const auto result = vkCreateDevice(m_vk_physical_device, &create_info,
                                           nullptr, &vk_device);
        STORM_ENSURES(result == VK_SUCCESS);

        return vk_device;
    }

    inline VkPhysicalDeviceMemoryProperties
        PhysicalDevice::vkMemoryProperties() const noexcept {
        return m_vk_memory_properties;
    }

    inline VkPhysicalDevice PhysicalDevice::vkPhysicalDevice() const noexcept {
        STORM_EXPECTS(m_vk_physical_device != VK_NULL_HANDLE);
        return m_vk_physical_device;
    }

    inline PhysicalDevice::operator VkPhysicalDevice() const noexcept {
        STORM_EXPECTS(m_vk_physical_device != VK_NULL_HANDLE);
        return m_vk_physical_device;
    }

} // namespace storm::render
