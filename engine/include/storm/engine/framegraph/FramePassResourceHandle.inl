// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UInt32 FramePassResourceHandle::index() const noexcept { return m_index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassResourceHandle::isValid() const noexcept { return m_index != INVALID; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void FramePassResourceHandle::invalidate() noexcept { m_index = INVALID; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassResourceHandle::operator<(const FramePassResourceHandle &rhs) const
        noexcept {
        return rhs.index() < INVALID;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassResourceHandle::operator==(const FramePassResourceHandle &rhs) const
        noexcept {
        return rhs.index() == INVALID;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassResourceHandle::operator!=(const FramePassResourceHandle &rhs) const
        noexcept {
        return !operator==(rhs);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>::FramePassResourceID() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>::FramePassResourceID(FramePassResourceHandle handle) noexcept
        : FramePassResourceHandle { handle } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>::FramePassResourceID(const FramePassResourceID &) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource> &
        FramePassResourceID<Resource>::operator=(const FramePassResourceID &) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>::FramePassResourceID(FramePassResourceID &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource> &
        FramePassResourceID<Resource>::operator=(FramePassResourceID &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResourceID<Resource>::~FramePassResourceID() = default;
} // namespace storm::engine
