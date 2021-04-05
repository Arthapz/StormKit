// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "../Utils.hpp"

#include "Log.hpp"
#include "WaylandKeyboard.hpp"
#include "WaylandMouse.hpp"
#include "WaylandWindow.hpp"

/////////// - Posix - ///////////
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#include <linux/input-event-codes.h>

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

/////////////////////////////////////
/////////////////////////////////////
auto wl_seat_capabilities_handler(void *data, wl_seat *seat, std::uint32_t capabilities) noexcept
    -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->seatCapabilities(seat, capabilities);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_seat_name_handler([[maybe_unused]] void *data, wl_seat *seat, const char *name) noexcept
    -> void {
    dlog("WL Seat found! {}", name);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_enter_handler(void *data,
                              wl_pointer *pointer,
                              std::uint32_t serial,
                              wl_surface *surface,
                              wl_fixed_t surface_x,
                              wl_fixed_t surface_y) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->pointerEnter(pointer, serial, surface, surface_x, surface_y);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_leave_handler(void *data,
                              wl_pointer *pointer,
                              std::uint32_t serial,
                              wl_surface *surface) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->pointerLeave(pointer, serial, surface);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_motion_handler(void *data,
                               wl_pointer *pointer,
                               std::uint32_t time,
                               wl_fixed_t surface_x,
                               wl_fixed_t surface_y) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->pointerMotion(pointer, time, surface_x, surface_y);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_button_handler(void *data,
                               wl_pointer *pointer,
                               std::uint32_t serial,
                               std::uint32_t time,
                               std::uint32_t button,
                               std::uint32_t state) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->pointerButton(pointer, serial, time, button, state);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_axis_handler([[maybe_unused]] void *data,
                             [[maybe_unused]] wl_pointer *pointer,
                             [[maybe_unused]] std::uint32_t time,
                             [[maybe_unused]] std::uint32_t axis,
                             [[maybe_unused]] wl_fixed_t value) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_frame_handler([[maybe_unused]] void *data,
                              [[maybe_unused]] wl_pointer *pointer) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_axis_source_handler([[maybe_unused]] void *data,
                                    [[maybe_unused]] wl_pointer *pointer,
                                    [[maybe_unused]] std::uint32_t axis_source) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_axis_stop_handler([[maybe_unused]] void *data,
                                  [[maybe_unused]] wl_pointer *pointer,
                                  [[maybe_unused]] std::uint32_t time,
                                  [[maybe_unused]] std::uint32_t axis) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_pointer_axis_discrete_handler([[maybe_unused]] void *data,
                                      [[maybe_unused]] wl_pointer *pointer,
                                      [[maybe_unused]] std::uint32_t axis,
                                      [[maybe_unused]] std::int32_t discrete) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_keyboard_keymap_handler(void *data,
                                wl_keyboard *keyboard,
                                std::uint32_t format,
                                std::int32_t fd,
                                std::uint32_t size) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->keyboardKeymap(keyboard, format, fd, size);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_keyboard_enter_handler(void *data,
                               wl_keyboard *keyboard,
                               std::uint32_t serial,
                               wl_surface *surface,
                               wl_array *keys) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->keyboardEnter(keyboard, serial, surface, keys);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_keyboard_leave_handler(void *data,
                               wl_keyboard *keyboard,
                               std::uint32_t serial,
                               wl_surface *surface) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->keyboardLeave(keyboard, serial, surface);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_keyboard_key_handler(void *data,
                             wl_keyboard *keyboard,
                             std::uint32_t serial,
                             std::uint32_t time,
                             std::uint32_t key,
                             std::uint32_t state) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->keyboardKey(keyboard, serial, time, key, state);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_keyboard_modifiers_handler(void *data,
                                   wl_keyboard *keyboard,
                                   std::uint32_t serial,
                                   std::uint32_t mods_depressed,
                                   std::uint32_t mods_latcher,
                                   std::uint32_t mods_locked,
                                   std::uint32_t group) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->keyboardModifiers(keyboard, serial, mods_depressed, mods_latcher, mods_locked, group);
}

/////////////////////////////////////
/////////////////////////////////////
auto wl_keyboard_repeat_info_handler(void *data,
                                     wl_keyboard *keyboard,
                                     std::int32_t rate,
                                     std::int32_t delay) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window->keyboardRepeatInfo(keyboard, rate, delay);
}

auto wp_relative_pointer_relative_motion_handler(void *data,
                                                 zwp_relative_pointer_v1 *pointer,
                                                 std::uint32_t time_hi,
                                                 std::uint32_t time_lw,
                                                 wl_fixed_t dx,
                                                 wl_fixed_t dy,
                                                 wl_fixed_t dx_unaccel,
                                                 wl_fixed_t dy_unaccel) noexcept -> void {
    auto *window = static_cast<WaylandWindow *>(data);
    window
        ->relativePointerRelativeMotion(pointer, time_hi, time_lw, dx, dy, dx_unaccel, dy_unaccel);
}

auto wp_locked_pointer_locked_handler(
    [[maybe_unused]] void *data,
    [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer) noexcept -> void {
}

auto wp_locked_pointer_unlocked_handler(
    [[maybe_unused]] void *data,
    [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer) noexcept -> void {
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

static const auto stormkit_wl_seat_listener =
    wl_seat_listener { .capabilities = wl_seat_capabilities_handler, .name = wl_seat_name_handler };

static const auto stormkit_wl_pointer_listener =
    wl_pointer_listener { .enter         = wl_pointer_enter_handler,
                          .leave         = wl_pointer_leave_handler,
                          .motion        = wl_pointer_motion_handler,
                          .button        = wl_pointer_button_handler,
                          .axis          = wl_pointer_axis_handler,
                          .frame         = wl_pointer_frame_handler,
                          .axis_source   = wl_pointer_axis_source_handler,
                          .axis_stop     = wl_pointer_axis_stop_handler,
                          .axis_discrete = wl_pointer_axis_discrete_handler };

static const auto stormkit_wl_keyboard_listener =
    wl_keyboard_listener { .keymap      = wl_keyboard_keymap_handler,
                           .enter       = wl_keyboard_enter_handler,
                           .leave       = wl_keyboard_leave_handler,
                           .key         = wl_keyboard_key_handler,
                           .modifiers   = wl_keyboard_modifiers_handler,
                           .repeat_info = wl_keyboard_repeat_info_handler };

static const auto stormkit_wl_touchscreen_listener = wl_touch_listener {};

static const auto stormkit_relative_pointer_listener =
    zwp_relative_pointer_v1_listener { .relative_motion =
                                           wp_relative_pointer_relative_motion_handler };

static const auto stormkit_locked_pointer_listener =
    zwp_locked_pointer_v1_listener { .locked   = wp_locked_pointer_locked_handler,
                                     .unlocked = wp_locked_pointer_unlocked_handler };

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow() {
    m_keyboard_state = std::array {
        KeyState { XKB_KEY_a, false },           KeyState { XKB_KEY_b, false },
        KeyState { XKB_KEY_c, false },           KeyState { XKB_KEY_d, false },
        KeyState { XKB_KEY_e, false },           KeyState { XKB_KEY_f, false },
        KeyState { XKB_KEY_g, false },           KeyState { XKB_KEY_h, false },
        KeyState { XKB_KEY_i, false },           KeyState { XKB_KEY_j, false },
        KeyState { XKB_KEY_k, false },           KeyState { XKB_KEY_l, false },
        KeyState { XKB_KEY_m, false },           KeyState { XKB_KEY_n, false },
        KeyState { XKB_KEY_o, false },           KeyState { XKB_KEY_p, false },
        KeyState { XKB_KEY_q, false },           KeyState { XKB_KEY_r, false },
        KeyState { XKB_KEY_s, false },           KeyState { XKB_KEY_t, false },
        KeyState { XKB_KEY_u, false },           KeyState { XKB_KEY_v, false },
        KeyState { XKB_KEY_w, false },           KeyState { XKB_KEY_x, false },
        KeyState { XKB_KEY_y, false },           KeyState { XKB_KEY_z, false },
        KeyState { XKB_KEY_0, false },           KeyState { XKB_KEY_1, false },
        KeyState { XKB_KEY_2, false },           KeyState { XKB_KEY_3, false },
        KeyState { XKB_KEY_4, false },           KeyState { XKB_KEY_5, false },
        KeyState { XKB_KEY_6, false },           KeyState { XKB_KEY_7, false },
        KeyState { XKB_KEY_8, false },           KeyState { XKB_KEY_9, false },
        KeyState { XKB_KEY_Escape, false },      KeyState { XKB_KEY_Control_L, false },
        KeyState { XKB_KEY_Shift_L, false },     KeyState { XKB_KEY_Alt_L, false },
        KeyState { XKB_KEY_Super_L, false },     KeyState { XKB_KEY_Control_R, false },
        KeyState { XKB_KEY_Shift_R, false },     KeyState { XKB_KEY_Alt_R, false },
        KeyState { XKB_KEY_Super_R, false },     KeyState { XKB_KEY_Menu, false },
        KeyState { XKB_KEY_bracketleft, false }, KeyState { XKB_KEY_bracketright, false },
        KeyState { XKB_KEY_semicolon, false },   KeyState { XKB_KEY_comma, false },
        KeyState { XKB_KEY_period, false },      KeyState { XKB_KEY_quoteleft, false },
        KeyState { XKB_KEY_slash, false },       KeyState { XKB_KEY_backslash, false },
        KeyState { XKB_KEY_dead_grave, false },  KeyState { XKB_KEY_equal, false },
        KeyState { XKB_KEY_hyphen, false },      KeyState { XKB_KEY_space, false },
        KeyState { XKB_KEY_Return, false },      KeyState { XKB_KEY_BackSpace, false },
        KeyState { XKB_KEY_Tab, false },         KeyState { XKB_KEY_Page_Up, false },
        KeyState { XKB_KEY_Page_Down, false },   KeyState { XKB_KEY_Begin, false },
        KeyState { XKB_KEY_End, false },         KeyState { XKB_KEY_Home, false },
        KeyState { XKB_KEY_Insert, false },      KeyState { XKB_KEY_Delete, false },
        KeyState { XKB_KEY_KP_Add, false },      KeyState { XKB_KEY_KP_Subtract, false },
        KeyState { XKB_KEY_KP_Multiply, false }, KeyState { XKB_KEY_KP_Divide, false },
        KeyState { XKB_KEY_Left, false },        KeyState { XKB_KEY_Right, false },
        KeyState { XKB_KEY_Up, false },          KeyState { XKB_KEY_Down, false },
        KeyState { XKB_KEY_KP_0, false },        KeyState { XKB_KEY_KP_1, false },
        KeyState { XKB_KEY_KP_2, false },        KeyState { XKB_KEY_KP_3, false },
        KeyState { XKB_KEY_KP_4, false },        KeyState { XKB_KEY_KP_5, false },
        KeyState { XKB_KEY_KP_6, false },        KeyState { XKB_KEY_KP_7, false },
        KeyState { XKB_KEY_KP_8, false },        KeyState { XKB_KEY_KP_9, false },
        KeyState { XKB_KEY_F1, false },          KeyState { XKB_KEY_F2, false },
        KeyState { XKB_KEY_F3, false },          KeyState { XKB_KEY_F4, false },
        KeyState { XKB_KEY_F5, false },          KeyState { XKB_KEY_F6, false },
        KeyState { XKB_KEY_F7, false },          KeyState { XKB_KEY_F8, false },
        KeyState { XKB_KEY_F9, false },          KeyState { XKB_KEY_F10, false },
        KeyState { XKB_KEY_F11, false },         KeyState { XKB_KEY_F12, false },
        KeyState { XKB_KEY_F13, false },         KeyState { XKB_KEY_F14, false },
        KeyState { XKB_KEY_F15, false },         KeyState { XKB_KEY_Pause, false },
    };

    m_xkb_context.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));

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

    m_cursor_theme.reset(wl_cursor_theme_load(nullptr, 24, m_shm.get()));
    auto cursor = wl_cursor_theme_get_cursor(m_cursor_theme.get(), "default");

    if (!cursor) cursor = wl_cursor_theme_get_cursor(m_cursor_theme.get(), "left_ptr");

    auto cursor_image = cursor->images[0];

    m_cursor_buffer.reset(wl_cursor_image_get_buffer(cursor_image));

    m_cursor_surface.reset(wl_compositor_create_surface(m_compositor.get()));
    wl_surface_attach(m_cursor_surface.get(), m_cursor_buffer.get(), 0, 0);
    wl_surface_commit(m_cursor_surface.get());

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

    m_locked_mouse_position.x = m_extent.width / 2;
    m_locked_mouse_position.y = m_extent.height / 2;

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

    // Fake Buffer
    m_buffer.release();

    // XDG
    m_xdg_decoration_manager.reset();
    m_xdg_toplevel.reset();
    m_xdg_surface.reset();

    // WL_Shell
    m_wlshell_surface.reset();

    // Base_Surface
    m_surface.reset();

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
auto WaylandWindow::lockMouse() noexcept -> void {
    if (!m_relative_pointer_manager) {
        elog("Can't lock mouse, {} protocol is not present",
             zwp_relative_pointer_manager_v1_interface.name);
        return;
    }
    if (!m_pointer_constraints) {
        elog("Can't lock mouse, {} protocol is not present",
             zwp_pointer_constraints_v1_interface.name);
        return;
    }

    m_mouse_state.position_in_window = m_locked_mouse_position;

    m_relative_pointer.reset(
        zwp_relative_pointer_manager_v1_get_relative_pointer(m_relative_pointer_manager.get(),
                                                             m_pointer.get()));
    zwp_relative_pointer_v1_add_listener(m_relative_pointer.get(),
                                         &stormkit_relative_pointer_listener,
                                         this);

    m_locked_pointer.reset(
        zwp_pointer_constraints_v1_lock_pointer(m_pointer_constraints.get(),
                                                m_surface.get(),
                                                m_pointer.get(),
                                                nullptr,
                                                ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT));

    zwp_locked_pointer_v1_add_listener(m_locked_pointer.get(),
                                       &stormkit_locked_pointer_listener,
                                       this);

    m_mouse_locked = true;

    hideMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::unlockMouse() noexcept -> void {
    m_locked_mouse_position = m_mouse_state.position_in_window;

    m_locked_pointer.reset();
    m_relative_pointer.reset();

    m_mouse_locked = false;

    unhideMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::hideMouse() noexcept -> void {
    wl_pointer_set_cursor(m_pointer.get(), m_pointer_serial, nullptr, 0, 0);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::unhideMouse() noexcept -> void {
    wl_pointer_set_cursor(m_pointer.get(), m_pointer_serial, m_cursor_surface.get(), 0, 0);
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
        if (core::checkFlag(m_style, WindowStyle::Fullscreen))
            xdg_toplevel_set_fullscreen(m_xdg_toplevel.get(), nullptr);
        else
            xdg_toplevel_unset_fullscreen(m_xdg_toplevel.get());
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

/////////////////////////////////////
/////////////////////////////////////
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
    } else if (std::char_traits<char>::compare(interface, wl_seat_interface.name, size) == 0) {
        m_seat.reset(
            reinterpret_cast<wl_seat *>(wl_registry_bind(registry, id, &wl_seat_interface, 5)));
        wl_seat_add_listener(m_seat.get(), &stormkit_wl_seat_listener, this);
    } else if (std::char_traits<char>::compare(interface,
                                               zxdg_decoration_manager_v1_interface.name,
                                               size) == 0) {
        m_xdg_decoration_manager.reset(reinterpret_cast<zxdg_decoration_manager_v1 *>(
            wl_registry_bind(registry, id, &zxdg_decoration_manager_v1_interface, 1)));
    } else if (std::char_traits<char>::compare(interface,
                                               zwp_pointer_constraints_v1_interface.name,
                                               size) == 0) {
        m_pointer_constraints.reset(reinterpret_cast<zwp_pointer_constraints_v1 *>(
            wl_registry_bind(registry, id, &zwp_pointer_constraints_v1_interface, 1)));
    } else if (std::char_traits<char>::compare(interface,
                                               zwp_relative_pointer_manager_v1_interface.name,
                                               size) == 0) {
        m_relative_pointer_manager.reset(reinterpret_cast<zwp_relative_pointer_manager_v1 *>(
            wl_registry_bind(registry, id, &zwp_relative_pointer_manager_v1_interface, 1)));
    }
}

/////////////////////////////////////
/////////////////////////////////////
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
                                      wl_array *state) noexcept -> void {
    dlog("XDG Shell configure: {}:{}", width, height);

    m_opened  = true;
    m_visible = width > 0 && height > 0;

    auto data = static_cast<xdg_toplevel_state *>(state->data);
    for (auto i = 0u; i < state->size; ++i) {
        const auto state = data[i];

        switch (state) {
            case XDG_TOPLEVEL_STATE_MAXIMIZED: AbstractWindow::maximizeEvent(); break;
            case XDG_TOPLEVEL_STATE_RESIZING: AbstractWindow::resizeEvent(width, height); break;
            default: break;
        }
    }

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

    AbstractWindow::closeEvent();
}

/////////////////////////////////////
/////////////////////////////////////
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
auto WaylandWindow::seatCapabilities(wl_seat *seat, std::uint32_t capabilities) noexcept -> void {
    if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) > 0 && !m_keyboard) {
        m_keyboard.reset(wl_seat_get_keyboard(m_seat.get()));
        wl_keyboard_add_listener(m_keyboard.get(), &stormkit_wl_keyboard_listener, this);
    }

    if ((capabilities & WL_SEAT_CAPABILITY_POINTER) > 0 && !m_pointer) {
        m_pointer.reset(wl_seat_get_pointer(m_seat.get()));
        wl_pointer_add_listener(m_pointer.get(), &stormkit_wl_pointer_listener, this);
    }

    if ((capabilities & WL_SEAT_CAPABILITY_TOUCH) > 0 && !m_touchscreen) {
        m_touchscreen.reset(wl_seat_get_touch(m_seat.get()));
        // wl_touch_add_listener(m_touchscreen.get(), &stormkit_wl_touchscreen_listener, this);
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerEnter([[maybe_unused]] wl_pointer *pointer,
                                 std::uint32_t serial,
                                 [[maybe_unused]] wl_surface *surface,
                                 [[maybe_unused]] wl_fixed_t surface_x,
                                 [[maybe_unused]] wl_fixed_t surface_y) noexcept -> void {
    m_pointer_serial = serial;

    if (!m_mouse_locked) unhideMouse();
    else
        hideMouse();

    AbstractWindow::mouseEnteredEvent();
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerLeave([[maybe_unused]] wl_pointer *pointer,
                                 [[maybe_unused]] std::uint32_t serial,
                                 [[maybe_unused]] wl_surface *surface) noexcept -> void {
    m_pointer_serial = serial;

    AbstractWindow::mouseExitedEvent();
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerMotion([[maybe_unused]] wl_pointer *pointer,
                                  [[maybe_unused]] std::uint32_t time,
                                  wl_fixed_t surface_x,
                                  wl_fixed_t surface_y) noexcept -> void {
    if (m_mouse_locked) return;

    m_mouse_state.position_in_window.x = wl_fixed_to_int(surface_x);
    m_mouse_state.position_in_window.y = wl_fixed_to_int(surface_y);

    AbstractWindow::mouseMoveEvent(m_mouse_state.position_in_window.x,
                                   m_mouse_state.position_in_window.y);
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerButton([[maybe_unused]] wl_pointer *pointer,
                                  [[maybe_unused]] std::uint32_t serial,
                                  [[maybe_unused]] std::uint32_t time,
                                  std::uint32_t button,
                                  std::uint32_t state) noexcept -> void {
#define BUTTON_HANDLER(a, b)                                                           \
    case a: {                                                                          \
        auto it  = core::ranges::find_if(m_mouse_state.button_state,                   \
                                        [](const auto &s) { return s.button == a; }); \
        it->down = down;                                                               \
        if (down)                                                                      \
            AbstractWindow::mouseDownEvent(b,                                          \
                                           m_mouse_state.position_in_window.x,         \
                                           m_mouse_state.position_in_window.y);        \
        else                                                                           \
            AbstractWindow::mouseUpEvent(b,                                            \
                                         m_mouse_state.position_in_window.x,           \
                                         m_mouse_state.position_in_window.y);          \
        break;                                                                         \
    }

    m_pointer_serial = serial;

    const auto down = !!state;

    switch (button) {
        BUTTON_HANDLER(BTN_LEFT, MouseButton::Left)
        BUTTON_HANDLER(BTN_RIGHT, MouseButton::Right)
        BUTTON_HANDLER(BTN_MIDDLE, MouseButton::Middle)
        BUTTON_HANDLER(BTN_FORWARD, MouseButton::Button1)
        BUTTON_HANDLER(BTN_BACK, MouseButton::Button2)
        default:
            if (down)
                AbstractWindow::mouseDownEvent(MouseButton::Unknow,
                                               m_mouse_state.position_in_window.x,
                                               m_mouse_state.position_in_window.y);
            else
                AbstractWindow::mouseUpEvent(MouseButton::Unknow,
                                             m_mouse_state.position_in_window.x,
                                             m_mouse_state.position_in_window.y);
    }

#undef BUTTON_HANDLER
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardKeymap([[maybe_unused]] wl_keyboard *keyboard,
                                   std::uint32_t format,
                                   std::int32_t fd,
                                   std::uint32_t size) noexcept -> void {
    if (format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        auto map_shm = reinterpret_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));

        updateKeymap(std::string_view { map_shm, size });

        munmap(map_shm, size);
        ::close(fd);
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardEnter([[maybe_unused]] wl_keyboard *keyboard,
                                  [[maybe_unused]] std::uint32_t serial,
                                  [[maybe_unused]] wl_surface *surface,
                                  wl_array *keys) noexcept -> void {
    AbstractWindow::gainedFocusEvent();

    auto data = static_cast<std::uint32_t *>(keys->data);
    for (auto i = 0u; i < keys->size; ++i) {
        const auto keycode = data[i] + 8;

        auto character = char {};

        const auto symbol = xkb_state_key_get_one_sym(m_xkb_state.get(), keycode);
        xkb_state_key_get_utf8(m_xkb_state.get(), keycode, &character, sizeof(char));

        const auto skey = xkbKeyToStormkitKey(symbol);

        AbstractWindow::keyDownEvent(skey, character);
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardLeave(wl_keyboard *keyboard,
                                  std::uint32_t serial,
                                  wl_surface *surface) noexcept -> void {
    AbstractWindow::lostFocusEvent();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardKey([[maybe_unused]] wl_keyboard *keyboard,
                                [[maybe_unused]] std::uint32_t serial,
                                [[maybe_unused]] std::uint32_t time,
                                std::uint32_t key,
                                std::uint32_t state) noexcept -> void {
    auto character = char {};

    const auto keycode = key + 8;

    const auto symbol = xkb_state_key_get_one_sym(m_xkb_state.get(), keycode);
    xkb_state_key_get_utf8(m_xkb_state.get(), keycode, &character, sizeof(char));

    const auto skey = xkbKeyToStormkitKey(symbol);

    const auto down = state == WL_KEYBOARD_KEY_STATE_PRESSED;

    auto it = core::ranges::find_if(m_keyboard_state,
                                    [symbol](const auto &s) { return s.key == symbol; });

    it->down = down;
    if (down) AbstractWindow::keyDownEvent(skey, character);
    else
        AbstractWindow::keyUpEvent(skey, character);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardModifiers([[maybe_unused]] wl_keyboard *keyboard,
                                      [[maybe_unused]] std::uint32_t serial,
                                      std::uint32_t mods_depressed,
                                      std::uint32_t mods_latched,
                                      std::uint32_t mods_locked,
                                      std::uint32_t group) noexcept -> void {
    xkb_state_update_mask(m_xkb_state.get(),
                          mods_depressed,
                          mods_latched,
                          mods_locked,
                          0,
                          0,
                          group);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardRepeatInfo([[maybe_unused]] wl_keyboard *keyboard,
                                       [[maybe_unused]] std::int32_t rate,
                                       [[maybe_unused]] std::int32_t delay) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::relativePointerRelativeMotion(zwp_relative_pointer_v1 *pointer,
                                                  std::uint32_t time_hi,
                                                  std::uint32_t time_lw,
                                                  wl_fixed_t dx,
                                                  wl_fixed_t dy,
                                                  wl_fixed_t dx_unaccel,
                                                  wl_fixed_t dy_unaccel) noexcept -> void {
    m_mouse_state.position_in_window.x += wl_fixed_to_int(dx_unaccel);
    m_mouse_state.position_in_window.y += wl_fixed_to_int(dy_unaccel);

    AbstractWindow::mouseMoveEvent(m_mouse_state.position_in_window.x,
                                   m_mouse_state.position_in_window.y);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setMousePosition(core::Vector2i position) const noexcept -> void {
    if (m_mouse_locked) {
        zwp_locked_pointer_v1_set_cursor_position_hint(m_locked_pointer.get(),
                                                       wl_fixed_from_int(position.x),
                                                       wl_fixed_from_int(position.y));
        wl_surface_commit(m_surface.get());
    }
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

    if (m_xdg_decoration_manager) {
        zxdg_decoration_manager_v1_get_toplevel_decoration(m_xdg_decoration_manager.get(),
                                                           m_xdg_toplevel.get());
    }

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

    wl_surface_attach(m_surface.get(), m_buffer.get(), 0, 0);
    wl_surface_commit(m_surface.get());

    if (core::checkFlag(m_style, WindowStyle::Fullscreen))
        xdg_toplevel_set_fullscreen(m_xdg_toplevel.get(), nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::updateKeymap(std::string_view keymap_string) noexcept -> void {
    m_xkb_keymap.reset(xkb_keymap_new_from_string(m_xkb_context.get(),
                                                  std::data(keymap_string),
                                                  XKB_KEYMAP_FORMAT_TEXT_V1,
                                                  XKB_KEYMAP_COMPILE_NO_FLAGS));

    if (!m_xkb_keymap) {
        elog("Failed to compile a keymap");
        return;
    }

    m_xkb_state.reset(xkb_state_new(m_xkb_keymap.get()));

    if (!m_xkb_state) {
        elog("Failed to create XKB state");
        return;
    }

    updateXKBMods();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::updateXKBMods() noexcept -> void {
    m_xkb_mods =
        XKBMods { .shift   = xkb_keymap_mod_get_index(m_xkb_keymap.get(), XKB_MOD_NAME_SHIFT),
                  .lock    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), XKB_MOD_NAME_CAPS),
                  .control = xkb_keymap_mod_get_index(m_xkb_keymap.get(), XKB_MOD_NAME_CTRL),
                  .mod1    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod1"),
                  .mod2    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod2"),
                  .mod3    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod3"),
                  .mod4    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod4"),
                  .mod5    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod5") };
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
