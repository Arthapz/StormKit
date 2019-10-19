// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#define STATE(TYPE, NAME, MEMBER)             \
    inline TYPE &NAME() noexcept {            \
        STORM_EXPECTS(m_is_builded == false); \
        return m_state.MEMBER;                \
    }                                         \
    inline const TYPE &NAME() const noexcept { return m_state.MEMBER; }

namespace storm::render {
    class STORM_PUBLIC GraphicsPipeline {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline;

        explicit GraphicsPipeline(const Device &device,
                                  PipelineCacheConstObserverPtr cache = nullptr);
        ~GraphicsPipeline();

        GraphicsPipeline(GraphicsPipeline &&);
        GraphicsPipeline &operator=(GraphicsPipeline &&);

        STATE(GraphicsPipelineVertexInputState, vertexInputState, vertex_input_state)
        STATE(GraphicsPipelineInputAssemblyState, inputAssemblyState, input_assembly_state)
        STATE(GraphicsPipelineViewportState, viewportState, viewport_state)
        STATE(GraphicsPipelineRasterizationState, rasterizationState, rasterization_state)
        STATE(GraphicsPipelineMultiSampleState, multisampleState, multisample_state)
        STATE(GraphicsPipelineColorBlendState, colorBlendState, color_blend_state)
        STATE(GraphicsPipelineDynamicState, dynamicState, dynamic_state)
        STATE(GraphicsPipelineShaderState, shaderState, shader_state)
        STATE(GraphicsPipelineDepthStencilState, depthStencilState, depth_stencil_state)
        STATE(GraphicsPipelineLayout, layout, layout)

        void build();

        inline bool isBuilded() const noexcept;

        inline const Device &device() const noexcept;

        inline void setRenderPass(const RenderPass &render_pass) noexcept;
        inline const RenderPass &renderPass() const noexcept;

        inline void setState(GraphicsPipelineState state) noexcept;
        inline const GraphicsPipelineState &state() const noexcept;
        inline GraphicsPipelineState &state() noexcept;

        inline vk::Pipeline vkPipeline() const noexcept;
        inline vk::PipelineLayout vkPipelineLayout() const noexcept;
        inline operator vk::Pipeline() const noexcept;
        inline vk::Pipeline vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstObserverPtr m_device;
        PipelineCacheConstObserverPtr m_pipeline_cache;

        GraphicsPipelineState m_state;

        RenderPassConstObserverPtr m_render_pass;

        bool m_is_builded = false;

        RAIIVkPipeline m_vk_pipeline;
        RAIIVkPipelineLayout m_vk_pipeline_layout;
    };
} // namespace storm::render

#undef STATE

#include "GraphicsPipeline.inl"
