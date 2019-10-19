/////////// - StormKit::engine - ///////////
#include <storm/engine/core/Profiler.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Assert.hpp>
#include <storm/core/Ranges.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
Profiler::Profiler(const Engine &engine) : m_engine { &engine } {
}

/////////////////////////////////////
/////////////////////////////////////
Profiler::~Profiler() = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler::Profiler(const Profiler &) = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler &Profiler::operator=(const Profiler &) = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler::Profiler(Profiler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Profiler &Profiler::operator=(Profiler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Profiler::newFrame() {
    if (GSL_LIKELY(!m_first_iteration)) {
        auto &previous = m_entries[m_current_entry];

        if (++m_current_entry >= ENTRY_COUNT) m_current_entry = 0;

        auto &current = m_entries[m_current_entry];
        current.scopes.clear();

        current.start = Clock::now();
        previous.end  = current.start;
    } else {
        auto &current = m_entries[m_current_entry];

        current.start = Clock::now();

        m_first_iteration = false;
    }
}

/////////////////////////////////////
/////////////////////////////////////
void Profiler::beginStage(std::string name, core::UInt32 level) {
    auto &entry = m_entries[m_current_entry];

    auto &[_, scope] = entry.scopes.emplace_back(std::move(name), Scope {});

    scope.level     = level;
    scope.start     = Clock::now();
    scope.finalized = false;
}

/////////////////////////////////////
/////////////////////////////////////
void Profiler::endStage(std::string_view name) {
    auto &entry = m_entries[m_current_entry];

    auto it = core::ranges::find_if(entry.scopes,
                                    [&name](const auto &pair) { return pair.first == name; });

    STORM_EXPECTS(it != core::ranges::end(entry.scopes));

    auto &scope = it->second;

    STORM_EXPECTS(scope.finalized == false);

    scope.end       = Clock::now();
    scope.finalized = true;
}
