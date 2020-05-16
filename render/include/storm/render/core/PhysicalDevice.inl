// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
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
    inline core::UInt64 PhysicalDevice::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPhysicalDevice());
    }
} // namespace storm::render
