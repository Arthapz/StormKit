// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <optional>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    class STORM_PUBLIC HardwareBuffer: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Buffer;

        explicit HardwareBuffer(const Device &device,
                                HardwareBufferUsage usage,
                                core::ByteCount byte_count,
                                MemoryProperty property = MemoryProperty::Host_Visible |
                                                          MemoryProperty::Host_Coherent);
        ~HardwareBuffer();

        HardwareBuffer(HardwareBuffer &&);
        HardwareBuffer &operator=(HardwareBuffer &&);

        inline const Device &device() const noexcept;
        inline HardwareBufferUsage usage() const noexcept;
        inline core::ByteCount byteCount() const noexcept;

        core::Byte *map(core::UInt32 offset);
        void flush(core::Offset offset, core::ByteCount byte_count);
        void unmap();

        template<typename T>
        inline void upload(core::span<const T> data, core::Offset offset = 0);

        inline vk::Buffer vkBuffer() const noexcept;
        inline operator vk::Buffer() const noexcept;
        inline vk::Buffer vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        static core::UInt32 findMemoryType(core::UInt32 type_filter,
                                           VkMemoryPropertyFlags properties,
                                           const VkPhysicalDeviceMemoryProperties &mem_properties,
                                           const VkMemoryRequirements &mem_requirements);

        DeviceConstObserverPtr m_device;
        HardwareBufferUsage m_usage;
        core::ByteCount m_byte_count;

        RAIIVkBuffer m_vk_buffer;
        RAIIVmaAllocation m_vma_buffer_memory;
    };
} // namespace storm::render

#include "HardwareBuffer.inl"
