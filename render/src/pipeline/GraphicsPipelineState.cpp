#include <storm/render/pipeline/GraphicsPipelineState.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
	std::size_t hash<GraphicsPipelineVertexInputState>::
		operator()(const GraphicsPipelineVertexInputState &state) const
		noexcept {

		auto hash = std::size_t{0};
		core::hash_combine(hash, state.binding_descriptions);
		core::hash_combine(hash, state.input_attribute_descriptions);

		return hash;
	}

	std::size_t hash<GraphicsPipelineInputAssemblyState>::
		operator()(const GraphicsPipelineInputAssemblyState &state) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, state.topology);
		core::hash_combine(hash, state.primitive_restart_enable);

		return hash;
	}

	std::size_t hash<GraphicsPipelineViewportState>::
		operator()(const GraphicsPipelineViewportState &state) const noexcept {
		auto hash = std::size_t{0};
		for (const auto &viewport : state.viewports)
			core::hash_combine(hash, viewport);
		for (const auto &scissor : state.scissors)
			core::hash_combine(hash, scissor);

		return hash;
	}

	std::size_t hash<GraphicsPipelineRasterizationState>::
		operator()(const GraphicsPipelineRasterizationState &state) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, state.depth_clamp_enable);
		core::hash_combine(hash, state.rasterizer_discard_enable);
		core::hash_combine(hash, state.polygon_mode);
		core::hash_combine(hash, state.line_width);
		core::hash_combine(hash, state.cull_mode);
		core::hash_combine(hash, state.front_face);

		return hash;
	}

	std::size_t hash<GraphicsPipelineMultiSampleState>::
		operator()(const GraphicsPipelineMultiSampleState &state) const
		noexcept {

		auto hash = std::size_t{0};
		core::hash_combine(hash, state.sample_shading_enable);
		core::hash_combine(hash, state.rasterization_samples);

		return hash;
	}

	std::size_t hash<GraphicsPipelineColorBlendAttachmentState>::
		operator()(const GraphicsPipelineColorBlendAttachmentState &state) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, state.color_write_mask);
		core::hash_combine(hash, state.blend_enable);
		core::hash_combine(hash, state.src_color_blend_factor);
		core::hash_combine(hash, state.dst_color_blend_factor);
		core::hash_combine(hash, state.color_blend_operation);
		core::hash_combine(hash, state.src_alpha_blend_factor);
		core::hash_combine(hash, state.dst_alpha_blend_factor);
		core::hash_combine(hash, state.alpha_blend_operation);

		return hash;
	}

	std::size_t hash<GraphicsPipelineColorBlendState>::
		operator()(const GraphicsPipelineColorBlendState &state) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, state.logic_operation_enable);
		core::hash_combine(hash, state.logic_operation);

		for (const auto &attachment : state.attachments)
			core::hash_combine(hash, attachment);
		for (const auto blend_constant : state.blend_constants)
			core::hash_combine(hash, blend_constant);

		return hash;
	}

	std::size_t hash<GraphicsPipelineDynamicState>::operator()([
		[maybe_unused]] const GraphicsPipelineDynamicState &state) const
		noexcept {
		return 0;
	}

	std::size_t hash<GraphicsPipelineShaderState>::operator()([
		[maybe_unused]] const GraphicsPipelineShaderState &state) const
		noexcept {
		auto hash = std::size_t{0};

		for (const auto &shader : state.shaders)
			core::hash_combine(hash, &shader.get());

		return 0;
	}

	std::size_t hash<GraphicsPipelineDepthStencilState>::
		operator()(const GraphicsPipelineDepthStencilState &state) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, state.depth_test_enable);
		core::hash_combine(hash, state.depth_write_enable);
		core::hash_combine(hash, state.depth_compare_op);
		core::hash_combine(hash, state.depth_bounds_test_enable);
		core::hash_combine(hash, state.min_depth_bounds);
		core::hash_combine(hash, state.max_depth_bounds);

		return hash;
	}

	std::size_t hash<GraphicsPipelineLayout>::
		operator()(const GraphicsPipelineLayout &state) const noexcept {
		if (std::empty(state.descriptor_set_layouts))
			return 0;

		auto hash = std::size_t{0};
		for (const auto &layout : state.descriptor_set_layouts)
            core::hash_combine(hash, layout.get().hash());

		return hash;
	}

	std::size_t hash<GraphicsPipelineState>::
		operator()(const GraphicsPipelineState &state) const noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, state.input_assembly_state);
		core::hash_combine(hash, state.viewport_state);
		core::hash_combine(hash, state.rasterization_state);
		core::hash_combine(hash, state.multisample_state);
		core::hash_combine(hash, state.color_blend_state);
		core::hash_combine(hash, state.dynamic_state);
		core::hash_combine(hash, state.shader_state);
		core::hash_combine(hash, state.vertex_input_state);
		core::hash_combine(hash, state.depth_stencil_state);
		core::hash_combine(hash, state.layout);

		return hash;
	}
} // namespace std
