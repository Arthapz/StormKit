// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
     const DynamicLoader &Instance::loader() const noexcept { return m_loader; }

    /////////////////////////////////////
    /////////////////////////////////////
     core::span<const PhysicalDeviceOwnedPtr> Instance::physicalDevices() {
        return m_physical_devices;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     vk::Instance Instance::vkInstance() const noexcept {
        STORM_EXPECTS(m_vk_instance);
        return *m_vk_instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     Instance::operator vk::Instance() const noexcept {
        STORM_EXPECTS(m_vk_instance);
        return *m_vk_instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     vk::Instance Instance::vkHandle() const noexcept { return vkInstance(); }

    /////////////////////////////////////
    /////////////////////////////////////
     core::UInt64 Instance::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkInstance_T *());
    }
} // namespace storm::render
