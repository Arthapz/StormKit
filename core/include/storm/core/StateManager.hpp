// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORMKIT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

#include <functional>
#include <memory>
#include <queue>
#include <stack>
#include <storm/core/Fwd.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Numerics.hpp>

namespace storm::core {
    class STORMKIT_PUBLIC STORMKIT_EBCO StateManager final: public core::NonCopyable {
      public:
        explicit StateManager();

        StateManager(StateManager &&manager);

        StateManager &operator=(StateManager &&manager);

        ~StateManager();

        template<class T, typename... Args>
        void requestPush(Args &&...args) {
            static_assert(std::is_base_of<State, T>::value,
                          "T must inherit from storm::tools::State");

            requestPush(std::make_unique<T>(*this, std::forward<Args>(args)...));
        }

        void requestPush(StateOwnedPtr &&state);

        template<class T, typename... Args>
        void requestSet(Args &&...args) {
            static_assert(std::is_base_of<State, T>::value,
                          "T must inherit from storm::tools::State");

            requestSet(std::make_unique<T>(*this, std::forward<Args>(args)...));
        }

        void requestSet(StateOwnedPtr &&state);

        void requestPop();

        [[nodiscard]] bool hasPendingRequests() const noexcept {
            return !std::empty(m_action_queue);
        }

        void executeRequests();

        void update(storm::core::Secondf delta);

        void requestClear();

        State &top() noexcept;

        template<class T>
        T &top() {
            static_assert(std::is_base_of<State, T>::value,
                          "T must inherit from storm::tools::State");

            return *static_cast<T *>(m_stack.top().get());
        }

        inline core::ArraySize size() const noexcept { return m_stack.size(); }

      private:
        struct StateManagerAction {
            enum class Type { push, pop, set, clear } type;
            std::unique_ptr<State> state; // Null when pop
        };

        std::queue<StateManagerAction> m_action_queue;
        std::stack<StateOwnedPtr> m_stack;
    };
} // namespace storm::core

#ifdef STORMKIT_COMPILER_MSVC
    #pragma warning(pop)
#endif
