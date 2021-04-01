// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WaylandWindow.hpp"
#include "Log.hpp"
#include "WaylandKeyboard.hpp"
#include "WaylandMouse.hpp"

#include <storm/window/VideoSettings.hpp>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow() = default;

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::~WaylandWindow() {
    close();
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow(std::string title, const VideoSettings &settings, WindowStyle style) {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
void WaylandWindow::create(std::string title, const VideoSettings &settings, WindowStyle style) {
}

/////////////////////////////////////
/////////////////////////////////////
void WaylandWindow::close() noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
bool WaylandWindow::pollEvent(Event &event) noexcept {
    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
bool WaylandWindow::waitEvent(Event &event) noexcept {
    return AbstractWindow::waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void WaylandWindow::setTitle(std::string title) noexcept {
    m_title = std::move(title);
}

/////////////////////////////////////
/////////////////////////////////////
void WaylandWindow::setVideoSettings(const storm::window::VideoSettings &settings) noexcept {
    dlog("setVideoSettings not yet implemented");
}

/////////////////////////////////////
/////////////////////////////////////
const storm::core::Extentu &WaylandWindow::size() const noexcept {
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
bool WaylandWindow::isOpen() const noexcept {
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool WaylandWindow::isVisible() const noexcept {
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::NativeHandle WaylandWindow::nativeHandle() const noexcept {
    return reinterpret_cast<storm::window::NativeHandle>(const_cast<Handles *>(&m_handles));
}

std::vector<VideoSettings> WaylandWindow::getDesktopModes() {
    static auto video_settings = std::vector<VideoSettings> {};
    static auto init           = false;

    if (!init) { init = true; }

    return video_settings;
}

VideoSettings WaylandWindow::getDesktopFullscreenSize() {
    static auto video_setting = storm::window::VideoSettings {};
    static auto init          = false;

    if (!init) {
        const auto modes = getDesktopModes();

        for (const auto &mode : modes) {
            video_setting.size.width  = std::max(video_setting.size.width, mode.size.width);
            video_setting.size.height = std::max(video_setting.size.height, mode.size.height);
            video_setting.size.height = std::max(video_setting.size.depth, mode.size.depth);
        }

        init = true;
    }

    return video_setting;
}
