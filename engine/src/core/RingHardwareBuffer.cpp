#include <storm/engine/core/RingHardwareBuffer.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer::RingHardwareBuffer(core::ArraySize count,
                                       const render::Device &device,
                                       render::HardwareBufferUsage usage,
                                       core::ByteCount byte_count,
                                       render::MemoryProperty property)
    : m_device { &device }, m_count { count } {
    const auto &physical_device  = m_device->physicalDevice();
    const auto &capabilities     = physical_device.capabilities();
    const auto memory_properties = physical_device.memoryProperties();

    const auto it =
        std::find(std::cbegin(memory_properties), std::cend(memory_properties), property);

    if (property == (render::MemoryProperty::Host_Visible | render::MemoryProperty::Device_Local)) {
        if (it == std::cend(memory_properties))
            property = render::MemoryProperty::Host_Visible | render::MemoryProperty::Host_Coherent;
    } else
        STORM_ENSURES(it != std::cend(memory_properties));

    m_element_size = computeUniformBufferOffsetAlignement(byte_count, capabilities);
    m_offset       = 0u;

    m_buffer = m_device->createHardwareBufferPtr(usage, m_count * m_element_size, property);
}

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer::~RingHardwareBuffer() = default;

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer::RingHardwareBuffer(RingHardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer &RingHardwareBuffer::operator=(RingHardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void RingHardwareBuffer::next() {
    m_offset += m_element_size;
    m_offset %= (m_element_size * m_count);
}

/////////////////////////////////////
/////////////////////////////////////
core::Byte *RingHardwareBuffer::map(core::UInt32 offset) {
    return m_buffer->map(m_offset + offset);
}

/////////////////////////////////////
/////////////////////////////////////
void RingHardwareBuffer::unmap() {
    m_buffer->unmap();
}