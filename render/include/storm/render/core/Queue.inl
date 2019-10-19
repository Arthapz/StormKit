// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline QueueFlag Queue::type() const noexcept { return m_queue_flag; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &Queue::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Queue::familyIndex() const noexcept { return m_family_index; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Queue Queue::vkQueue() const noexcept {
        STORM_EXPECTS(m_vk_queue);
        return m_vk_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Queue::operator vk::Queue() const noexcept {
        STORM_EXPECTS(m_vk_queue);
        return m_vk_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Queue Queue::vkHandle() const noexcept { return vkQueue(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::CommandPool Queue::vkCommandPool() const noexcept {
        STORM_EXPECTS(m_vk_command_pool);
        return *m_vk_command_pool;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Queue::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkQueue_T *());
    }
} // namespace storm::render
