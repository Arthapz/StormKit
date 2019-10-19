// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::PipelineCache PipelineCache::vkPipelineCache() const noexcept {
        STORM_EXPECTS(m_vk_pipeline_cache);
        return *m_vk_pipeline_cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PipelineCache::operator vk::PipelineCache() const noexcept { return vkPipelineCache(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::PipelineCache PipelineCache::vkHandle() const noexcept { return vkPipelineCache(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 PipelineCache::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPipelineCache_T *());
    }
} // namespace storm::render
