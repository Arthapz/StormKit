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

namespace storm::window {
    class STORMKIT_PUBLIC AbstractWindow: public core::NonCopyable {
      public:
        AbstractWindow();
        virtual ~AbstractWindow() = 0;

        AbstractWindow(AbstractWindow &&);
        AbstractWindow &operator=(AbstractWindow &&);

        virtual void
            create(const std::string &title, const VideoSettings &settings, WindowStyle style) = 0;
        virtual void close() noexcept                                                          = 0;

        // BLC
        virtual bool pollEvent(Event &event, void *native_event = nullptr) noexcept;
        virtual bool waitEvent(Event &event, void *native_event = nullptr) noexcept = 0;

        virtual void setTitle(const std::string &title) noexcept              = 0;
        virtual void setVideoSettings(const VideoSettings &settings) noexcept = 0;

        virtual core::Extentu size() const noexcept = 0;

        inline const std::string &title() const noexcept;
        inline const VideoSettings &videoSettings() const noexcept;

        virtual bool isOpen() const noexcept    = 0;
        virtual bool isVisible() const noexcept = 0;

        virtual NativeHandle nativeHandle() const noexcept = 0;
        virtual void restoreWndProc() noexcept;

        void mouseDownEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept;
        void mouseUpEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept;

        void mouseMoveEvent(core::Int16 x, core::Int16 y) noexcept;

        void mouseEnteredEvent() noexcept;
        void mouseExitedEvent() noexcept;

        void keyDownEvent(Key key) noexcept;
        void keyUpEvent(Key key) noexcept;

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
} // namespace storm::window

#include "AbstractWindow.inl"
