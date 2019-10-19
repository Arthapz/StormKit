// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/core/Span.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(const render::Device &device) : m_device{&device} {};

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(const Device &device, core::Extent extent,
				 PixelFormat format, VkImage image)
	: m_device{&device}, m_vk_image{image}, m_is_owning_image{false} {
	m_extent = extent;
	m_format = format;

	m_subresource_range =
		VkImageSubresourceRange{.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
								.baseMipLevel	= 0,
								.levelCount		= 1,
								.baseArrayLayer = 0,
								.layerCount		= 1};

	const auto &device_ = static_cast<const Device &>(*m_device);
	const auto create_info =
		VkImageViewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
							  .image = m_vk_image,
							  .viewType			= VK_IMAGE_VIEW_TYPE_2D,
							  .format			= toVK(m_format),
							  .components		= {.r = VK_COMPONENT_SWIZZLE_R,
											   .g = VK_COMPONENT_SWIZZLE_G,
											   .b = VK_COMPONENT_SWIZZLE_B,
											   .a = VK_COMPONENT_SWIZZLE_A},
							  .subresourceRange = m_subresource_range};

	m_vk_image_view = device_.createVkImageView(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
Texture::~Texture() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_image_view != VK_NULL_HANDLE)
		device.destroyVkImageView(m_vk_image_view);
	if (m_is_owning_image && m_vk_image != VK_NULL_HANDLE)
		device.destroyVkImage(m_vk_image);
	if (m_is_owning_image && m_vma_image_memory != VK_NULL_HANDLE)
		device.deallocateVmaAllocation(m_vma_image_memory);
}

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(Texture &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Texture &Texture::operator=(Texture &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromImage(image::Image &image) {
	STORM_EXPECTS(m_vk_image == VK_NULL_HANDLE);
	STORM_EXPECTS(m_vk_image_view == VK_NULL_HANDLE);

	const auto format = [&image]() {
		switch (image.channels()) {
		case 1:
			return render::PixelFormat::R8_UNorm;
		case 2:
			return render::PixelFormat::RG8_UNorm;
		case 3:
			return render::PixelFormat::RGB8_UNorm;
		case 4:
			return render::PixelFormat::RGBA8_UNorm;
		}

		return render::PixelFormat::Undefined;
	}();

	loadFromMemory(image.data(), {image.extent().width, image.extent().height},
				   format);
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromMemory(storm::core::span<const std::byte> data,
							 core::Extent extent, render::PixelFormat format) {
	STORM_EXPECTS(m_vk_image == VK_NULL_HANDLE);
	STORM_EXPECTS(m_vk_image_view == VK_NULL_HANDLE);

	createTextureData(extent, format);

	auto staging_buffer = m_device->createStagingBuffer(std::size(data));
	staging_buffer->upload<const std::byte>(data);

	auto fence = m_device->createFence();

	auto command_buffer = m_device->graphicsQueue().createCommandBuffer();
	command_buffer->begin(true);
	command_buffer->transitionImageLayout(*this,
										  ImageLayout::Transfer_Dst_Optimal);
	command_buffer->copyBufferToImage(*staging_buffer, *this);
	command_buffer->transitionImageLayout(
		*this, ImageLayout::Shader_Read_Only_Optimal);
	command_buffer->end();
	command_buffer->build();

	m_device->graphicsQueue().submit(
		core::makeConstObserverArray(command_buffer), {}, {},
		core::makeObserver(fence));
	m_device->waitForFence(*fence);
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::createTextureData(core::Extent extent, render::PixelFormat format,
								render::SampleCountFlag samples,
								std::uint32_t mip_levels, std::uint32_t layers,
								render::ImageUsage usage) {
	STORM_EXPECTS(m_vk_image == VK_NULL_HANDLE);
	STORM_EXPECTS(m_vk_image_view == VK_NULL_HANDLE);
	STORM_EXPECTS(m_mip_levels > 0);
	STORM_EXPECTS(m_layers > 0);

	const auto &device		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device.vkDeviceTable();

	m_samples	 = samples;
	m_mip_levels = mip_levels;
	m_layers	 = layers;
	m_format	 = format;
	m_extent	 = extent;

	const auto create_info = VkImageCreateInfo{
		.sType		   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType	   = VK_IMAGE_TYPE_2D,
		.format		   = toVK(format),
		.extent		   = {.width = extent.w, .height = extent.h, .depth = 1},
		.mipLevels	   = m_mip_levels,
		.arrayLayers   = m_layers,
		.samples	   = toVK(m_samples),
		.tiling		   = VK_IMAGE_TILING_OPTIMAL,
		.usage		   = toVK(usage),
		.sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

	m_vk_image = device.createVkImage(create_info);

	auto requirement = VkMemoryRequirements{};
	device_table.vkGetImageMemoryRequirements(device, m_vk_image, &requirement);

	const auto alloc_info = VmaAllocationCreateInfo{
		.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};

	m_vma_image_memory = device.allocateVmaAllocation(alloc_info, requirement);

	vmaBindImageMemory(device.vmaAllocator(), m_vma_image_memory, m_vk_image);

	auto image_aspect = VkImageAspectFlags{VK_IMAGE_ASPECT_COLOR_BIT};
	if (isDepthFormat(format)) {
		image_aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (isDepthStencilFormat(format))
			image_aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	m_subresource_range = VkImageSubresourceRange{.aspectMask	= image_aspect,
												  .baseMipLevel = 0,
												  .levelCount	= 1,
												  .baseArrayLayer = 0,
												  .layerCount	  = 1};
	const auto create_view_info =
		VkImageViewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
							  .image = m_vk_image,
							  .viewType			= VK_IMAGE_VIEW_TYPE_2D,
							  .format			= toVK(m_format),
							  .components		= {.r = VK_COMPONENT_SWIZZLE_R,
											   .g = VK_COMPONENT_SWIZZLE_G,
											   .b = VK_COMPONENT_SWIZZLE_B,
											   .a = VK_COMPONENT_SWIZZLE_A},
							  .subresourceRange = m_subresource_range};

	m_vk_image_view = device.createVkImageView(create_view_info);

	m_extent = extent;
	m_format = format;
}
