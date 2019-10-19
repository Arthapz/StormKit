// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Enums.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>

namespace storm::engine {
    class STORM_PUBLIC RingHardwareBuffer: public core::NonCopyable {
      public:
        RingHardwareBuffer(core::ArraySize count,
                           const render::Device &device,
                           render::HardwareBufferUsage usage,
                           core::ByteCount byte_count,
                           render::MemoryProperty property = render::MemoryProperty::Host_Visible |
                                                             render::MemoryProperty::Device_Local);
        ~RingHardwareBuffer();

        RingHardwareBuffer(RingHardwareBuffer &&);
        RingHardwareBuffer &operator=(RingHardwareBuffer &&);

        void next();
        core::Byte *map(core::UInt32 offset);
        void unmap();
        template<typename T>
        inline void upload(core::span<const T> data, core::Offset offset = 0);

        inline render::HardwareBuffer &buffer();
        inline const render::HardwareBuffer &buffer() const noexcept;

        inline core::ArraySize count() const noexcept;
        inline core::ArraySize elementSize() const noexcept;
        inline core::ByteCount byteCount() const noexcept;
        inline core::UOffset currentOffset() const noexcept;

      private:
        render::DeviceConstObserverPtr m_device;

        core::ArraySize m_count;
        core::ArraySize m_element_size;
        core::UOffset m_offset;

        render::HardwareBufferOwnedPtr m_buffer;
    };
} // namespace storm::engine

#include "RingHardwareBuffer.inl"