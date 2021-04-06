/////////// - StormKit::core - ///////////
#include <storm/core/State.hpp>
#include <storm/core/StateManager.hpp>

using namespace storm::core;

////////////////////////////////////////
////////////////////////////////////////
State::State(StateManager &owner) noexcept : m_owner { owner } {
}

////////////////////////////////////////
////////////////////////////////////////
State::~State() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
State::State(State &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
auto State::operator=(State &&) noexcept -> State & = default;
