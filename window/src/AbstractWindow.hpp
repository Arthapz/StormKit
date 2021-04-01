// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <queue>
#include <string>

/////////// - StormKit::window - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Event.hpp>
#include <storm/window/Fwd.hpp>
#include <storm/window/VideoSettings.hpp>
#include <storm/window/WindowStyle.hpp>

namespace storm::window::details {
    class STORMKIT_PUBLIC AbstractWindow: public core::NonCopyable {
      public:
        AbstractWindow();
        virtual ~AbstractWindow();

        AbstractWindow(AbstractWindow &&) noexcept;
        AbstractWindow &operator=(AbstractWindow &&) noexcept;

        virtual void
            create(std::string title, const VideoSettings &settings, WindowStyle style) = 0;
        virtual void close() noexcept                                                   = 0;

        virtual bool pollEvent(Event &event) noexcept;
        virtual bool waitEvent(Event &event) noexcept;

        virtual void setTitle(std::string title) noexcept                     = 0;
        virtual void setVideoSettings(const VideoSettings &settings) noexcept = 0;

        [[nodiscard]] virtual const core::Extentu &size() const noexcept = 0;

        [[nodiscard]] std::string_view title() const noexcept;
        [[nodiscard]] const VideoSettings &videoSettings() const noexcept;

        [[nodiscard]] virtual bool isOpen() const noexcept    = 0;
        [[nodiscard]] virtual bool isVisible() const noexcept = 0;

        [[nodiscard]] virtual NativeHandle nativeHandle() const noexcept = 0;
        void restoreWndProc() noexcept;

        void mouseDownEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept;
        void mouseUpEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept;

        void mouseMoveEvent(core::Int16 x, core::Int16 y) noexcept;

        void mouseEnteredEvent() noexcept;
        void mouseExitedEvent() noexcept;

        void keyDownEvent(Key key, char character) noexcept;
        void keyUpEvent(Key key, char character) noexcept;

        void closeEvent() noexcept;

        void resizeEvent(core::UInt16 width, core::UInt16 height) noexcept;
        void minimizeEvent() noexcept;
        void maximizeEvent() noexcept;

      protected:
        void pushEvent(Event event) noexcept;

        std::string m_title;
        VideoSettings m_video_settings;
        WindowStyle m_style;

        std::queue<Event> m_events;
    };
} // namespace storm::window::details

#include "AbstractWindow.inl"
