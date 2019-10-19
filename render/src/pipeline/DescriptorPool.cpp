// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool::DescriptorPool(const Device &device,
							   const DescriptorSetLayout &layout,
							   std::vector<Size> sizes, std::size_t max_sets)
	: m_device{&device}, m_layout{&layout} {
	auto vk_sizes = std::vector<VkDescriptorPoolSize>{};
	vk_sizes.reserve(std::size(sizes));

	for (const auto &size : sizes) {
		const auto pool_size = VkDescriptorPoolSize{
			.type = toVK(size.type),
			.descriptorCount =
				gsl::narrow_cast<std::uint32_t>(size.descriptor_count)};
		vk_sizes.emplace_back(std::move(pool_size));
	}

	const auto create_info = VkDescriptorPoolCreateInfo{
		.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets	   = gsl::narrow_cast<std::uint32_t>(max_sets),
		.poolSizeCount = gsl::narrow_cast<std::uint32_t>(std::size(sizes)),
		.pPoolSizes	= std::data(vk_sizes)};

	m_vk_descriptor_pool = m_device->createVkDescriptorPool(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool::~DescriptorPool() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_descriptor_pool != VK_NULL_HANDLE)
		device.destroyVkDescriptorPool(m_vk_descriptor_pool);
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool::DescriptorPool(DescriptorPool &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool &DescriptorPool::operator=(DescriptorPool &&) = default;

/////////////////////////////////////
/////////////////////////////////////
std::vector<render::DescriptorSetOwnedPtr>
	DescriptorPool::allocateDescriptorSets(std::size_t count) {
	const auto &device_table = m_device->vkDeviceTable();

	auto layouts =
		std::vector{count, static_cast<const DescriptorSetLayout &>(*m_layout)
							   .vkDescriptorSetLayout()};

	auto types = std::vector<DescriptorType>{};

	for (const auto &binding : m_layout->bindings()) {
		if (std::find(std::begin(types), std::end(types), binding.type) ==
			std::end(types))
			types.emplace_back(binding.type);
	}

	const auto allocate_info = VkDescriptorSetAllocateInfo{
		.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool		= m_vk_descriptor_pool,
		.descriptorSetCount = gsl::narrow_cast<std::uint32_t>(count),
		.pSetLayouts		= std::data(layouts)};

	auto vk_sets = std::vector<VkDescriptorSet>{};
	vk_sets.resize(count);

	device_table.vkAllocateDescriptorSets(*m_device, &allocate_info,
										  std::data(vk_sets));

	auto sets = std::vector<render::DescriptorSetOwnedPtr>{};
	sets.reserve(count);
	for (auto vk_set : vk_sets)
		sets.emplace_back(
			std::make_unique<DescriptorSet>(*this, types, vk_set));

	return sets;
}
