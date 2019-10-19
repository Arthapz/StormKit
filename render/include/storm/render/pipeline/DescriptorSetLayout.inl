// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void DescriptorSetLayout::addBinding(DescriptorSetLayoutBinding binding) {
        m_bindings.emplace_back(std::move(binding));

        updateHash();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::Hash64 DescriptorSetLayout::hash() const noexcept { return m_hash; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<const DescriptorSetLayoutBinding> DescriptorSetLayout::bindings() const
        noexcept {
        return m_bindings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::DescriptorSetLayout DescriptorSetLayout::vkDescriptorSetLayout() const noexcept {
        STORM_EXPECTS(m_vk_descriptor_set_layout);
        return *m_vk_descriptor_set_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorSetLayout::operator vk::DescriptorSetLayout() const noexcept {
        return vkDescriptorSetLayout();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::DescriptorSetLayout DescriptorSetLayout::vkHandle() const noexcept {
        return vkDescriptorSetLayout();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool DescriptorSetLayout::operator==(const DescriptorSetLayout &second) {
        return m_hash == second.hash();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 DescriptorSetLayout::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkDescriptorSetLayout_T *());
    }
} // namespace storm::render
