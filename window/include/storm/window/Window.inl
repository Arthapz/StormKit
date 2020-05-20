// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window {
    /////////////////////////////////////
    /////////////////////////////////////
     void Window::create(const std::string &title,
                               const VideoSettings &settings,
                               WindowStyle style) noexcept {
        m_impl->create(title, settings, style);
    }
    /////////////////////////////////////
    /////////////////////////////////////
     void Window::close() noexcept { m_impl->close(); }

    /////////////////////////////////////
    /////////////////////////////////////
     bool Window::pollEvent(Event &event, void *native_event) noexcept {
        return m_impl->pollEvent(event, native_event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     bool Window::waitEvent(Event &event, void *native_event) noexcept {
        return m_impl->waitEvent(event, native_event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void Window::setTitle(const std::string &title) noexcept { m_impl->setTitle(title); }

    /////////////////////////////////////
    /////////////////////////////////////
     void Window::setVideoSettings(const VideoSettings &settings) noexcept {
        m_impl->setVideoSettings(settings);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::Extentu Window::size() const noexcept { return m_impl->size(); }

    /////////////////////////////////////
    /////////////////////////////////////
     const std::string &Window::title() const noexcept { return m_impl->title(); }

    /////////////////////////////////////
    /////////////////////////////////////
     VideoSettings Window::videoSettings() const noexcept { return m_impl->videoSettings(); }

    /////////////////////////////////////
    /////////////////////////////////////
     bool Window::isOpen() const noexcept { return m_impl->isOpen(); }

    /////////////////////////////////////
    /////////////////////////////////////
     bool Window::isVisible() const noexcept { return m_impl->isVisible(); }

    /////////////////////////////////////
    /////////////////////////////////////
     NativeHandle Window::nativeHandle() const noexcept { return m_impl->nativeHandle(); }
} // namespace storm::window
