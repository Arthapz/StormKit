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
    m_action_queue.emplace(StateManagerAction { StateManagerAction::Type::push, std::move(state) });
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::requestSet(StateOwnedPtr &&state) {
    m_action_queue.emplace(StateManagerAction { StateManagerAction::Type::set, std::move(state) });
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::requestPop() {
    if (!m_stack.empty())
        m_action_queue.emplace(StateManagerAction { StateManagerAction::Type::pop, nullptr });
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::executeRequests() {
    if (m_action_queue.empty()) return;

    while (!m_action_queue.empty()) {
        auto &caction = m_action_queue.front();

        switch (caction.type) {
            case StateManagerAction::Type::push:
                if (!m_stack.empty()) m_stack.top()->pause();
                m_stack.push(std::move(caction.state));
                break;
            case StateManagerAction::Type::pop:
                m_stack.pop();
                if (!m_stack.empty()) m_stack.top()->resume();
                break;
            case StateManagerAction::Type::set:
                if (!m_stack.empty()) m_stack.pop();
                m_stack.push(std::move(caction.state));
                break;
            case StateManagerAction::Type::clear:
                if (!m_stack.empty()) {
                    std::stack<std::unique_ptr<State>> q;
                    m_stack.swap(q);
                }
                break;
        }

        m_action_queue.pop();
    }
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::update(Secondf delta) {
    if (!m_stack.empty()) m_stack.top()->update(delta);
}

////////////////////////////////////////
////////////////////////////////////////
void StateManager::requestClear() {
    if (!m_stack.empty())
        m_action_queue.emplace(StateManagerAction { StateManagerAction::Type::clear, nullptr });
}

////////////////////////////////////////
////////////////////////////////////////
State &StateManager::top() noexcept {
    STORMKIT_EXPECTS(!m_stack.empty());

    return *(m_stack.top());
}
