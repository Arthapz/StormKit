// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UInt32 Profiler::currentEntryIndex() const noexcept {
        if (m_current_entry == 0) return ENTRY_COUNT - 1u;

        return m_current_entry - 1u;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::span<const Profiler::Entry> Profiler::entries() const noexcept {
        return m_entries;
    }
} // namespace storm::engine