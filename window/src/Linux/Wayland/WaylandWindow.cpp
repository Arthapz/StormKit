// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WaylandWindow.hpp"
#include "Log.hpp"
#include "WaylandKeyboard.hpp"
#include "WaylandMouse.hpp"
#include "xdg-shell.h"

#include <storm/window/VideoSettings.hpp>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

struct Payload {
    WLCompositorScoped &compositor;
    WLShellScoped &shell;
};

static auto global_registry_handler(void *data,
                                    wl_registry *registry,
                                    std::uint32_t id,
                                    const char *interface,
                                    std::uint32_t version) noexcept -> void {
    dlog("Wayland registry acquired {} (id: {}, version: {})", interface, id, version);

    auto payload = reinterpret_cast<Payload *>(data);

    const auto size = std::char_traits<char>::length(interface);
    if (std::char_traits<char>::compare(interface, "wl_compositor", size) == 0) {
        payload->compositor.reset(reinterpret_cast<wl_compositor *>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 1)));
    } else if (std::char_traits<char>::compare(interface, "wl_shell", size) == 0) {
        payload->shell.reset(
            reinterpret_cast<wl_shell *>(wl_registry_bind(registry, id, &wl_shell_interface, 1)));
    }
}

static auto global_registry_remover([[maybe_unused]] void *data,
                                    [[maybe_unused]] wl_registry *registry,
                                    std::uint32_t id) noexcept -> void {
    dlog("Wayland registry lost {}", id);
}

static const auto registry_listener =
    wl_registry_listener { .global        = global_registry_handler,
                           .global_remove = global_registry_remover };

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
WaylandWindow::WaylandWindow(WaylandWindow &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::operator=(WaylandWindow &&) noexcept -> WaylandWindow & = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::create(std::string title, const VideoSettings &settings, WindowStyle style) -> void {
    m_display.reset(wl_display_connect(nullptr));
    if (m_display) dlog("Wayland context initialized");
    else {
        flog("Failed to initialize wayland");
        std::exit(EXIT_FAILURE);
    }

    m_registry.reset(wl_display_get_registry(m_display.get()));

    auto payload = Payload { m_compositor, m_shell };

    wl_registry_add_listener(m_registry.get(), &registry_listener, &payload);

    wl_display_dispatch(m_display.get());
    wl_display_roundtrip(m_display.get());

    if (m_compositor) dlog("Wayland compositor found !");
    else {
        flog("Failed to find a Wayland compositor");
        std::exit(EXIT_FAILURE);
    }
    if (m_shell) dlog("Wayland shell found !");
    else {
        dlog("Failed to find a Wayland shell");
        std::exit(EXIT_FAILURE);
    }

    m_surface.reset(wl_compositor_create_surface(m_compositor.get()));
    m_shell_surface.reset(wl_shell_get_shell_surface(m_shell.get(), m_surface.get()));
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::close() noexcept -> void {
    wl_display_flush(m_display.get());

    m_shell_surface.reset(nullptr);
    m_surface.reset(nullptr);
    m_shell.reset(nullptr);
    m_compositor.reset(nullptr);
    m_registry.reset(nullptr);
    m_display.reset(nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pollEvent(Event &event) noexcept -> bool {
    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::waitEvent(Event &event) noexcept -> bool {
    return AbstractWindow::waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setTitle(std::string title) noexcept -> void {
    m_title = std::move(title);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setVideoSettings(const storm::window::VideoSettings &settings) noexcept -> void {
    dlog("setVideoSettings not yet implemented");
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::size() const noexcept -> const storm::core::Extentu &{
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isOpen() const noexcept -> bool{
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isVisible() const noexcept -> bool{
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::nativeHandle() const noexcept -> storm::window::NativeHandle{
    return reinterpret_cast<storm::window::NativeHandle>(const_cast<Handles *>(&m_handles));
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::getDesktopModes() -> std::vector<VideoSettings>{
    static auto video_settings = std::vector<VideoSettings> {};
    static auto init           = false;

    if (!init) { init = true; }

    return video_settings;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::getDesktopFullscreenSize() -> VideoSettings{
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
