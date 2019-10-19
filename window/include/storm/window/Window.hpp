// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Flags.hpp>
#include <storm/core/Platform.hpp>

#include <storm/window/AbstractWindow.hpp>

namespace storm::window {
    class STORM_PUBLIC Window: public core::NonCopyable {
      public:
        Window() noexcept;
        Window(const std::string &title, const VideoSettings &settings, WindowStyle style) noexcept;
        ~Window();

        Window(Window &&);
        Window &operator=(Window &&);

        inline void create(const std::string &title,
                           const VideoSettings &settings,
                           WindowStyle style) noexcept;
        inline void close() noexcept;

        void display() noexcept;

        inline bool pollEvent(Event &event, void *native_event = nullptr) noexcept;
        inline bool waitEvent(Event &event, void *native_event = nullptr) noexcept;

        inline void setTitle(const std::string &title) noexcept;
        inline void setVideoSettings(const VideoSettings &settings) noexcept;

        inline core::Extentu size() const noexcept;
        inline const std::string &title() const noexcept;
        inline VideoSettings videoSettings() const noexcept;

        inline bool isOpen() const noexcept;
        inline bool isVisible() const noexcept;

        inline NativeHandle nativeHandle() const noexcept;

      private:
        AbstractWindowOwnedPtr m_impl;
    };
} // namespace storm::window

#include "Window.inl"
