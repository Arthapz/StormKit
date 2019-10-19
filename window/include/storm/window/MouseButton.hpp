// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Strings.hpp>

namespace storm::window {
	enum class MouseButton : std::uint8_t {
		UNKNOW,
		LEFT,
		RIGHT,
		MIDDLE,
		BUTTON1,
		BUTTON2
	};

	////////////////////////////////////////
	////////////////////////////////////////
	inline std::string mouseButtonToString(storm::window::MouseButton value) {
		using namespace storm::window;

		switch (value) {
		case MouseButton::LEFT:
			return "LEFT";
		case MouseButton::RIGHT:
			return "RIGHT";
		case MouseButton::MIDDLE:
			return "MIDDLE";
		case MouseButton::BUTTON1:
			return "BUTTON1";
		case MouseButton::BUTTON2:
			return "BUTTON2";
		case MouseButton::UNKNOW:
			return "UNKNOW";
		}

		return "UNKNOWN BUTTON";
	}
} // namespace storm::window

CUSTOM_FORMAT(storm::window::MouseButton, "{}",
			  storm::window::mouseButtonToString(data));
