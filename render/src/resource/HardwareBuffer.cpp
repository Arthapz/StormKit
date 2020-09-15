// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <fstream>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>

using namespace storm;
using namespace storm::render;

#define DELETER [](auto handle, const auto &device) { device->deallocateVmaAllocation(handle); }

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::HardwareBuffer(const render::Device &device,
                               HardwareBufferUsage usage,
                               core::ArraySize size,
                               MemoryProperty property)
    : m_device { &device }, m_usage { usage }, m_byte_count { size },
      m_vma_buffer_memory { DELETER, *m_device } {
    const auto create_info = vk::BufferCreateInfo {}
                                 .setSize(m_byte_count)
                                 .setUsage(toVK(usage))
                                 .setSharingMode(vk::SharingMode::eExclusive);

    m_vk_buffer = m_device->createVkBuffer(create_info);

    const auto requirements = m_device->getVkBufferMemoryRequirements(*m_vk_buffer);

    auto allocate_info =
        VmaAllocationCreateInfo { .requiredFlags = toVK(property).operator unsigned int() };

    m_vma_buffer_memory.reset(m_device->allocateVmaAllocation(allocate_info, requirements));

    m_device->bindVmaBufferMemory(m_vma_buffer_memory, *m_vk_buffer);
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::~HardwareBuffer() = default;

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::HardwareBuffer(HardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer &HardwareBuffer::operator=(HardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
core::Byte *HardwareBuffer::map(core::UInt32 offset) {
    STORM_EXPECTS(offset < m_byte_count);

    const auto &device = static_cast<const Device &>(*m_device);

    return device.mapVmaMemory(m_vma_buffer_memory) + offset;
}

/////////////////////////////////////
/////////////////////////////////////
void HardwareBuffer::flush(core::Offset offset, core::ByteCount count) {
    STORM_EXPECTS(offset < gsl::narrow_cast<core::Offset>(m_byte_count));
    STORM_EXPECTS(count <= m_byte_count);

    vmaFlushAllocation(m_device->vmaAllocator(),
                       m_vma_buffer_memory,
                       gsl::narrow_cast<VkDeviceSize>(offset),
                       count);
}

/////////////////////////////////////
/////////////////////////////////////
void HardwareBuffer::unmap() {
    m_device->unmapVmaMemory(m_vma_buffer_memory);
}

/////////////////////////////////////
/////////////////////////////////////
core::UInt32
    HardwareBuffer::findMemoryType(core::UInt32 type_filter,
                                   VkMemoryPropertyFlags properties,
                                   const VkPhysicalDeviceMemoryProperties &mem_properties,
                                   [[maybe_unused]] const VkMemoryRequirements &mem_requirements) {
    for (auto i = 0u; i < mem_properties.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) &&
            (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    return 0;
}
