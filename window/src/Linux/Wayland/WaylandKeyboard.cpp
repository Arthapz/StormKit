// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WaylandKeyboard.hpp"
#include "Log.hpp"
#include "WaylandWindow.hpp"

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
WaylandKeyboard::WaylandKeyboard(const AbstractWindow &window) noexcept : Keyboard { window } {
    m_xcb_window = static_cast<WaylandWindowConstPtr>(m_window);
}

/////////////////////////////////////
/////////////////////////////////////
WaylandKeyboard::~WaylandKeyboard() = default;

/////////////////////////////////////
/////////////////////////////////////
WaylandKeyboard::WaylandKeyboard(WaylandKeyboard &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandKeyboard::operator=(WaylandKeyboard &&) noexcept -> WaylandKeyboard & = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandKeyboard::isKeyPressed(Key key) const noexcept -> bool {
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandKeyboard::setVirtualKeyboardVisible(bool visible) noexcept -> void {
    dlog("WaylandKeyboard::setVirtualKeyboardVisible isn't yet implemented");
}
