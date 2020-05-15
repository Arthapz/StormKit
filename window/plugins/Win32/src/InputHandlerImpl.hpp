// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/window/InputHandler.hpp>

namespace storm::window {
    class InputHandlerImplImpl;
    class STORM_PUBLIC InputHandlerImpl: public core::NonCopyable {
      public:
        InputHandlerImpl();
        ~InputHandlerImpl();

        InputHandlerImpl(InputHandlerImpl &&);
        InputHandlerImpl &operator=(InputHandlerImpl &&);

        static bool isKeyPressed(Key key);
        static bool isMouseButtonPressed(MouseButton button);
        static void setMousePosition(core::Position2u position);
        static void setMousePosition(core::Position2i position, const Window &relative_to);
        static core::Position2u getMousePosition();
        static core::Position2i getMousePosition(const Window &relative_to);

        static void setVirtualKeyboardVisible(bool visible);
    };
} // namespace storm::window
