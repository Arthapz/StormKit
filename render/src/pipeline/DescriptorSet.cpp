// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::DescriptorSet(const DescriptorPool &pool,
							 std::vector<DescriptorType> types,
							 VkDescriptorSet set)
	: m_pool{&pool}, m_types{std::move(types)}, m_vk_descriptor_set{set} {
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::~DescriptorSet() = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::DescriptorSet(DescriptorSet &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet &DescriptorSet::operator=(DescriptorSet &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSet::update(std::vector<render::Descriptor> descriptors) {
	STORM_EXPECTS(!std::empty(descriptors));

	const auto &device		 = m_pool->device();
	const auto &device_table = device.vkDeviceTable();

	auto vk_buffer_infos =
		std::vector<std::pair<std::uint32_t, VkDescriptorBufferInfo>>{};
	auto vk_image_infos =
		std::vector<std::pair<std::uint32_t, VkDescriptorImageInfo>>{};

	for (const auto &descriptor_proxy : descriptors) {
		if (std::holds_alternative<BufferDescriptor>(descriptor_proxy)) {
			const auto &descriptor =
				std::get<BufferDescriptor>(descriptor_proxy);
			const auto &buffer =
				static_cast<const HardwareBuffer &>(*descriptor.buffer);

			const auto buffer_info =
				VkDescriptorBufferInfo{.buffer = buffer,
									   .offset = descriptor.offset,
									   .range  = descriptor.range};

			vk_buffer_infos.emplace_back(descriptor.binding,
										 std::move(buffer_info));
		} else if (std::holds_alternative<ImageDescriptor>(descriptor_proxy)) {
			const auto &descriptor =
				std::get<ImageDescriptor>(descriptor_proxy);
			const auto &texture =
				static_cast<const Texture &>(*descriptor.texture);
			const auto &sampler =
				static_cast<const Sampler &>(*descriptor.sampler);

			const auto image_info =
				VkDescriptorImageInfo{.sampler	   = sampler,
									  .imageView   = texture.vkImageView(),
									  .imageLayout = toVK(descriptor.layout)};

			vk_image_infos.emplace_back(descriptor.binding,
										std::move(image_info));
		}
	}

		auto vk_write_infos = std::vector<VkWriteDescriptorSet>{};
	vk_write_infos.reserve(std::size(vk_buffer_infos) +
						   std::size(vk_image_infos));

		for (const auto &[binding, vk_buffer_info] : vk_buffer_infos) {
		const auto write_info = VkWriteDescriptorSet{
			.sType			 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			 = m_vk_descriptor_set,
			.dstBinding		 = binding,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType	 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo	 = &vk_buffer_info,
			};

		vk_write_infos.emplace_back(std::move(write_info));
	}

		for (const auto &[binding, vk_image_info] : vk_image_infos) {
		const auto write_info = VkWriteDescriptorSet{
			.sType			 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet			 = m_vk_descriptor_set,
			.dstBinding		 = binding,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType	 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo		 = &vk_image_info,
			};

		vk_write_infos.emplace_back(std::move(write_info));
	}

	device_table.vkUpdateDescriptorSets(
		device, gsl::narrow_cast<std::uint32_t>(std::size(vk_write_infos)),
		std::data(vk_write_infos), 0, nullptr);
}

