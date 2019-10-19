// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>

#undef None

namespace storm::window {
	enum class EventType : std::uint8_t {
		None,
		Closed,
		Resized,
		Minimized,
		Maximized,
		KeyPressed,
		KeyReleased,
		MouseButtonPushed,
		MouseButtonReleased,
		MouseEntered,
		MouseExited,
		MouseMoved,
		LostFocus,
		GainedFocus
	};
	struct Event {
		struct ResizedEvent {
			std::uint16_t width;
			std::uint16_t height;
		};
		struct KeyEvent {
			Key key;
		};
		struct MouseEvent {
			MouseButton button;
			std::int16_t x;
			std::int16_t y;
		};
		EventType type = EventType::None;
		union {
			ResizedEvent resizedEvent;
			KeyEvent keyEvent;
			MouseEvent mouseEvent;
			// MouseMoved       mouseMoved;
		};
	};
} // namespace storm::window
