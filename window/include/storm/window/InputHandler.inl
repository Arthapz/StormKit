// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window {
    /////////////////////////////////////
    /////////////////////////////////////
     bool InputHandler::isKeyPressed(Key key) const noexcept {
        return m_impl->isKeyPressed(key);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     bool InputHandler::isMouseButtonPressed(MouseButton button) const noexcept {
        return m_impl->isMouseButtonPressed(button);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::Position2u InputHandler::getMousePositionOnDesktop() const noexcept {
        return m_impl->getMousePositionOnDesktop();
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void InputHandler::setMousePositionOnDesktop(core::Position2u position) noexcept {
        m_impl->setMousePositionOnDesktop(std::move(position));
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::Position2i InputHandler::getMousePositionOnWindow() const noexcept {
        return m_impl->getMousePositionOnWindow();
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void InputHandler::setMousePositionOnWindow(core::Position2i position) noexcept {
        m_impl->setMousePositionOnWindow(std::move(position));
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void InputHandler::setVirtualKeyboardVisible(bool visible) noexcept {
        m_impl->setVirtualKeyboardVisible(visible);
    }
} // namespace storm::window
