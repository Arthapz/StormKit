// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void Window::create(const std::string &title,
                               const VideoSettings &settings,
                               WindowStyle style) noexcept {
        m_impl->create(title, settings, style);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline void Window::close() noexcept { m_impl->close(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Window::pollEvent(Event &event, void *native_event) noexcept {
        return m_impl->pollEvent(event, native_event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Window::waitEvent(Event &event, void *native_event) noexcept {
        return m_impl->waitEvent(event, native_event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Window::setTitle(const std::string &title) noexcept { m_impl->setTitle(title); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Window::setVideoSettings(const VideoSettings &settings) noexcept {
        m_impl->setVideoSettings(settings);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::Extentu Window::size() const noexcept { return m_impl->size(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const std::string &Window::title() const noexcept { return m_impl->title(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline VideoSettings Window::videoSettings() const noexcept { return m_impl->videoSettings(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Window::isOpen() const noexcept { return m_impl->isOpen(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Window::isVisible() const noexcept { return m_impl->isVisible(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline NativeHandle Window::nativeHandle() const noexcept { return m_impl->nativeHandle(); }
} // namespace storm::window
