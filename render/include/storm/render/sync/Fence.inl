// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Fence Fence::vkFence() const noexcept {
        STORM_EXPECTS(m_vk_fence);
        return *m_vk_fence;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Fence::operator vk::Fence() const noexcept {
        STORM_EXPECTS(m_vk_fence);
        return *m_vk_fence;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Fence Fence::vkHandle() const noexcept { return vkFence(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Fence::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkFence_T *());
    }
} // namespace storm::render
