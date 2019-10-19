// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>

#undef None

namespace storm::window {
    enum class EventType : core::UInt8 {
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
            core::UInt16 width;
            core::UInt16 height;
        };
        struct KeyEvent {
            Key key;
        };
        struct MouseEvent {
            MouseButton button;
            core::Int16 x;
            core::Int16 y;
        };
        EventType type = EventType::None;
        union {
            ResizedEvent resized_event;
            KeyEvent key_event;
            MouseEvent mouse_event;
        };
    };
} // namespace storm::window
