// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

/* @author Arthapz
 * @version 0.1
 **/

#include <memory>
#include <stack>
#include <storm/core/Fwd.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonDefaultInstanciable.hpp>
#include <storm/core/Platform.hpp>

namespace storm::core {
    /**
     * @class State
     * @ingroup tools
     * @brief The Abstract State class
     *
     * The class is the base of all State
     */
    class STORM_PUBLIC STORM_EBCO State: public core::NonCopyable {
      public:
        /**
         * @brief Constructor
         *
         * Constructor of State
         * @param parent : the state manager parent to this state
         * (StateManager)
         */

        explicit State(StateManager &owner);

        /**
         * @brief Move Constructor
         *
         * Move Constructor of State
         *
         * @param state : the state which be moved (State rv)
         */
        State(State &&state);

        /**
         * @brief Move assignement
         *
         * Move assignement of State
         *
         * @param state : the state which be moved (State rv)
         * @return a reference to current instance
         */
        State &operator=(State &&state);

        /**
         * @brief Destructor
         *
         * Destructor of State
         */
        virtual ~State();

        /**
         * @brief pause the state
         *
         * Method which pause the state
         */
        virtual void pause() = 0;

        /**
         * @brief resume the state
         *
         * Method which resume the state
         */
        virtual void resume() = 0;

        /**
         * @brief update the state
         *
         * Method which update the state data
         *
         * @param delta : the time since last update in microsecond(64 bit
         * unsigned integer)
         */
        virtual void update(core::UInt64 delta = 0U) = 0;

        /**
         * @brief render the state
         *
         * Method which render the state data
         *
         */
        virtual void render() = 0;

      protected:
        StateManagerRef m_owner;
    };
} // namespace storm::core

#ifdef STORM_COMPILER_MSVC
    #pragma warning(pop)
#endif
