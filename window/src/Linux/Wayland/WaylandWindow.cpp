// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WaylandWindow.hpp"
#include "Log.hpp"
#include "WaylandKeyboard.hpp"
#include "WaylandMouse.hpp"

/////////// - Posix - ///////////
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

struct Payload {
    WLCompositorScoped &compositor;
    XDGShellScoped &shell;
    WLShmScoped &shm;
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
    } else if (std::char_traits<char>::compare(interface, "xdg_wm_base", size) == 0) {
        payload->shell.reset(reinterpret_cast<xdg_wm_base *>(
            wl_registry_bind(registry, id, &xdg_wm_base_interface, 1)));
    } else if (std::char_traits<char>::compare(interface, "wl_shm", size) == 0) {
        payload->shm.reset(
            reinterpret_cast<wl_shm *>(wl_registry_bind(registry, id, &wl_shm_interface, 1)));
    }
}

static auto global_registry_remover([[maybe_unused]] void *data,
                                    [[maybe_unused]] wl_registry *registry,
                                    std::uint32_t id) noexcept -> void {
    dlog("Wayland registry lost {}", id);
}

static auto xdg_toplevel_configure_handler([[maybe_unused]] void *data,
                                           [[maybe_unused]] xdg_toplevel *xdg_tl,
                                           int32_t width,
                                           int32_t height,
                                           [[maybe_unused]] wl_array *states) noexcept -> void {
    dlog("XDG Shell configure: {}:{}", width, height);
}

static auto xdg_toplevel_close_handler([[maybe_unused]] void *data,
                                       [[maybe_unused]] xdg_toplevel *xdg_tl) noexcept -> auto {
    dlog("XDG Shell close");
}

static auto xdg_surface_configure_handler([[maybe_unused]] void *data,
                                          xdg_surface *surface,
                                          std::uint32_t serial) noexcept -> void {
    xdg_surface_ack_configure(surface, serial);
}

static auto xdg_shell_ping_handler([[maybe_unused]] void *data,
                                   xdg_wm_base *xdg_shell,
                                   std::uint32_t serial) noexcept -> void {
    ilog("Ping received from shell");

    xdg_wm_base_pong(xdg_shell, serial);
}

static const auto stormkit_registry_listener =
    wl_registry_listener { .global        = global_registry_handler,
                           .global_remove = global_registry_remover };

static const auto stormkit_xdg_toplevel_listener =
    xdg_toplevel_listener { .configure = xdg_toplevel_configure_handler,
                            .close     = xdg_toplevel_close_handler };

static const auto stormkit_xdg_surface_listener =
    xdg_surface_listener { .configure = xdg_surface_configure_handler };

static const auto stormkit_xdg_shell_listener =
    xdg_wm_base_listener { .ping = xdg_shell_ping_handler };

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow() = default;

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::~WaylandWindow() {
    wl_display_flush(m_display.get());
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
auto WaylandWindow::create(std::string title, const VideoSettings &settings, WindowStyle style)
    -> void {
    m_title = title;

    m_display.reset(wl_display_connect(nullptr));
    if (m_display) dlog("Wayland context initialized");
    else {
        flog("Failed to initialize wayland");
        std::exit(EXIT_FAILURE);
    }

    m_registry.reset(wl_display_get_registry(m_display.get()));

    auto payload = Payload { m_compositor, m_xdg_shell, m_shm };

    wl_registry_add_listener(m_registry.get(), &stormkit_registry_listener, &payload);

    wl_display_dispatch(m_display.get());
    wl_display_roundtrip(m_display.get());

    if (m_compositor) dlog("Wayland compositor found !");
    else {
        flog("Failed to find a Wayland compositor");
        std::exit(EXIT_FAILURE);
    }
    if (m_xdg_shell) dlog("XDG shell found !");
    else {
        dlog("Failed to find a XDG shell");
        std::exit(EXIT_FAILURE);
    }

    xdg_wm_base_add_listener(m_xdg_shell.get(), &stormkit_xdg_shell_listener, nullptr);

    m_surface.reset(wl_compositor_create_surface(m_compositor.get()));
    m_xdg_surface.reset(xdg_wm_base_get_xdg_surface(m_xdg_shell.get(), m_surface.get()));

    xdg_surface_add_listener(m_xdg_surface.get(), &stormkit_xdg_surface_listener, nullptr);

    m_xdg_toplevel.reset(xdg_surface_get_toplevel(m_xdg_surface.get()));

    xdg_toplevel_add_listener(m_xdg_toplevel.get(), &stormkit_xdg_toplevel_listener, nullptr);

    xdg_toplevel_set_title(m_xdg_toplevel.get(), m_title.c_str());

    const auto byte_per_pixel = settings.bpp / sizeof(core::Byte);
    const auto buffer_size    = settings.size.width * settings.size.height * byte_per_pixel;
    const auto buffer_stride  = settings.size.width * byte_per_pixel;

    auto fd = syscall(SYS_memfd_create, "buffer", 0);
    ftruncate(fd, buffer_size);

    [[maybe_unused]] auto *data =
        mmap(nullptr, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    m_shm_pool.reset(wl_shm_create_pool(m_shm.get(), fd, buffer_size));

    m_buffer.reset(wl_shm_pool_create_buffer(m_shm_pool.get(),
                                             0,
                                             settings.size.width,
                                             settings.size.height,
                                             buffer_stride,
                                             WL_SHM_FORMAT_XRGB8888));

    wl_surface_attach(m_surface.get(), m_buffer.get(), 0, 0);

    wl_surface_commit(m_surface.get());
    wl_display_roundtrip(m_display.get());

    m_extent     = settings.size;
    m_is_visible = true;
    m_is_open    = true;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::close() noexcept -> void {
    wl_display_flush(m_display.get());

    m_buffer.reset(nullptr);
    m_shm_pool.reset(nullptr);
    m_shm.reset(nullptr);
    m_xdg_toplevel.reset(nullptr);
    m_xdg_surface.reset(nullptr);
    m_surface.reset(nullptr);
    m_xdg_shell.reset(nullptr);
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

    xdg_toplevel_set_title(m_xdg_toplevel.get(), m_title.c_str());
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setVideoSettings(const storm::window::VideoSettings &settings) noexcept
    -> void {
    dlog("setVideoSettings not yet implemented");
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::size() const noexcept -> const storm::core::Extentu & {
    return m_extent;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isOpen() const noexcept -> bool {
    wl_display_dispatch(m_display.get());

    return m_is_open;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isVisible() const noexcept -> bool {
    return m_is_visible;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::nativeHandle() const noexcept -> storm::window::NativeHandle {
    return reinterpret_cast<storm::window::NativeHandle>(const_cast<Handles *>(&m_handles));
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::getDesktopModes() -> std::vector<VideoSettings> {
    static auto video_settings = std::vector<VideoSettings> {};
    static auto init           = false;

    if (!init) { init = true; }

    return video_settings;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::getDesktopFullscreenSize() -> VideoSettings {
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
