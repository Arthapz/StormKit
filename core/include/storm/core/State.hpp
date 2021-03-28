// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORMKIT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

#include <memory>
#include <stack>
#include <storm/core/Fwd.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonDefaultInstanciable.hpp>
#include <storm/core/Numerics.hpp>
#include <storm/core/Platform.hpp>

namespace storm::core {
    class STORMKIT_PUBLIC STORMKIT_EBCO State: public core::NonCopyable {
      public:
        explicit State(StateManager &owner) noexcept;

        State(State &&state) noexcept;

        State &operator=(State &&state) noexcept;

        virtual ~State() noexcept;

        virtual void pause() = 0;

        virtual void resume() = 0;

        virtual void update(core::Secondf delta) = 0;

      protected:
        StateManagerRef m_owner;
    };
} // namespace storm::core

#ifdef STORMKIT_COMPILER_MSVC
    #pragma warning(pop)
#endif
