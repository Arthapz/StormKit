// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Pimpl.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/AbstractInputHandler.hpp>
#include <storm/window/Fwd.hpp>

namespace storm::window {

    class STORM_PUBLIC InputHandler: public core::NonCopyable {
      public:
        explicit InputHandler(const Window &window);
        ~InputHandler();

        InputHandler(InputHandler &&);
        InputHandler &operator=(InputHandler &&);

        inline bool isKeyPressed(Key key) const noexcept;
        inline bool isMouseButtonPressed(MouseButton button) const noexcept;

        inline core::Position2u getMousePositionOnDesktop() const noexcept;
        inline void setMousePositionOnDesktop(core::Position2u position) noexcept;

        inline core::Position2i getMousePositionOnWindow() const noexcept;
        inline void setMousePositionOnWindow(core::Position2i position) noexcept;

        inline void setVirtualKeyboardVisible(bool visible) noexcept;

        // TODO implement a input handler like gainput
        // TODO implement touch events
      private:
        AbstractInputHandlerObserverPtr m_impl;
    };
} // namespace storm::window

#include "InputHandler.inl"
