// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>

/////////// - XCB - ///////////
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

namespace storm::window::details {
    STORMKIT_PRIVATE xkb_keysym_t stormkitKeyToXKBKey(storm::window::Key key) noexcept;
    STORMKIT_PRIVATE storm::window::Key xkbKeyToStormkitKey(xkb_keysym_t key) noexcept;
} // namespace storm::window::details
