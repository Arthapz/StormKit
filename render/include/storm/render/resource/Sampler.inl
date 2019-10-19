// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Sampler Sampler::vkSampler() const noexcept {
        STORM_EXPECTS(m_vk_sampler);
        return *m_vk_sampler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Sampler::operator vk::Sampler() const noexcept {
        STORM_EXPECTS(m_vk_sampler);
        return *m_vk_sampler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Sampler Sampler::vkHandle() const noexcept { return vkSampler(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Sampler::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkSampler_T *());
    }
} // namespace storm::render
