// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
RenderPass::RenderPass(const render::Device &device)
	: m_device{&device} {};

/////////////////////////////////////
/////////////////////////////////////
RenderPass::~RenderPass() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_render_pass != VK_NULL_HANDLE)
		device.destroyVkRenderPass(m_vk_render_pass);
}

/////////////////////////////////////
/////////////////////////////////////
RenderPass::RenderPass(RenderPass &&) = default;

/////////////////////////////////////
/////////////////////////////////////
RenderPass &RenderPass::operator=(RenderPass &&) = default;

/////////////////////////////////////
/////////////////////////////////////
std::uint32_t RenderPass::addAttachment(Attachment attachment) {
	m_attachments.emplace_back(std::move(attachment));

	return gsl::narrow_cast<std::uint32_t>(std::size(m_attachments) - 1u);
}

/////////////////////////////////////
/////////////////////////////////////
void RenderPass::build() {
	const auto &device = static_cast<const Device &>(*m_device);

	auto attachments = std::vector<VkAttachmentDescription>{};
	attachments.reserve(std::size(m_attachments));
	for (const auto &attachment : m_attachments) {
		const auto vk_attachment = VkAttachmentDescription{
			.format			= toVK(attachment.format),
			.samples		= toVK(attachment.samples),
			.loadOp			= toVK(attachment.load_op),
			.storeOp		= toVK(attachment.store_op),
			.stencilLoadOp  = toVK(attachment.stencil_load_op),
			.stencilStoreOp = toVK(attachment.stencil_store_op),
			.initialLayout  = toVK(attachment.initial_layout),
			.finalLayout	= toVK(attachment.final_layout)};

		attachments.emplace_back(std::move(vk_attachment));
	}

	auto color_attachment_refs =
		std::vector<std::vector<VkAttachmentReference>>{};
	auto depth_attachment_ref = std::optional<VkAttachmentReference>{};
	auto subpasses			  = std::vector<VkSubpassDescription>{};
	auto subpasses_deps		  = std::vector<VkSubpassDependency>{};

	for (const auto &subpass : m_subpasses) {
		auto &color_attachment_ref = color_attachment_refs.emplace_back();
		color_attachment_ref.reserve(std::size(subpass.attachment_refs));

		for (const auto &attachment_ref : subpass.attachment_refs) {
			if (isDepthFormat(
					m_attachments[attachment_ref.attachment_id].format)) {
				depth_attachment_ref = VkAttachmentReference{
					.attachment = attachment_ref.attachment_id,
					.layout		= toVK(attachment_ref.layout)};
				continue;
			}
			const auto vk_attachment_ref = VkAttachmentReference{
				.attachment = attachment_ref.attachment_id,
				.layout		= toVK(attachment_ref.layout)};

			color_attachment_ref.emplace_back(std::move(vk_attachment_ref));
		}

		const auto vk_subpass = VkSubpassDescription{
			.pipelineBindPoint	= toVK(subpass.bind_point),
			.colorAttachmentCount = gsl::narrow_cast<std::uint32_t>(
				std::size(color_attachment_ref)),
			.pColorAttachments		 = std::data(color_attachment_ref),
			.pDepthStencilAttachment = (depth_attachment_ref.has_value())
										   ? &depth_attachment_ref.value()
										   : nullptr};

		subpasses.emplace_back(std::move(vk_subpass));

		const auto vk_subpass_dependency = VkSubpassDependency{
			.srcSubpass	= VK_SUBPASS_EXTERNAL,
			.dstSubpass	= 0,
			.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
							 VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};

		subpasses_deps.emplace_back(std::move(vk_subpass_dependency));
	}

	const auto create_info = VkRenderPassCreateInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount =
			gsl::narrow_cast<std::uint32_t>(std::size(attachments)),
		.pAttachments = std::data(attachments),
		.subpassCount = gsl::narrow_cast<std::uint32_t>(std::size(subpasses)),
		.pSubpasses   = std::data(subpasses),
		.dependencyCount =
			gsl::narrow_cast<std::uint32_t>(std::size(subpasses_deps)),
		.pDependencies = std::data(subpasses_deps)};

	m_vk_render_pass = device.createVkRenderPass(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
render::FramebufferOwnedPtr RenderPass::createFramebuffer(
	core::Extent extent,
	std::vector<render::TextureConstObserverPtr> textures) const {
	return std::make_unique<Framebuffer>(*this, std::move(extent),
										 std::move(textures));
}

