// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <storm/core/Flags.hpp>

namespace storm::window {
	enum class WindowStyle : std::uint8_t {
		TitleBar	= 0b1,
		Close		= TitleBar | 0b10,
		Minimizable = TitleBar | 0b100,
		Resizable   = TitleBar | 0b1000,
		All			= Resizable | Minimizable | Close,
		Fullscreen  = 0b10000
	};
}

FLAG_ENUM(storm::window::WindowStyle)
