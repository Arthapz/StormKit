// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

static const auto old_layout_access_map =
	std::unordered_map<VkImageLayout,
					   std::pair<VkAccessFlags, VkPipelineStageFlags>>{
		{VK_IMAGE_LAYOUT_UNDEFINED,
		 {VkAccessFlagBits{}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}},
		{VK_IMAGE_LAYOUT_PREINITIALIZED,
		 {VkAccessFlagBits{}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}},
		{VK_IMAGE_LAYOUT_GENERAL,
		 {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}},
		{VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		 {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}},
		{VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		 {VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT}},
		{VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
		 {VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT}},
		{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		 {VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT}},
		{VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		 {VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}},
		{VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		 {VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}},
		{VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		 {VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}}};

static const auto new_layout_access_map =
	std::unordered_map<VkImageLayout,
					   std::pair<VkAccessFlags, VkPipelineStageFlags>>{
		{VK_IMAGE_LAYOUT_UNDEFINED, {VkAccessFlagBits{}, {}}},
		{VK_IMAGE_LAYOUT_PREINITIALIZED, {VkAccessFlagBits{}, {}}},
		{VK_IMAGE_LAYOUT_GENERAL,
		 {VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
		  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT}},
		{VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		 {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}},
		{VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		 {VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT}},
		{VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
		 {VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT}},
		{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		 {VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT}},
		{VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		 {VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}},
		{VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		 {VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}},
		{VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		 {VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}}};

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer::CommandBuffer(const render::Queue &queue,
							 render::CommandBufferLevel level,
							 VkCommandBuffer command_buffer)
	: m_queue{&queue}, m_level{level}, m_vk_command_buffer{command_buffer} {};

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer::~CommandBuffer() {
	const auto &queue  = static_cast<const Queue &>(*m_queue);
	const auto &device = static_cast<const Device &>(queue.device());

	if (m_vk_command_buffer != VK_NULL_HANDLE)
		device.deallocateVkCommandBuffers(queue.vkCommandPool(),
										  std::array{m_vk_command_buffer});
}

void CommandBuffer::reset() noexcept {
	const auto &queue		 = static_cast<const Queue &>(*m_queue);
	const auto &device		 = static_cast<const Device &>(queue.device());
	const auto &device_table = device.vkDeviceTable();

	device_table.vkResetCommandBuffer(m_vk_command_buffer, 0);

	m_state = State::Initial;
}

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer::CommandBuffer(CommandBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer &CommandBuffer::operator=(CommandBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void CommandBuffer::build() noexcept {
	STORM_EXPECTS(m_vk_command_buffer != VK_NULL_HANDLE);
	STORM_EXPECTS(m_state == State::Recording);

	auto commands = std::exchange(m_commands, std::queue<render::Command>{});
	const auto &device		 = static_cast<const Device &>(m_queue->device());
	const auto &device_table = device.vkDeviceTable();

	while (!commands.empty()) {
		auto &command = commands.front();

		std::visit(
			core::overload{
				[this, &device_table](const BeginCommand &command) {
					auto begin_info = VkCommandBufferBeginInfo{};
					begin_info.sType =
						VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
					begin_info.flags =
						(command.one_time_submit)
							? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
							: VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
					begin_info.pInheritanceInfo = nullptr;

					auto result = device_table.vkBeginCommandBuffer(
						m_vk_command_buffer, &begin_info);
					STORM_ENSURES(result == VK_SUCCESS);
				},
				[this,
				 &device_table]([[maybe_unused]] const EndCommand &command) {
					device_table.vkEndCommandBuffer(m_vk_command_buffer);

					updateImageLayouts();
				},
				[this, &device_table](const BeginRenderPassCommand &command) {
					const auto &render_pass =
						static_cast<const RenderPass &>(command.render_pass);
					const auto &framebuffer =
						static_cast<const Framebuffer &>(command.framebuffer);

					auto clear_values = std::vector<VkClearValue>{};
					clear_values.reserve(std::size(command.clear_values));
					for (const auto &clear_value_variant :
						 command.clear_values) {
						auto clear_value = VkClearValue{};

						if (std::holds_alternative<ClearColor>(
								clear_value_variant)) {
							const auto &clear_color =
								std::get<ClearColor>(clear_value_variant);
							clear_value.color.float32[0] = clear_color.color.r;
							clear_value.color.float32[1] = clear_color.color.g;
							clear_value.color.float32[2] = clear_color.color.b;
							clear_value.color.float32[3] = clear_color.color.a;
						} else if (std::holds_alternative<ClearDepthStencil>(
									   clear_value_variant)) {
							const auto &clear_depth_stencil =
								std::get<ClearDepthStencil>(
									clear_value_variant);
							clear_value.depthStencil.depth =
								clear_depth_stencil.depth;
							clear_value.depthStencil.stencil =
								clear_depth_stencil.stencil;
						}

						clear_values.emplace_back(std::move(clear_value));
					}

					const auto render_pass_info = VkRenderPassBeginInfo{
						.sType		 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
						.renderPass  = render_pass,
						.framebuffer = framebuffer,
						.renderArea  = {.offset = {0, 0},
										.extent = {framebuffer.extent().width,
												   framebuffer.extent().height}},
						.clearValueCount = gsl::narrow_cast<std::uint32_t>(
							std::size(clear_values)),
						.pClearValues = std::data(clear_values)};

					device_table.vkCmdBeginRenderPass(
						m_vk_command_buffer, &render_pass_info,
						VK_SUBPASS_CONTENTS_INLINE);

					const auto attachments = command.render_pass.attachments();
					const auto textures	= command.framebuffer.textures();
					for (const auto &subpass :
						 command.render_pass.subpasses()) {
						for (const auto &refs : subpass.attachment_refs) {
							const auto &attachment =
								attachments[refs.attachment_id];
							const auto &texture =
									*textures[refs.attachment_id];

							m_to_update_texture_layout[&texture] =
								attachment.final_layout;
						}
					}
				},
				[this, &device_table](const EndRenderPassCommand &command) {
					device_table.vkCmdEndRenderPass(m_vk_command_buffer);
				},
				[this,
				 &device_table](const BindGraphicsPipelineCommand &command) {
					const auto &pipeline =
						static_cast<const GraphicsPipeline &>(command.pipeline);
					device_table.vkCmdBindPipeline(
						m_vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipeline);
				},
				[this, &device_table](const DrawCommand &command) {
					STORM_EXPECTS(command.vertex_count > 0u);

					device_table.vkCmdDraw(
						m_vk_command_buffer, command.vertex_count,
						command.instance_count, command.first_vertex,
						command.first_instance);
				},
				[this, &device_table](const DrawIndexedCommand &command) {
					STORM_EXPECTS(command.index_count > 0u);

					device_table.vkCmdDrawIndexed(
						m_vk_command_buffer, command.index_count,
						command.instance_count, command.first_index,
						command.vertex_offset, command.first_instance);
				},
				[this, &device_table](const BindVertexBuffersCommand &command) {
					STORM_EXPECTS(!std::empty(command.buffers));
					STORM_EXPECTS(!std::empty(command.offsets));
					STORM_EXPECTS(std::size(command.buffers) ==
								  std::size(command.offsets));

					auto buffers = std::vector<VkBuffer>{};
					auto offsets = std::vector<VkDeviceSize>{};

					for (const auto &buffer : command.buffers)
						buffers.emplace_back(
							static_cast<const HardwareBuffer &>(buffer.get()));

					for (const auto offset : command.offsets)
						offsets.emplace_back(offset);

					device_table.vkCmdBindVertexBuffers(
						m_vk_command_buffer, 0,
						gsl::narrow_cast<std::uint32_t>(std::size(buffers)),
						std::data(buffers), std::data(offsets));
				},
				[this, &device_table](const BindIndexBufferCommand &command) {
					const auto &buffer =
						static_cast<const HardwareBuffer &>(command.buffer);

					device_table.vkCmdBindIndexBuffer(
						m_vk_command_buffer, buffer, command.offset,
						(command.large_indices) ? VK_INDEX_TYPE_UINT32
												: VK_INDEX_TYPE_UINT16);
				},
				[this, &device_table](const CopyBufferCommand &command) {
					auto src_buffer =
						static_cast<const HardwareBuffer &>(command.source)
							.vkBuffer();
					auto dst_buffer =
						static_cast<const HardwareBuffer &>(command.destination)
							.vkBuffer();

					auto copy_buffer	  = VkBufferCopy{};
					copy_buffer.size	  = command.size;
					copy_buffer.srcOffset = command.src_offset;
					copy_buffer.dstOffset = command.dst_offset;

					device_table.vkCmdCopyBuffer(m_vk_command_buffer,
												 src_buffer, dst_buffer, 1,
												 &copy_buffer);
				},
				[this,
				 &device_table](const BindDescriptorSetsCommand &command) {
					STORM_EXPECTS(!std::empty(command.descriptor_sets));

					auto descriptor_sets = std::vector<VkDescriptorSet>{};
					for (const auto &descriptor_set : command.descriptor_sets)
						descriptor_sets.emplace_back(
							static_cast<const DescriptorSet &>(
								descriptor_set.get()));

					const auto pipeline_layout =
						static_cast<const GraphicsPipeline &>(*command.pipeline)
							.vkPipelineLayout();

					device_table.vkCmdBindDescriptorSets(
						m_vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipeline_layout, 0,
						gsl::narrow_cast<std::uint32_t>(
							std::size(descriptor_sets)),
						std::data(descriptor_sets), 0, nullptr);
				},
				[this, &device_table](const CopyBufferToImageCommand &command) {
					const auto &source =
						static_cast<const HardwareBuffer &>(command.source);
					const auto &destination =
						static_cast<const Texture &>(command.destination);
					const auto &subresource_range =
						destination.vkSubresourceRange();

					const auto copy_buffer = VkBufferImageCopy{
						.bufferOffset	  = 0,
						.bufferRowLength   = 0,
						.bufferImageHeight = 0,
						.imageSubresource =
							{.aspectMask	 = subresource_range.aspectMask,
							 .mipLevel		 = subresource_range.baseMipLevel,
							 .baseArrayLayer = subresource_range.baseArrayLayer,
							 .layerCount	 = subresource_range.layerCount},
						.imageOffset = {0, 0, 0},
						.imageExtent = {destination.extent().w,
										destination.extent().h, 1}};

					device_table.vkCmdCopyBufferToImage(
						m_vk_command_buffer, source, destination.vkImage(),
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_buffer);
				},
				[this,
				 &device_table](const TransitionImageLayoutCommand &command) {
					auto &dst_image   = static_cast<Texture &>(command.image);
					const auto &range = dst_image.vkSubresourceRange();

					const auto it = m_to_update_texture_layout.find(&dst_image);
					const auto old_layout =
						toVK(it != std::end(m_to_update_texture_layout)
								 ? it->second
								 : dst_image.layout());

					const auto &src_access =
						old_layout_access_map.find(old_layout);
					const auto &dst_access =
						new_layout_access_map.find(toVK(command.new_layout));

					const auto src_stage = src_access->second.second;
					const auto dst_stage = dst_access->second.second;

					const auto barrier = VkImageMemoryBarrier{
						.sType		   = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.srcAccessMask = src_access->second.first,
						.dstAccessMask = dst_access->second.first,
						.oldLayout	 = old_layout,
						.newLayout	 = toVK(command.new_layout),
						.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.image				 = dst_image.vkImage(),
						.subresourceRange	= range};

					device_table.vkCmdPipelineBarrier(
						m_vk_command_buffer, src_stage, dst_stage, 0, 0,
						nullptr, 0, nullptr, 1, &barrier);

					m_to_update_texture_layout[&dst_image] = command.new_layout;
				},
				[this, &device_table](
					const ExecuteSubCommandBuffersCommand &command) {
					auto vk_command_buffers = std::vector<VkCommandBuffer>{};
					vk_command_buffers.reserve(
						std::size(command.command_buffers));
					for (const auto &cmb : command.command_buffers) {
						STORM_EXPECTS(cmb.get().level() ==
									  render::CommandBufferLevel::Secondary);
						vk_command_buffers.emplace_back(
							static_cast<const CommandBuffer &>(cmb.get()));
					}

					device_table.vkCmdExecuteCommands(
						m_vk_command_buffer,
						gsl::narrow_cast<std::uint32_t>(
							std::size(command.command_buffers)),
						std::data(vk_command_buffers));
				},
				[this, &device_table](const CopyImageCommand &command) {
					auto extent = VkExtent3D{command.source.extent().w,
											 command.source.extent().h, 1};

					const auto regions = std::array{VkImageCopy{
						.srcSubresource =
							VkImageSubresourceLayers{
								.aspectMask = toVK(command.source_aspect_mask),
								.mipLevel   = 0,
								.baseArrayLayer = 0,
								.layerCount		= 1},
						.srcOffset = {0, 0, 0},
						.dstSubresource =
							VkImageSubresourceLayers{
								.aspectMask =
									toVK(command.destination_aspect_mask),
								.mipLevel		= 0,
								.baseArrayLayer = 0,
								.layerCount		= 1},
						.dstOffset = {0, 0, 0},
						.extent	= extent}};

					device_table.vkCmdCopyImage(
						m_vk_command_buffer,
						static_cast<const Texture &>(command.source).vkImage(),
						toVK(command.source_layout),
						static_cast<const Texture &>(command.destination)
							.vkImage(),
						toVK(command.destination_layout),
						gsl::narrow_cast<std::uint32_t>(std::size(regions)),
						std::data(regions));
				}},
			command);

		commands.pop();
	}

	m_state = State::Executable;
}

void CommandBuffer::updateImageLayouts() {
	for (auto &[texture, layout] : m_to_update_texture_layout) {
		texture->setLayout(layout);
	}
	m_to_update_texture_layout.clear();
}
