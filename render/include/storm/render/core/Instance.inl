// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline core::span<const PhysicalDeviceOwnedPtr>
        Instance::physicalDevices() {
        return m_physical_devices;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline VkInstance Instance::vkInstance() const noexcept {
        STORM_EXPECTS(m_instance != VK_NULL_HANDLE);
        return m_instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Instance::operator VkInstance() const noexcept {
        STORM_EXPECTS(m_instance != VK_NULL_HANDLE);
        return m_instance;
    }
} // namespace storm::render
