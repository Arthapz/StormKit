// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorSet
        DescriptorPool::allocateDescriptorSet(const DescriptorSetLayout &layout) const {
        auto descriptor = std::move(allocateDescriptorSets(1u, layout)[0]);

        return descriptor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorSetOwnedPtr
        DescriptorPool::allocateDescriptorSetPtr(const DescriptorSetLayout &layout) const {
        auto descriptor = std::move(allocateDescriptorSetsPtr(1u, layout)[0]);

        return descriptor;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &DescriptorPool::device() const noexcept { return *m_device; }
    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::DescriptorPool DescriptorPool::vkDescriptorPool() const noexcept {
        STORM_EXPECTS(m_vk_descriptor_pool);
        return *m_vk_descriptor_pool;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorPool::operator vk::DescriptorPool() const noexcept {
        return vkDescriptorPool();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::DescriptorPool DescriptorPool::vkHandle() const noexcept {
        return vkDescriptorPool();
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 DescriptorPool::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkDescriptorPool_T *());
    }
} // namespace storm::render