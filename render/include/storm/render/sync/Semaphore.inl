// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Semaphore Semaphore::vkSemaphore() const noexcept {
        STORM_EXPECTS(m_vk_semaphore);
        return *m_vk_semaphore;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Semaphore::operator vk::Semaphore() const noexcept {
        STORM_EXPECTS(m_vk_semaphore);
        return *m_vk_semaphore;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Semaphore Semaphore::vkHandle() const noexcept { return vkSemaphore(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Semaphore::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkSemaphore_T *());
    }
} // namespace storm::render
