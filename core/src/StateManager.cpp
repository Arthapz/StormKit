#include <cassert>
#include <storm/core/State.hpp>
#include <storm/core/StateManager.hpp>
#include <storm/core/Strings.hpp>

using namespace storm::core;

////////////////////////////////////////
////////////////////////////////////////
StateManager::StateManager() = default;

////////////////////////////////////////
////////////////////////////////////////
StateManager::~StateManager() = default;

////////////////////////////////////////
////////////////////////////////////////
StateManager::StateManager(StateManager &&) = default;

////////////////////////////////////////
////////////////////////////////////////
StateManager &StateManager::operator=(StateManager &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void StateManager::requestPush(StateOwnedPtr &&state) {
	m_actionQueue.emplace(
		StateManagerAction{StateManagerAction::Type::push, std::move(state)});
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::requestSet(StateOwnedPtr &&state) {
	m_actionQueue.emplace(
		StateManagerAction{StateManagerAction::Type::set, std::move(state)});
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::requestPop() {
	if (!m_stack.empty())
		m_actionQueue.emplace(
			StateManagerAction{StateManagerAction::Type::pop, nullptr});
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::executeRequests() {
	if (m_actionQueue.empty())
		return;

	while (!m_actionQueue.empty()) {
		auto &caction = m_actionQueue.front();

		switch (caction.type) {
		case StateManagerAction::Type::push:
			if (!m_stack.empty())
				m_stack.top()->pause();
			m_stack.push(std::move(caction.state));
			break;
		case StateManagerAction::Type::pop:
			m_stack.pop();
			if (!m_stack.empty())
				m_stack.top()->resume();
			break;
		case StateManagerAction::Type::set:
			if (!m_stack.empty())
				m_stack.pop();
			m_stack.push(std::move(caction.state));
			break;
		case StateManagerAction::Type::clear:
			if (!m_stack.empty()) {
				std::stack<std::unique_ptr<State>> q;
				m_stack.swap(q);
			}
			break;
		}

		m_actionQueue.pop();
	}
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::update(std::uint64_t delta) {
	if (!m_stack.empty())
		m_stack.top()->update(delta);
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::render() {
	if (!m_stack.empty())
		m_stack.top()->render();
}

void StateManager::requestClear() {
	if (!m_stack.empty())
		m_actionQueue.emplace(
			StateManagerAction{StateManagerAction::Type::clear, nullptr});
}

////////////////////////////////////////
////////////////////////////////////////
State &StateManager::top() noexcept {
	assert(!m_stack.empty() && "can't return the top of an empty stack !");

	return *(m_stack.top());
}
