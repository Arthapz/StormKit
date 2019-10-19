// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline bool GraphicsPipeline::isBuilded() const noexcept { return m_is_builded; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &GraphicsPipeline::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void GraphicsPipeline::setRenderPass(const RenderPass &render_pass) noexcept {
        m_render_pass = core::makeConstObserver(render_pass);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const RenderPass &GraphicsPipeline::renderPass() const noexcept {
        return *m_render_pass;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void GraphicsPipeline::setState(GraphicsPipelineState state) noexcept {
        m_state = std::move(state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline GraphicsPipelineState &GraphicsPipeline::state() noexcept { return m_state; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const GraphicsPipelineState &GraphicsPipeline::state() const noexcept { return m_state; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Pipeline GraphicsPipeline::vkPipeline() const noexcept {
        STORM_EXPECTS(m_vk_pipeline);
        return *m_vk_pipeline;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::PipelineLayout GraphicsPipeline::vkPipelineLayout() const noexcept {
        STORM_EXPECTS(m_vk_pipeline_layout);
        return *m_vk_pipeline_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline GraphicsPipeline::operator vk::Pipeline() const noexcept { return vkPipeline(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Pipeline GraphicsPipeline::vkHandle() const noexcept { return vkPipeline(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 GraphicsPipeline::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPipeline_T *());
    }
} // namespace storm::render
