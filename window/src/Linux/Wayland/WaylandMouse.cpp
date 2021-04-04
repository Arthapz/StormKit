// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WaylandMouse.hpp"
#include "WaylandWindow.hpp"

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
WaylandMouse::WaylandMouse(const AbstractWindow &window) noexcept : Mouse { window } {
    m_wayland_window = static_cast<WaylandWindowConstPtr>(m_window);
}

/////////////////////////////////////
/////////////////////////////////////
WaylandMouse::~WaylandMouse() = default;

/////////////////////////////////////
/////////////////////////////////////
WaylandMouse::WaylandMouse(WaylandMouse &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::operator=(WaylandMouse &&) noexcept -> WaylandMouse & = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::isButtonPressed(MouseButton button) const noexcept -> bool {
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::getPositionOnDesktop() const noexcept -> core::Position2u {
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::setPositionOnDesktop(core::Position2u position) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::getPositionOnWindow() const noexcept -> core::Position2i {
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::setPositionOnWindow(core::Position2i position) noexcept -> void {
}
