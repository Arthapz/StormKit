// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::core {
	class App;
	DECLARE_PTR_AND_REF(App)

	class NonCopyable;
	DECLARE_PTR_AND_REF(NonCopyable)

	class NonDefaultInstanciable;
	DECLARE_PTR_AND_REF(NonDefaultInstanciable)

	class NonMovable;
	DECLARE_PTR_AND_REF(NonMovable)

	class State;
	DECLARE_PTR_AND_REF(State)

	class StateManager;
	DECLARE_PTR_AND_REF(StateManager)
} // namespace storm::core
