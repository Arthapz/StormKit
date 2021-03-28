// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::window - ///////////
#include <storm/window/AbstractInputHandler.hpp>

namespace storm::window {
    class InputHandlerImpl: public AbstractInputHandler {
      public:
        explicit InputHandlerImpl(const Window &window);
        ~InputHandlerImpl() override;

        InputHandlerImpl(InputHandlerImpl &&);
        InputHandlerImpl &operator=(InputHandlerImpl &&);

        bool isKeyPressed(Key key) const noexcept override;
        bool isMouseButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getMousePositionOnDesktop() const noexcept override;
        void setMousePositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getMousePositionOnWindow() const noexcept override;
        void setMousePositionOnWindow(core::Position2i position) noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;
    };
} // namespace storm::window
