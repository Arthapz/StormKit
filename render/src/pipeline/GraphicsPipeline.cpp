// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline::GraphicsPipeline(const render::Device &device)
	: m_device{&device} {};

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline::~GraphicsPipeline() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_is_builded && m_vk_pipeline != VK_NULL_HANDLE)
		device.destroyVkPipeline(m_vk_pipeline);
	if (m_is_builded && m_vk_pipeline_layout != VK_NULL_HANDLE)
		device.destroyVkPipelineLayout(m_vk_pipeline_layout);
	if (m_is_builded && m_vk_descriptor_set_layout != VK_NULL_HANDLE)
		device.destroyVkDescriptorSetLayout(m_vk_descriptor_set_layout);
}

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline &GraphicsPipeline::operator=(GraphicsPipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void GraphicsPipeline::build() {
	STORM_EXPECTS(m_is_builded == false);
	STORM_EXPECTS(m_render_pass != nullptr);

	const auto &device		= static_cast<const Device &>(*m_device);
	const auto &render_pass = static_cast<const RenderPass &>(*m_render_pass);

	auto binding_descriptions = std::vector<VkVertexInputBindingDescription>{};
	binding_descriptions.reserve(
		std::size(m_state.vertex_input_state.binding_descriptions));
	for (const auto &binding :
		 m_state.vertex_input_state.binding_descriptions) {
		const auto binding_desc = VkVertexInputBindingDescription{
			.binding   = binding.binding,
			.stride	= gsl::narrow_cast<std::uint32_t>(binding.stride),
			.inputRate = toVK(binding.input_rate)};
		binding_descriptions.emplace_back(std::move(binding_desc));
	}

	auto attribute_descriptions =
		std::vector<VkVertexInputAttributeDescription>{};
	attribute_descriptions.reserve(
		std::size(m_state.vertex_input_state.input_attribute_descriptions));
	for (const auto &attribute :
		 m_state.vertex_input_state.input_attribute_descriptions) {
		const auto attribute_desc = VkVertexInputAttributeDescription{
			.location = attribute.location,
			.binding  = attribute.binding,
			.format   = toVK(attribute.format),
			.offset   = gsl::narrow_cast<std::uint32_t>(attribute.offset)};

		attribute_descriptions.emplace_back(std::move(attribute_desc));
	}

	const auto vertex_input_info = VkPipelineVertexInputStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount =
			gsl::narrow_cast<std::uint32_t>(std::size(binding_descriptions)),
		.pVertexBindingDescriptions = std::data(binding_descriptions),
		.vertexAttributeDescriptionCount =
			gsl::narrow_cast<std::uint32_t>(std::size(attribute_descriptions)),
		.pVertexAttributeDescriptions = std::data(attribute_descriptions)};

	const auto input_assembly = VkPipelineInputAssemblyStateCreateInfo{
		.sType	= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = toVK(m_state.input_assembly_state.topology),
		.primitiveRestartEnable =
			m_state.input_assembly_state.primitive_restart_enable};

	auto viewports = std::vector<VkViewport>{};
	viewports.reserve(std::size(m_state.viewport_state.viewports));

	for (const auto &viewport : m_state.viewport_state.viewports) {
		const auto vk_viewport = VkViewport{.x		  = viewport.position.x,
											.y		  = viewport.position.y,
											.width	= viewport.extent.w,
											.height   = viewport.extent.h,
											.minDepth = viewport.depth.x,
											.maxDepth = viewport.depth.y};

		viewports.emplace_back(std::move(vk_viewport));
	}

	auto scissors = std::vector<VkRect2D>{};
	viewports.reserve(std::size(m_state.viewport_state.scissors));

	for (const auto &scissor : m_state.viewport_state.scissors) {
		const auto vk_scissor =
			VkRect2D{.offset = {scissor.offset.x, scissor.offset.y},
					 .extent = {scissor.extent.w, scissor.extent.h}};

		scissors.emplace_back(std::move(vk_scissor));
	}

	const auto viewport_state = VkPipelineViewportStateCreateInfo{
		.sType		   = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = gsl::narrow_cast<std::uint32_t>(std::size(viewports)),
		.pViewports	= std::data(viewports),
		.scissorCount  = gsl::narrow_cast<std::uint32_t>(std::size(scissors)),
		.pScissors	 = std::data(scissors)};

	const auto rasterizer = VkPipelineRasterizationStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = m_state.rasterization_state.depth_clamp_enable,
		.rasterizerDiscardEnable =
			m_state.rasterization_state.rasterizer_discard_enable,
		.polygonMode	 = toVK(m_state.rasterization_state.polygon_mode),
		.cullMode		 = toVK(m_state.rasterization_state.cull_mode),
		.frontFace		 = toVK(m_state.rasterization_state.front_face),
		.depthBiasEnable = VK_FALSE,
		.lineWidth		 = m_state.rasterization_state.line_width};

	const auto multisampling = VkPipelineMultisampleStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples =
			toVK(m_state.multisample_state.rasterization_samples),
		.sampleShadingEnable = m_state.multisample_state.sample_shading_enable};

	auto blend_attachments = std::vector<VkPipelineColorBlendAttachmentState>{};
	blend_attachments.reserve(std::size(m_state.color_blend_state.attachments));

	for (const auto &attachment : m_state.color_blend_state.attachments) {
		const auto vk_attachment = VkPipelineColorBlendAttachmentState{
			.blendEnable		 = attachment.blend_enable,
			.srcColorBlendFactor = toVK(attachment.src_color_blend_factor),
			.dstColorBlendFactor = toVK(attachment.dst_color_blend_factor),
			.colorBlendOp		 = toVK(attachment.color_blend_operation),
			.srcAlphaBlendFactor = toVK(attachment.src_alpha_blend_factor),
			.dstAlphaBlendFactor = toVK(attachment.dst_alpha_blend_factor),
			.alphaBlendOp		 = toVK(attachment.alpha_blend_operation),
			.colorWriteMask		 = toVK(attachment.color_write_mask)};

		blend_attachments.emplace_back(std::move(vk_attachment));
	}

	auto color_blending = VkPipelineColorBlendStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = m_state.color_blend_state.logic_operation_enable,
		.logicOp	   = toVK(m_state.color_blend_state.logic_operation),
		.attachmentCount =
			gsl::narrow_cast<std::uint32_t>(std::size(blend_attachments)),
		.pAttachments = std::data(blend_attachments)};

	std::copy(std::begin(m_state.color_blend_state.blend_constants),
			  std::end(m_state.color_blend_state.blend_constants),
			  color_blending.blendConstants);

	const auto dynamic_state = VkPipelineDynamicStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 0,
		.pDynamicStates	= nullptr};

	auto shaders = std::vector<VkPipelineShaderStageCreateInfo>{};
	shaders.reserve(std::size(m_state.shader_state.shaders));

	for (const auto &_shader : m_state.shader_state.shaders) {
		const auto &shader = static_cast<const Shader &>(_shader.get());

		const auto vk_shader = VkPipelineShaderStageCreateInfo{
			.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage  = static_cast<VkShaderStageFlagBits>(toVK(shader.type())),
			.module = shader,
			.pName  = "main"};

		shaders.emplace_back(std::move(vk_shader));
	}

		auto depth_stencil = VkPipelineDepthStencilStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable  = m_state.depth_stencil_state.depth_test_enable,
			.depthWriteEnable = m_state.depth_stencil_state.depth_write_enable,
			.depthCompareOp   = toVK(m_state.depth_stencil_state.depth_compare_op),
			.depthBoundsTestEnable =
				m_state.depth_stencil_state.depth_bounds_test_enable,
			.stencilTestEnable = VK_FALSE,
			.minDepthBounds	= m_state.depth_stencil_state.min_depth_bounds,
			.maxDepthBounds	= m_state.depth_stencil_state.max_depth_bounds,
			};

	auto set_layout = std::vector<VkDescriptorSetLayout>{};
	for (const auto &layout : m_state.layout.descriptor_set_layouts)
		set_layout.emplace_back(
			layout.get());

	const auto pipeline_layout = VkPipelineLayoutCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount =
			gsl::narrow_cast<std::uint32_t>(std::size(set_layout)),
		.pSetLayouts			= std::data(set_layout),
		.pushConstantRangeCount = 0,
		.pPushConstantRanges	= nullptr};

	m_vk_pipeline_layout = device.createVkPipelineLayout(pipeline_layout);

	const auto create_info = VkGraphicsPipelineCreateInfo{
		.sType		= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = gsl::narrow_cast<std::uint32_t>(std::size(shaders)),
		.pStages	= std::data(shaders),
		.pVertexInputState   = &vertex_input_info,
		.pInputAssemblyState = &input_assembly,
		.pViewportState		 = &viewport_state,
		.pRasterizationState = &rasterizer,
		.pMultisampleState   = &multisampling,
		.pDepthStencilState  = &depth_stencil,
		.pColorBlendState	= &color_blending,
		.pDynamicState		 = &dynamic_state,
		.layout				 = m_vk_pipeline_layout,
		.renderPass			 = render_pass,
		.subpass			 = 0,
		.basePipelineHandle  = VK_NULL_HANDLE,
		.basePipelineIndex   = -1};

	m_vk_pipeline = device.createVkGraphicsPipeline(create_info);

	m_is_builded = true;
}
