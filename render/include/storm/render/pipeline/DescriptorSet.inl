// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
     core::span<const DescriptorType> DescriptorSet::types() const noexcept {
        return m_types;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     vk::DescriptorSet DescriptorSet::vkDescriptorSet() const noexcept {
        STORM_EXPECTS(m_vk_descriptor_set);
        return *m_vk_descriptor_set;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     DescriptorSet::operator vk::DescriptorSet() const noexcept { return vkDescriptorSet(); }

    /////////////////////////////////////
    /////////////////////////////////////
     vk::DescriptorSet DescriptorSet::vkHandle() const noexcept { return vkDescriptorSet(); }

    /////////////////////////////////////
    /////////////////////////////////////
     core::UInt64 DescriptorSet::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkDescriptorSet_T *());
    }
} // namespace storm::render
