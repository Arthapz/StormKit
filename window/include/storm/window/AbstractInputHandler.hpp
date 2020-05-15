// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::window - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

namespace storm::window {
    class STORM_PUBLIC AbstractInputHandler: public core::NonCopyable {
      public:
        explicit AbstractInputHandler(const Window &window);
        virtual ~AbstractInputHandler() = 0;

        AbstractInputHandler(AbstractInputHandler &&);
        AbstractInputHandler &operator=(AbstractInputHandler &&);

        virtual bool isKeyPressed(Key key) const noexcept                    = 0;
        virtual bool isMouseButtonPressed(MouseButton button) const noexcept = 0;

        virtual core::Position2u getMousePositionOnDesktop() const noexcept        = 0;
        virtual void setMousePositionOnDesktop(core::Position2u position) noexcept = 0;

        virtual core::Position2i getMousePositionOnWindow() const noexcept        = 0;
        virtual void setMousePositionOnWindow(core::Position2i position) noexcept = 0;

        virtual void setVirtualKeyboardVisible(bool visible) noexcept = 0;

      protected:
        window::WindowConstObserverPtr m_window;
    };
} // namespace storm::window
