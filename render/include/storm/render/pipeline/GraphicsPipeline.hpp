// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#define STATE(TYPE, NAME, MEMBER)                                              \
	inline TYPE &NAME() noexcept {                                             \
		STORM_EXPECTS(m_is_builded == false);                                  \
		return m_state.MEMBER;                                                 \
	}                                                                          \
	inline const TYPE &NAME() const noexcept {                                 \
		return m_state.MEMBER;                                                 \
	}

namespace storm::render {
	class STORM_PUBLIC GraphicsPipeline {
	  public:
		explicit GraphicsPipeline(const Device &device);
		 ~GraphicsPipeline() ;

		GraphicsPipeline(GraphicsPipeline &&);
		GraphicsPipeline &operator=(GraphicsPipeline &&);

		STATE(GraphicsPipelineVertexInputState, vertexInputState,
			  vertex_input_state)
		STATE(GraphicsPipelineInputAssemblyState, inputAssemblyState,
			  input_assembly_state)
		STATE(GraphicsPipelineViewportState, viewportState, viewport_state)
		STATE(GraphicsPipelineRasterizationState, rasterizationState,
			  rasterization_state)
		STATE(GraphicsPipelineMultiSampleState, multisampleState,
			  multisample_state)
		STATE(GraphicsPipelineColorBlendState, colorBlendState,
			  color_blend_state)
		STATE(GraphicsPipelineDynamicState, dynamicState, dynamic_state)
		STATE(GraphicsPipelineShaderState, shaderState, shader_state)
		STATE(GraphicsPipelineDepthStencilState, depthStencilState,
			  depth_stencil_state)
		STATE(GraphicsPipelineLayout, layout, layout)

		void build();

		inline bool isBuilded() const noexcept {
			return m_is_builded;
		}

		inline const Device &device() const noexcept {
			return *m_device;
		}

		inline void setRenderPass(const RenderPass &render_pass) noexcept {
			m_render_pass = core::makeConstObserver(&render_pass);
		}
		inline const RenderPass &renderPass() const noexcept {
			return *m_render_pass;
		}

		inline void setState(GraphicsPipelineState state) noexcept {
			m_state = std::move(state);
		}
		inline const GraphicsPipelineState &state() const noexcept {
			return m_state;
		}

		inline VkPipeline vkPipeline() const noexcept {
			STORM_EXPECTS(m_vk_pipeline != VK_NULL_HANDLE);
			return m_vk_pipeline;
		}

		inline VkPipelineLayout vkPipelineLayout() const noexcept {
			STORM_EXPECTS(m_vk_pipeline_layout != VK_NULL_HANDLE);
			return m_vk_pipeline_layout;
		}

		inline operator VkPipeline() const noexcept {
			STORM_EXPECTS(m_vk_pipeline != VK_NULL_HANDLE);
			return m_vk_pipeline;
		}
	  private:
		DeviceConstObserverPtr m_device;

		GraphicsPipelineState m_state;

		RenderPassConstObserverPtr m_render_pass;

		bool m_is_builded = false;

		VkPipeline m_vk_pipeline						 = VK_NULL_HANDLE;
		VkPipelineLayout m_vk_pipeline_layout			 = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_vk_descriptor_set_layout = VK_NULL_HANDLE;
	};
} // namespace storm::render

#undef STATE
