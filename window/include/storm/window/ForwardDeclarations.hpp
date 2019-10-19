// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <memory>
#include <storm/core/Memory.hpp>

namespace storm::window {
	enum class EventType : std::uint8_t;
	enum class Key : std::int8_t;
	enum class MouseButton : std::uint8_t;
	enum class WindowStyle : std::uint8_t;

	struct Event;
	struct VideoSettings;

	class AbstractWindow;
	DECLARE_PTR_AND_REF(AbstractWindow)

	class Window;
	DECLARE_PTR_AND_REF(Window)

	using NativeHandle = void *;
} // namespace storm::window
