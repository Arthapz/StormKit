// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>

#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Framebuffer::Framebuffer(const RenderPass &render_pass, core::Extent extent,
                         std::vector<TextureConstObserverPtr> textures)
    : m_render_pass{&render_pass}, m_extent{std::move(extent)},
	  m_textures{std::move(textures)} {
	auto attachments = std::vector<VkImageView>{};

	for (const auto &texture : m_textures) {
		const auto &texture_ = static_cast<const Texture &>(*texture);
		attachments.emplace_back(texture_);
	}

	const auto create_info = VkFramebufferCreateInfo{
		.sType		= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = *m_render_pass,
		.attachmentCount =
			gsl::narrow_cast<std::uint32_t>(std::size(attachments)),
		.pAttachments = std::data(attachments),
		.width		  = m_extent.w,
		.height		  = m_extent.h,
		.layers		  = 1};

	const auto &device = m_render_pass->device();
	m_vk_framebuffer = device.createVkFramebuffer(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
Framebuffer::~Framebuffer() {
	const auto &device = m_render_pass->device();

	if (m_vk_framebuffer != VK_NULL_HANDLE)
		device.destroyVkFramebuffer(m_vk_framebuffer);
}

/////////////////////////////////////
/////////////////////////////////////
Framebuffer::Framebuffer(Framebuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Framebuffer &Framebuffer::operator=(Framebuffer &&) = default;
