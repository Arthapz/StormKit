// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

/**
 * @file StateManager.hpp
 * @brief the state manager
 * @author Arthapz
 * @version 0.1
 **/

#include <functional>
#include <memory>
#include <queue>
#include <stack>
#include <storm/core/ForwardDeclarations.hpp>
#include <storm/core/NonCopyable.hpp>

namespace storm::core {
	/**
	 * @class StateManager
	 * @ingroup tools
	 * @brief The State Manager
	 *
	 * The class handle states like a stack
	 */
	class STORM_PUBLIC STORM_EBCO StateManager final
		: public core::NonCopyable {
	  public:
		/**
		 * @brief Default Constructor
		 *
		 * Default Constructor of State
		 */
		explicit StateManager();

		/**
		 * @brief Move Constructor
		 *
		 * Move Constructor of StateManager
		 *
		 * @param manager : the state manager which be moved (StateManager rv)
		 */
		StateManager(StateManager &&manager);

		/**
		 * @brief Move assignement
		 *
		 * Move assignement of StateManager
		 *
		 * @param manager : the state manager which be moved (StateManager rv)
		 * @return a reference to current instance
		 **/
		StateManager &operator=(StateManager &&manager);

		/**
		 * @brief Destructor
		 *
		 * Destructor of StateManager
		 */
		~StateManager();

		/**
		 * @brief push the stack
		 *
		 * Method which create the state object and place it in Queue: the next
		 * executeRequests call will push it
		 *
		 * @param args : the state args (variadic)
		 */
		template <class T, typename... Args>
		inline void requestPush(Args &&... args) {
			static_assert(std::is_base_of<State, T>::value,
						  "T must inherit from storm::tools::State");

			requestPush(
				std::make_unique<T>(*this, std::forward<Args>(args)...));
		}

		/**
		 * @brief push the stack
		 *
		 * Method which make a state and place it in Queue: the next
		 * executeRequests call will push it
		 *
		 * @param state : the state
		 */
		void requestPush(StateOwnedPtr &&state);

		/**
		 * @brief set the stack
		 *
		 * Method wich create the state object it in Queue: the next
		 * executeRequests call will set it
		 *
		 * @param args : the state args (variadic)
		 */
		template <class T, typename... Args>
		inline void requestSet(Args &&... args) {
			static_assert(std::is_base_of<State, T>::value,
						  "T must inherit from storm::tools::State");

			requestSet(std::make_unique<T>(*this, std::forward<Args>(args)...));
		}

		/**
		 * @brief set the stack
		 *
		 * Method wich create the state and place it in Queue: the next
		 * executeRequests call will set it
		 *
		 * @param state : the state
		 */
		void requestSet(StateOwnedPtr &&state);

		/**
		 * @brief pop the stack
		 *
		 * Method which make a state and place it in queue: the next
		 * executeRequests call will pop it
		 */
		void requestPop();

		/**
		 * @brief execute requests
		 *
		 * Method which execute push, pop, set requested in the queue
		 */
		void executeRequests();

		/**
		 * @brief update the top state
		 *
		 * Method which update the top state data
		 *
		 * @param delta : the time since last update (float) default(0.f)
		 */
		void update(std::uint64_t delta = 0U);

		/**
		 * @brief render the top state
		 *
		 * Method which render the top state data
		 *
		 */
		void render();

		void requestClear();

		/**
		 * @brief get the top state
		 *
		 * Method which get a reference to the top state
		 *
		 * @return a reference to te top state
		 */
		State &top() noexcept;

		template <class T> T &top() {
			static_assert(std::is_base_of<State, T>::value,
						  "T must inherit from storm::tools::State");

			return *static_cast<T *>(m_stack.top().get());
		}

		inline std::size_t size() const noexcept {
			return m_stack.size();
		}

	  private:
		struct StateManagerAction {
			enum class Type { push, pop, set, clear } type;
			std::unique_ptr<State> state; // Null when pop
		};

		std::queue<StateManagerAction> m_actionQueue;
		std::stack<StateOwnedPtr> m_stack;
	};
} // namespace storm::core

#ifdef STORM_COMPILER_MSVC
#pragma warning(pop)
#endif
