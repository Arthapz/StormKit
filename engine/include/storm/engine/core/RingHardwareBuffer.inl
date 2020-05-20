// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
     void RingHardwareBuffer::upload(core::span<const T> data, core::Offset offset) {
        m_buffer->upload(data, m_offset + offset);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     render::HardwareBuffer &RingHardwareBuffer::buffer() { return *m_buffer; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     const render::HardwareBuffer &RingHardwareBuffer::buffer() const noexcept {
        return *m_buffer;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     core::ArraySize RingHardwareBuffer::count() const noexcept { return m_count; }

    ////////////////////////////////////////
    ////////////////////////////////////////
     core::ArraySize RingHardwareBuffer::elementSize() const noexcept {
        return m_element_size;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     core::ByteCount RingHardwareBuffer::byteCount() const noexcept {
        return m_buffer->byteCount();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     core::UOffset RingHardwareBuffer::currentOffset() const noexcept { return m_offset; }
} // namespace storm::engine