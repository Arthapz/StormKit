// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout::DescriptorSetLayout(const Device &device)
	: m_device{&device} {
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout::~DescriptorSetLayout() {
	if (m_vk_descriptor_set_layout != VK_NULL_HANDLE)
		m_device->destroyVkDescriptorSetLayout(m_vk_descriptor_set_layout);
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout &DescriptorSetLayout::
	operator=(DescriptorSetLayout &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSetLayout::updateHash() noexcept {
	m_hash = std::hash<decltype(m_bindings)>{}(m_bindings);
}

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSetLayout::build() {
	const auto &device = static_cast<const Device &>(*m_device);

	auto descriptor_set_bindings = std::vector<VkDescriptorSetLayoutBinding>{};
	descriptor_set_bindings.reserve(std::size(m_bindings));

	for (const auto &descriptor_set_binding : m_bindings) {
		const auto vk_descriptor_set_binding = VkDescriptorSetLayoutBinding{
			.binding		 = descriptor_set_binding.binding,
			.descriptorType  = toVK(descriptor_set_binding.type),
			.descriptorCount = gsl::narrow_cast<std::uint32_t>(
				descriptor_set_binding.descriptor_count),
			.stageFlags = toVK(descriptor_set_binding.stages)};
		descriptor_set_bindings.emplace_back(
			std::move(vk_descriptor_set_binding));
	}

	const auto descriptor_layout = VkDescriptorSetLayoutCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount =
			gsl::narrow_cast<std::uint32_t>(std::size(descriptor_set_bindings)),
		.pBindings = std::data(descriptor_set_bindings)};

	m_vk_descriptor_set_layout =
		device.createVkDescriptorSetLayout(descriptor_layout);
}


namespace std {
	std::size_t hash<storm::render::DescriptorSetLayoutBinding>::operator()(
		const storm::render::DescriptorSetLayoutBinding &binding) const
		noexcept {
		auto hash = std::size_t{0};
		core::hash_combine(hash, binding.binding);
		core::hash_combine(hash, binding.type);
		core::hash_combine(hash, binding.stages);
		core::hash_combine(hash, binding.descriptor_count);

		return hash;
	}

	std::size_t hash<std::vector<storm::render::DescriptorSetLayoutBinding>>::
		operator()(const std::vector<storm::render::DescriptorSetLayoutBinding>
					   &bindings) const noexcept {
		auto hash = std::size_t{0};
		for (const auto &binding : bindings) core::hash_combine(hash, binding);

		return hash;
	}
} // namespace std
