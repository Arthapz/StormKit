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

/////////////////////////////////////
/////////////////////////////////////
auto global_registry_handler(void *data,
                             wl_registry *registry,
                             std::uint32_t id,
                             const char *i,
                             std::uint32_t version) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->registryGlobal(registry, id, i, version);
}

/////////////////////////////////////
/////////////////////////////////////
auto global_registry_remover([[maybe_unused]] void *data,
                             [[maybe_unused]] wl_registry *registry,
                             std::uint32_t id) noexcept -> void {
    dlog("Wayland registry lost {}", id);
}

/////////////////////////////////////
/////////////////////////////////////
auto xdg_toplevel_configure_handler(void *data,
                                    xdg_toplevel *xdg_tl,
                                    int32_t width,
                                    int32_t height,
                                    wl_array *states) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->toplevelConfigure(xdg_tl, width, height, states);
}

/////////////////////////////////////
/////////////////////////////////////
auto xdg_toplevel_close_handler(void *data, xdg_toplevel *xdg_tl) noexcept -> auto {
    auto *window = static_cast<WaylandWindow *>(data);
    window->toplevelClose(xdg_tl);
}

/////////////////////////////////////
/////////////////////////////////////
auto xdg_surface_configure_handler(void *data, xdg_surface *surface, std::uint32_t serial) noexcept
    -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->surfaceConfigure(surface, serial);
}

/////////////////////////////////////
/////////////////////////////////////
auto xdg_shell_ping_handler([[maybe_unused]] void *data,
                            xdg_wm_base *xdg_shell,
                            std::uint32_t serial) noexcept -> void {
    ilog("Ping received from shell");

    xdg_wm_base_pong(xdg_shell, serial);
}

auto wl_shell_surface_configure_handler(void *data,
                                        wl_shell_surface *shell_surface,
                                        std::uint32_t edges,
                                        std::int32_t width,
                                        std::int32_t height) {
    auto *window = static_cast<WaylandWindow *>(data);
    window->shellSurfaceConfigure(shell_surface, edges, width, height);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_shell_ping_handler([[maybe_unused]] void *data,
                           wl_shell_surface *shell_surface,
                           std::uint32_t serial) noexcept -> void {
    ilog("Ping received from shell");

    wl_shell_surface_pong(shell_surface, serial);
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

static const auto stormkit_wl_shell_surface_listener =
    wl_shell_surface_listener { .ping      = wl_shell_ping_handler,
                                .configure = wl_shell_surface_configure_handler };

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow() {
    m_display.reset(wl_display_connect(nullptr));
    if (m_display) dlog("Wayland context initialized");
    else {
        flog("Failed to initialize wayland");
        std::exit(EXIT_FAILURE);
    }

    m_registry.reset(wl_display_get_registry(m_display.get()));

    wl_registry_add_listener(m_registry.get(), &stormkit_registry_listener, this);

    wl_display_dispatch(m_display.get());
    wl_display_roundtrip(m_display.get());

    if (m_compositor) dlog("Wayland compositor found !");
    else {
        flog("Failed to find a Wayland compositor");
        std::exit(EXIT_FAILURE);
    }

    m_handles.display = m_display.get();
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::~WaylandWindow() {
    wl_display_flush(m_display.get());
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow(std::string title, const VideoSettings &settings, WindowStyle style)
    : WaylandWindow {} {
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
    m_title          = title;
    m_extent         = settings.size;
    m_video_settings = settings;
    m_style          = style;

    m_surface.reset(wl_compositor_create_surface(m_compositor.get()));

    if (m_xdg_shell) {
        dlog("XDG shell found !");
        createXDGShell();
    } else {
        dlog("XDGShell not found, falling back to WLShell");

        if (m_wayland_shell) createWaylandShell();
        else {
            flog("WLShell not found, abording...");
            std::exit(EXIT_FAILURE);
        }
    }

    m_handles.surface = m_surface.get();
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
    m_wlshell_surface.reset(nullptr);
    m_surface.reset(nullptr);
    m_xdg_shell.reset(nullptr);

    m_configured = false;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pollEvent(Event &event) noexcept -> bool {
    while (wl_display_prepare_read(m_display.get()) != 0)
        wl_display_dispatch_pending(m_display.get());
    wl_display_flush(m_display.get());
    wl_display_read_events(m_display.get());
    wl_display_dispatch_pending(m_display.get());

    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::waitEvent(Event &event) noexcept -> bool {
    while (!m_configured) wl_display_dispatch(m_display.get());
    while (wl_display_prepare_read(m_display.get()) != 0) wl_display_dispatch(m_display.get());
    wl_display_flush(m_display.get());

    return AbstractWindow::waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setTitle(std::string title) noexcept -> void {
    m_title = std::move(title);

    if (m_xdg_toplevel) {
        xdg_toplevel_set_title(m_xdg_toplevel.get(), m_title.c_str());
        xdg_toplevel_set_app_id(m_xdg_toplevel.get(), m_title.c_str());
    } else {
        wl_shell_surface_set_title(m_wlshell_surface.get(), m_title.c_str());
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setVideoSettings(const storm::window::VideoSettings &settings) noexcept
    -> void {
    if (m_xdg_toplevel) {
        xdg_surface_set_window_geometry(m_xdg_surface.get(),
                                        0,
                                        0,
                                        settings.size.width,
                                        settings.size.height);
    } else {
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::size() const noexcept -> const storm::core::Extentu & {
    while (!m_configured) wl_display_dispatch(m_display.get());

    return m_extent;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isOpen() const noexcept -> bool {
    while (!m_configured) wl_display_dispatch(m_display.get());

    return m_opened;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isVisible() const noexcept -> bool {
    while (!m_configured) wl_display_dispatch(m_display.get());

    return m_visible;
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

auto WaylandWindow::registryGlobal(wl_registry *registry,
                                   std::uint32_t id,
                                   const char *interface,
                                   std::uint32_t version) noexcept -> void {
    dlog("Wayland registry acquired {} (id: {}, version: {})", interface, id, version);

    const auto size = std::char_traits<char>::length(interface);
    if (std::char_traits<char>::compare(interface, wl_compositor_interface.name, size) == 0) {
        m_compositor.reset(reinterpret_cast<wl_compositor *>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 3)));
    } else if (std::char_traits<char>::compare(interface, xdg_wm_base_interface.name, size) == 0) {
        m_xdg_shell.reset(reinterpret_cast<xdg_wm_base *>(
            wl_registry_bind(registry, id, &xdg_wm_base_interface, 1)));
        xdg_wm_base_add_listener(m_xdg_shell.get(), &stormkit_xdg_shell_listener, nullptr);
    } else if (std::char_traits<char>::compare(interface, wl_shm_interface.name, size) == 0) {
        m_shm.reset(
            reinterpret_cast<wl_shm *>(wl_registry_bind(registry, id, &wl_shm_interface, 1)));
    } else if (std::char_traits<char>::compare(interface, wl_shell_interface.name, size) == 0) {
        m_wayland_shell.reset(
            reinterpret_cast<wl_shell *>(wl_registry_bind(registry, id, &wl_shell_interface, 1)));
    }
}

auto WaylandWindow::surfaceConfigure(xdg_surface *surface, std::uint32_t serial) noexcept -> void {
    dlog("XDG surface configure, serial: {}", serial);

    xdg_surface_ack_configure(surface, serial);
    m_configured = true;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::toplevelConfigure([[maybe_unused]] xdg_toplevel *xdg_tl,
                                      std::int32_t width,
                                      std::int32_t height,
                                      [[maybe_unused]] wl_array *state) noexcept -> void {
    dlog("XDG Shell configure: {}:{}", width, height);

    m_opened  = true;
    m_visible = width > 0 && height > 0;

    if (width <= 0 || height <= 0) return;

    m_extent.width  = width;
    m_extent.height = height;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::toplevelClose([[maybe_unused]] xdg_toplevel *xdg_tl) noexcept -> void {
    m_opened        = false;
    m_visible       = false;
    m_extent.width  = 0;
    m_extent.height = 0;
}

auto WaylandWindow::shellSurfaceConfigure([[maybe_unused]] wl_shell_surface *xdg_tl,
                                          [[maybe_unused]] std::uint32_t edges,
                                          std::int32_t width,
                                          std::int32_t height) noexcept -> void {
    dlog("WL Shell configure: {}:{}", width, height);

    m_opened  = true;
    m_visible = width > 0 && height > 0;

    if (width <= 0 || height <= 0) return;

    m_extent.width  = width;
    m_extent.height = height;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::createXDGShell() noexcept -> void {
    m_xdg_surface.reset(xdg_wm_base_get_xdg_surface(m_xdg_shell.get(), m_surface.get()));

    xdg_surface_add_listener(m_xdg_surface.get(), &stormkit_xdg_surface_listener, this);

    m_xdg_toplevel.reset(xdg_surface_get_toplevel(m_xdg_surface.get()));

    xdg_toplevel_add_listener(m_xdg_toplevel.get(), &stormkit_xdg_toplevel_listener, this);

    xdg_toplevel_set_title(m_xdg_toplevel.get(), m_title.c_str());
    xdg_toplevel_set_app_id(m_xdg_toplevel.get(), m_title.c_str());
    xdg_toplevel_set_min_size(m_xdg_toplevel.get(),
                              m_video_settings.size.width,
                              m_video_settings.size.height);

    wl_surface_damage(m_surface.get(), 0, 0, m_extent.width, m_extent.height);
    createPixelbuffer();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::createWaylandShell() noexcept -> void {
    m_wlshell_surface.reset(wl_shell_get_shell_surface(m_wayland_shell.get(), m_surface.get()));
    wl_shell_surface_add_listener(m_wlshell_surface.get(),
                                  &stormkit_wl_shell_surface_listener,
                                  this);
    wl_shell_surface_set_toplevel(m_wlshell_surface.get());

    wl_surface_damage(m_surface.get(), 0, 0, m_extent.width, m_extent.height);
    createPixelbuffer();
}

auto WaylandWindow::createPixelbuffer() noexcept -> void {
    const auto byte_per_pixel = m_video_settings.bpp / sizeof(core::Byte);
    const auto buffer_size =
        m_video_settings.size.width * m_video_settings.size.height * byte_per_pixel;
    const auto buffer_stride = m_video_settings.size.width * byte_per_pixel;

    auto fd = syscall(SYS_memfd_create, "buffer", 0);
    ftruncate(fd, buffer_size);

    [[maybe_unused]] auto *data =
        mmap(nullptr, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    m_shm_pool.reset(wl_shm_create_pool(m_shm.get(), fd, buffer_size));

    m_buffer.reset(wl_shm_pool_create_buffer(m_shm_pool.get(),
                                             0,
                                             m_video_settings.size.width,
                                             m_video_settings.size.height,
                                             buffer_stride,
                                             WL_SHM_FORMAT_XRGB8888));

    // wl_surface_attach(m_surface.get(), m_buffer.get(), 0, 0);
    wl_surface_commit(m_surface.get());
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
