// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <fstream>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::HardwareBuffer(
	const render::Device &device, HardwareBufferUsage usage,
	std::size_t size,
	MemoryProperty property)
	: m_device{&device}, m_usage{usage}, m_size{size} {
	const auto &device_		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device_.vkDeviceTable();

	const auto create_info =
		VkBufferCreateInfo{.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						   .size		= m_size,
						   .usage		= toVK(usage),
						   .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

	m_vk_buffer		 = device_.createVkBuffer(create_info);
	auto requirement = VkMemoryRequirements{};
	device_table.vkGetBufferMemoryRequirements(device_, m_vk_buffer,
											   &requirement);

	auto allocate_info =
		VmaAllocationCreateInfo{.requiredFlags = toVK(property)};

	m_vma_buffer_memory =
		device_.allocateVmaAllocation(allocate_info, requirement);

	device_.bindVmaBufferMemory(m_vma_buffer_memory, m_vk_buffer);
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::~HardwareBuffer() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vma_buffer_memory != VK_NULL_HANDLE)
		device.deallocateVmaAllocation(m_vma_buffer_memory);
	if (m_vk_buffer != VK_NULL_HANDLE)
		device.destroyVkBuffer(m_vk_buffer);
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::HardwareBuffer(HardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer &HardwareBuffer::operator=(HardwareBuffer &&) = default;


/////////////////////////////////////
/////////////////////////////////////
std::byte *HardwareBuffer::map(std::uint32_t offset) {
	STORM_EXPECTS(offset < m_size);

	const auto &device = static_cast<const Device &>(*m_device);

	return device.mapVmaMemory(m_vma_buffer_memory) + offset;
}

/////////////////////////////////////
/////////////////////////////////////
void HardwareBuffer::flush(std::ptrdiff_t offset, std::size_t size) {
	STORM_EXPECTS(offset < gsl::narrow_cast<std::ptrdiff_t>(m_size));
	STORM_EXPECTS(size <= m_size);

	const auto &device = static_cast<const Device &>(*m_device);

	vmaFlushAllocation(device.vmaAllocator(), m_vma_buffer_memory, offset,
					   size);
}

/////////////////////////////////////
/////////////////////////////////////
void HardwareBuffer::unmap() {
	const auto &device = static_cast<const Device &>(*m_device);

	device.unmapVmaMemory(m_vma_buffer_memory);
}

/////////////////////////////////////
/////////////////////////////////////
std::uint32_t HardwareBuffer::findMemoryType(
	std::uint32_t type_filter, VkMemoryPropertyFlags properties,
	const VkPhysicalDeviceMemoryProperties &mem_properties,
	[[maybe_unused]] const VkMemoryRequirements &mem_requirements) {
	for (auto i = 0u; i < mem_properties.memoryTypeCount; ++i) {
		if ((type_filter & (1 << i)) &&
			(mem_properties.memoryTypes[i].propertyFlags & properties) ==
				properties)
			return i;
	}

	return 0;
}
