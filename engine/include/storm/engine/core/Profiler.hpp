// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <chrono>
#include <unordered_map>

/////////// - StormKit::core - ///////////
#include <storm/core/Types.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC Profiler {
      public:
        static constexpr auto ENTRY_COUNT = 100u;

        using Clock = std::chrono::high_resolution_clock;
        struct Scope {
            core::UInt32 level;

            Clock::time_point start;
            Clock::time_point end;

            bool finalized = false;
        };

        struct Entry {
            Clock::time_point start;
            Clock::time_point end;

            std::vector<std::pair<std::string, Scope>> scopes;
        };

        explicit Profiler(const Engine &engine);
        ~Profiler();

        Profiler(const Profiler &);
        Profiler &operator=(const Profiler &);

        Profiler(Profiler &&);
        Profiler &operator=(Profiler &&);

        void newFrame();
        void beginStage(std::string name, core::UInt32 level = 1);
        void endStage(std::string_view name);

        inline core::UInt32 currentEntryIndex() const noexcept;
        inline core::span<const Entry> entries() const noexcept;

      private:
        EngineConstObserverPtr m_engine;

        std::array<Entry, ENTRY_COUNT> m_entries;

        core::UInt32 m_current_entry = 0u;
        bool m_first_iteration       = true;
    };
} // namespace storm::engine

#include "Profiler.inl"
