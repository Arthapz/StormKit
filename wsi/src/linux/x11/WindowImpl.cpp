
// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.wsi.details.x11.windowimpl;

import stormkit.wsi.details.x11.log;
import stormkit.wsi.details.x11.utils;
#else
    /////////// - StormKit::wsi - ///////////
    #include "WindowImpl.mpp"
    #include "Log.mpp"
    #include "Utils.mpp"
#endif

/////////// - SL - ///////////
#include <cstddef>
#include <cstdlib>

/////////// - XCB - ///////////
extern "C" {
#include <xcb/randr.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xfixes.h>
#include <xcb/xinput.h>

/////////// - XKB - ///////////
#define explicit _explicit
#include <xcb/xkb.h>
#undef explicit

#include <xkbcommon/xkbcommon-x11.h>
}

namespace stormkit::wsi::details::x11 {
    static constexpr auto WM_HINTS_STR            = std::string_view("_MOTIF_WM_HINTS");
    static constexpr auto WM_PROTOCOLS            = std::string_view("WM_PROTOCOLS");
    static constexpr auto WM_DELETE_WINDOW        = std::string_view("WM_DELETE_WINDOW");
    static constexpr auto WM_STATE_STR            = std::string_view("_NET_WM_STATE");
    static constexpr auto WM_STATE_FULLSCREEN_STR = std::string_view("_NET_WM_STATE_FULLSCREEN");

    static constexpr auto _NET_WM_STATE_REMOVE = 0; // remove/unset property
    static constexpr auto _NET_WM_STATE_ADD    = 1; // add/set property
    static constexpr auto _NET_WM_STATE_TOGGLE = 2; // toggle property

    static constexpr auto MOUSE_RAW_EVENTS =
        core::UInt32 { XCB_INPUT_XI_EVENT_MASK_RAW_BUTTON_PRESS |
                       XCB_INPUT_XI_EVENT_MASK_RAW_BUTTON_RELEASE |
                       XCB_INPUT_XI_EVENT_MASK_RAW_MOTION };
    static constexpr auto KEYBOARD_RAW_EVENTS =
        core::UInt32 { XCB_INPUT_XI_EVENT_MASK_RAW_KEY_PRESS |
                       XCB_INPUT_XI_EVENT_MASK_RAW_KEY_RELEASE };
    static constexpr auto KEYBOARD_EVENTS =
        core::UInt32 { XCB_INPUT_XI_EVENT_MASK_KEY_PRESS | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE };

    static constexpr auto XINPUT_MASK_MODIFIERS = core::UInt32 { XCB_INPUT_MODIFIER_MASK_ANY };

    auto isExtEvent(xcb_generic_event_t *event, int opcode) noexcept -> bool {
        return reinterpret_cast<xcb_ge_generic_event_t *>(event)->extension == opcode;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::WindowImpl() {
        m_connection  = getXCBConnection();
        m_xkb_context = getXKBContext();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::WindowImpl(std::string title, const VideoSettings &settings, WindowStyle style)
        : WindowImpl {} {
        create(std::move(title), settings, style);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::~WindowImpl() { xcb_flush(getXCBConnection()); }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::WindowImpl(WindowImpl &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::operator=(WindowImpl &&) noexcept -> WindowImpl & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::create(std::string title, const VideoSettings &settings, WindowStyle style)
        -> void {
        static constexpr const auto EVENTS =
            XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS |
            XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_BUTTON_MOTION |
            XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
            XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
            XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_VISIBILITY_CHANGE |
            XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_EXPOSURE;

        auto screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection)).data;
        m_window.reset(xcb_generate_id(m_connection));

        core::UInt32 value_list[] = { screen->white_pixel, screen->black_pixel, EVENTS };
        auto cookie =
            xcb_create_window_checked(m_connection,
                                      XCB_COPY_FROM_PARENT,
                                      m_window,
                                      screen->root,
                                      0,
                                      0,
                                      settings.size.width,
                                      settings.size.height,
                                      1,
                                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                      screen->root_visual,
                                      XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK,
                                      value_list);

        auto error = xcb_request_check(m_connection, cookie);
        if (error) [[unlikely]]
            elog("Failed to create window\n    > reason: {}", getXCBError(error));

        m_extent = settings.size;

        m_locked_mouse_position.x = m_extent.width / 2;
        m_locked_mouse_position.y = m_extent.height / 2;

        // init key_symbol map, this is needed to extract the keysymbol from event
        m_key_symbols.reset(xcb_key_symbols_alloc(m_connection));

        auto xkb_ext_reply = xcb_get_extension_data(m_connection, &xcb_xkb_id);
        STORMKIT_EXPECTS(xkb_ext_reply);
        {
            STORMKIT_RAII_CAPSULE(Reply, xcb_xkb_use_extension_reply_t, free)
            auto cookie =
                xcb_xkb_use_extension(m_connection, XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);
            auto reply = ReplyScoped { xcb_xkb_use_extension_reply(m_connection, cookie, nullptr) };

            STORMKIT_EXPECTS(reply);
            STORMKIT_EXPECTS_MESSAGE(reply->supported,
                                     "The XKB extension is not supported on this X server");
        }

        auto xfixes_ext_reply = xcb_get_extension_data(m_connection, &xcb_xfixes_id);
        STORMKIT_EXPECTS(xfixes_ext_reply);
        {
            STORMKIT_RAII_CAPSULE(Reply, xcb_xfixes_query_version_reply_t, free)

            auto cookie = xcb_xfixes_query_version(m_connection, 4, 0);
            auto reply =
                ReplyScoped { xcb_xfixes_query_version_reply(m_connection, cookie, nullptr) };

            STORMKIT_EXPECTS(reply);
        }

        auto xcb_input_ext_reply = xcb_get_extension_data(m_connection, &xcb_input_id);
        STORMKIT_EXPECTS(xcb_input_ext_reply);
        {
            STORMKIT_RAII_CAPSULE(Reply, xcb_input_xi_query_version_reply_t, free)

            auto cookie = xcb_input_xi_query_version(m_connection, 2, XCB_INPUT_MINOR_VERSION);
            auto reply =
                ReplyScoped { xcb_input_xi_query_version_reply(m_connection, cookie, nullptr) };

            STORMKIT_EXPECTS(reply);

            m_xi_opcode = xcb_input_ext_reply->major_opcode;
        }

        updateKeymap();

        constexpr auto required_map_parts =
            core::UInt16 { XCB_XKB_MAP_PART_KEY_TYPES | XCB_XKB_MAP_PART_KEY_SYMS |
                           XCB_XKB_MAP_PART_MODIFIER_MAP | XCB_XKB_MAP_PART_EXPLICIT_COMPONENTS |
                           XCB_XKB_MAP_PART_KEY_ACTIONS | XCB_XKB_MAP_PART_KEY_BEHAVIORS |
                           XCB_XKB_MAP_PART_VIRTUAL_MODS | XCB_XKB_MAP_PART_VIRTUAL_MOD_MAP };

        constexpr auto required_events =
            core::UInt16 { XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY |
                           XCB_XKB_EVENT_TYPE_STATE_NOTIFY };

        // XKB events are reported to all interested clients without regard
        // to the current keyboard input focus or grab state
        xcb_xkb_select_events_checked(m_connection,
                                      XCB_XKB_ID_USE_CORE_KBD,
                                      required_events,
                                      0,
                                      required_events,
                                      required_map_parts,
                                      required_map_parts,
                                      nullptr);

        auto mask = KEYBOARD_EVENTS | MOUSE_RAW_EVENTS;

        xcb_input_xi_passive_grab_device(m_connection,
                                         XCB_CURRENT_TIME,
                                         m_window,
                                         XCB_CURSOR_NONE,
                                         0,
                                         XCB_INPUT_DEVICE_ALL,
                                         1,
                                         1,
                                         XCB_INPUT_GRAB_TYPE_BUTTON | XCB_INPUT_GRAB_TYPE_KEYCODE,
                                         XCB_INPUT_GRAB_MODE_22_ASYNC,
                                         XCB_INPUT_GRAB_MODE_22_ASYNC,
                                         XCB_INPUT_GRAB_OWNER_NO_OWNER,
                                         &mask,
                                         &XINPUT_MASK_MODIFIERS);

        // set the.wsi.name
        setTitle(std::move(title));

        // set the.wsi.s style
        // checking if the.wsi.manager support.wsi.decoration
        static constexpr const auto MWM_HINTS_FUNCTIONS   = 1 << 0;
        static constexpr const auto MWM_HINTS_DECORATIONS = 1 << 1;

        static constexpr const auto MWM_DECOR_BORDER   = 1 << 1;
        static constexpr const auto MWM_DECOR_RESIZE   = 1 << 2;
        static constexpr const auto MWM_DECOR_TITLE    = 1 << 3;
        static constexpr const auto MWM_DECOR_MENU     = 1 << 4;
        static constexpr const auto MWM_DECOR_MINIMIZE = 1 << 5;
        static constexpr const auto MWM_DECOR_MAXIMIZE = 1 << 6;

        static constexpr const auto MWM_FUNC_RESIZE   = 1 << 1;
        static constexpr const auto MWM_FUNC_MOVE     = 1 << 2;
        static constexpr const auto MWM_FUNC_MINIMIZE = 1 << 3;
        static constexpr const auto MWM_FUNC_MAXIMIZE = 1 << 4;
        static constexpr const auto MWM_FUNC_CLOSE    = 1 << 5;

        m_window_hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;

        if (core::checkFlag(style, WindowStyle::TitleBar)) {
            m_window_hints.decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU;
            m_window_hints.functions |= MWM_FUNC_MOVE;
        }

        if (core::checkFlag(style, WindowStyle::Close)) {
            m_window_hints.decorations |= 0;
            m_window_hints.functions |= MWM_FUNC_CLOSE;
        }

        if (core::checkFlag(style, WindowStyle::Minimizable)) {
            m_window_hints.decorations |= MWM_DECOR_MINIMIZE;
            m_window_hints.functions |= MWM_FUNC_MINIMIZE;
        }

        if (core::checkFlag(style, WindowStyle::Resizable)) {
            m_window_hints.decorations |= MWM_DECOR_RESIZE | MWM_DECOR_MAXIMIZE;
            m_window_hints.functions |= MWM_FUNC_RESIZE | MWM_FUNC_MAXIMIZE;
        } else {
            auto size_hints = xcb_size_hints_t {};

            xcb_icccm_size_hints_set_min_size(&size_hints, m_extent.width, m_extent.height);
            xcb_icccm_size_hints_set_max_size(&size_hints, m_extent.width, m_extent.height);

            xcb_icccm_set_wm_normal_hints(m_connection, m_window, &size_hints);
        }

        xcb_flush(m_connection);

        m_handles.connection  = m_connection;
        m_handles.window      = m_window;
        m_handles.key_symbols = m_key_symbols.get();

        auto close_atom    = getXCBAtom(WM_DELETE_WINDOW, false);
        auto protocol_atom = getXCBAtom(WM_PROTOCOLS, true);
        if (!protocol_atom) [[unlikely]]
            elog("Failed to get atom {}\n    > reason: {}",
                 WM_PROTOCOLS,
                 protocol_atom.error().message);
        else if (!close_atom) [[unlikely]]
            elog("Failed to get atom {}\n    > reason: {}",
                 WM_DELETE_WINDOW,
                 close_atom.error().message);
        else
            xcb_change_property(m_connection,
                                XCB_PROP_MODE_REPLACE,
                                m_window,
                                *protocol_atom,
                                XCB_ATOM_ATOM,
                                32,
                                1,
                                &(*close_atom));

        auto state_atom = getXCBAtom(WM_STATE_STR, false);
        if (!state_atom) [[unlikely]]
            elog("Failed to get atom {}\n    > reason: {}",
                 WM_STATE_STR,
                 state_atom.error().message);
        else
            xcb_change_property(m_connection,
                                XCB_PROP_MODE_REPLACE,
                                m_window,
                                *state_atom,
                                XCB_ATOM_ATOM,
                                32,
                                0,
                                nullptr);

        xcb_map_window(m_connection, m_window);

        xcb_flush(m_connection);

        m_title   = std::move(title);
        m_visible = true;
        m_open    = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::close() noexcept -> void {
        m_window.reset();

        m_title.clear();
        m_open    = false;
        m_visible = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::pollEvent(Event &event) noexcept -> bool {
        STORMKIT_RAII_CAPSULE(Event, xcb_generic_event_t, free)

        for (auto xevent = EventScoped { xcb_poll_for_event(m_connection) }; xevent;
             xevent.reset(xcb_poll_for_event(m_connection)))
            processEvents(*xevent);

        return WindowImplBase::pollEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::waitEvent(Event &event) noexcept -> bool {
        auto xevent = xcb_poll_for_event(m_connection);

        while (!xevent) xevent = xcb_poll_for_event(m_connection);

        processEvents(*xevent);

        return WindowImplBase::waitEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setTitle(std::string title) noexcept -> void {
        xcb_change_property(m_connection,
                            XCB_PROP_MODE_REPLACE,
                            m_window,
                            XCB_ATOM_WM_NAME,
                            XCB_ATOM_STRING,
                            8,
                            core::as<core::UInt32>(title.length()),
                            title.c_str());

        xcb_flush(m_connection);

        m_title = std::move(title);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setFullscreenEnabled(bool enabled) noexcept -> void {
        auto fullscreen_atom = getXCBAtom(WM_STATE_FULLSCREEN_STR, false);
        if (!fullscreen_atom) [[unlikely]] {
            elog("Failed to setup fullscreen\n     > Failed to get atom {}\n        > reason: {}",
                 WM_STATE_STR,
                 fullscreen_atom.error().message);
            return;
        }

        auto state_atom = getXCBAtom(WM_STATE_FULLSCREEN_STR, false);
        if (!state_atom) [[unlikely]] {
            elog("Failed to setup fullscreen\n    > Failed to get atom {}\n        > reason: {}",
                 WM_STATE_STR,
                 state_atom.error().message);
            return;
        }

        auto ev           = xcb_client_message_event_t {};
        ev.response_type  = XCB_CLIENT_MESSAGE;
        ev.type           = *state_atom;
        ev.format         = 32;
        ev.window         = m_window;
        ev.data.data32[0] = enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
        ev.data.data32[1] = *fullscreen_atom;
        ev.data.data32[2] = XCB_ATOM_NONE;
        ev.data.data32[3] = 0;
        ev.data.data32[4] = 0;

        xcb_send_event(m_connection,
                       1,
                       m_window,
                       XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                       reinterpret_cast<const char *>(&ev));

        m_fullscreen = enabled;

        xcb_flush(m_connection);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::lockMouse() noexcept -> void {
        xcb_warp_pointer(m_connection,
                         XCB_NONE,
                         m_window,
                         0,
                         0,
                         0,
                         0,
                         m_extent.width / 2,
                         m_extent.height / 2);

        xcb_flush(m_connection);

        auto cookie =
            xcb_grab_pointer(m_connection,
                             1,
                             m_window,
                             XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                                 XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION,
                             XCB_GRAB_MODE_ASYNC,
                             XCB_GRAB_MODE_ASYNC,
                             m_window,
                             XCB_NONE,
                             XCB_CURRENT_TIME);
        xcb_grab_pointer_reply(m_connection, cookie, nullptr);
        xcb_flush(m_connection);

        m_mouse_locked = true;

        hideMouse();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::unlockMouse() noexcept -> void {
        xcb_ungrab_pointer(m_connection, XCB_CURRENT_TIME);
        xcb_flush(m_connection);

        m_mouse_locked = false;

        unhideMouse();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::hideMouse() noexcept -> void {
        xcb_xfixes_hide_cursor(m_connection, m_window);
        xcb_flush(m_connection);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::unhideMouse() noexcept -> void {
        xcb_xfixes_show_cursor(m_connection, m_window);
        xcb_flush(m_connection);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::nativeHandle() const noexcept -> stormkit::wsi::Window::NativeHandle {
        return reinterpret_cast<stormkit::wsi::Window::NativeHandle>(
            const_cast<Handles *>(&m_handles));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setKeyRepeatEnabled(bool enabled) noexcept -> void {
        auto mask = ((enabled) ? KEYBOARD_EVENTS : KEYBOARD_RAW_EVENTS) | MOUSE_RAW_EVENTS;

        xcb_input_xi_passive_grab_device(m_connection,
                                         XCB_CURRENT_TIME,
                                         m_window,
                                         XCB_CURSOR_NONE,
                                         0,
                                         XCB_INPUT_DEVICE_ALL,
                                         1,
                                         1,
                                         XCB_INPUT_GRAB_TYPE_KEYCODE,
                                         XCB_INPUT_GRAB_MODE_22_ASYNC,
                                         XCB_INPUT_GRAB_MODE_22_ASYNC,
                                         XCB_INPUT_GRAB_OWNER_NO_OWNER,
                                         &mask,
                                         &XINPUT_MASK_MODIFIERS);

        xcb_flush(m_connection);

        m_key_repeat_enabled = enabled;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setVirtualKeyboardVisible(bool visible) noexcept -> void {
        elog("x11::WindowImpl::setVirtualKeyboardVisible isn't yet implemented");
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setMousePosition(core::Position2i position) noexcept -> void {
        auto connection = getXCBConnection();

        xcb_warp_pointer(connection, XCB_NONE, m_window, 0, 0, 0, 0, position->x, position->y);

        xcb_flush(connection);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setMousePositionOnDesktop(core::Position2u position) noexcept -> void {
        auto connection = getXCBConnection();

        const auto default_screen_id = 0;
        auto root_window             = defaultRootWindow(connection, default_screen_id);

        xcb_warp_pointer(connection, XCB_NONE, root_window, 0, 0, 0, 0, position->x, position->y);

        xcb_flush(connection);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::getDesktopModes() -> std::vector<VideoSettings> {
        STORMKIT_RAII_CAPSULE(Reply, xcb_randr_get_screen_resources_current_reply_t, free)
        STORMKIT_RAII_CAPSULE(Output, xcb_randr_get_output_info_reply_t, free)
        STORMKIT_RAII_CAPSULE(CRTC, xcb_randr_get_crtc_info_reply_t, free)

        static auto video_settings = std::vector<VideoSettings> {}; // TODO Reimplement with RAII
        static auto init           = false;

        if (!init) {
            auto display = getXCBConnection();
            auto screen  = xcb_setup_roots_iterator(xcb_get_setup(display)).data;
            auto root    = screen->root;

            auto reply = ReplyScoped { xcb_randr_get_screen_resources_current_reply(
                display,
                xcb_randr_get_screen_resources_current(display, root),
                nullptr) };

            auto timestamp = reply->config_timestamp;

            auto len          = xcb_randr_get_screen_resources_current_outputs_length(reply.get());
            auto randr_output = xcb_randr_get_screen_resources_current_outputs(reply.get());

            video_settings.reserve(len);
            for (auto i = 0; i < len; ++i) {
                auto output = OutputScoped { xcb_randr_get_output_info_reply(
                    display,
                    xcb_randr_get_output_info(display, randr_output[i], timestamp),
                    nullptr) };
                if (output == nullptr) continue;

                auto crtc =
                    CRTCScoped { xcb_randr_get_crtc_info_reply(display,
                                                               xcb_randr_get_crtc_info(display,
                                                                                       output->crtc,
                                                                                       timestamp),
                                                               nullptr) };

                if (crtc == nullptr) continue;

                auto video_setting =
                    stormkit::wsi::VideoSettings { { crtc->width, crtc->height }, 32u };
                video_settings.emplace_back(std::move(video_setting));
            }

            init = true;
        }

        return video_settings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::getDesktopFullscreenSize() -> VideoSettings {
        static auto video_setting = stormkit::wsi::VideoSettings {};
        static auto init          = false;

        if (!init) {
            const auto modes = getDesktopModes();

            for (const auto &mode : modes) {
                video_setting.size.width  = std::max(video_setting.size.width, mode.size.width);
                video_setting.size.height = std::max(video_setting.size.height, mode.size.height);
                video_setting.size.depth  = std::max(video_setting.size.depth, mode.size.depth);
            }

            init = true;
        }

        return video_setting;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::processEvents(xcb_generic_event_t event) -> void {
        auto *xevent             = &event;
        const auto response_type = xevent->response_type & ~0x80;

        switch (response_type) {
            case XCB_KEY_PRESS: {
                auto kevent = reinterpret_cast<xcb_key_press_event_t *>(xevent);
                handleKeyEvent(kevent->detail, true);
                break;
            }
            case XCB_KEY_RELEASE: {
                auto kevent = reinterpret_cast<xcb_key_release_event_t *>(xevent);
                handleKeyEvent(kevent->detail, false);
                break;
            }
            case XCB_MOTION_NOTIFY: {
                auto mouse_event = reinterpret_cast<xcb_motion_notify_event_t *>(xevent);

                const auto x = mouse_event->event_x;
                const auto y = mouse_event->event_y;

                if (m_mouse_locked) {
                    if (x != m_extent.width / 2 || y != m_extent.height / 2) {
                        auto dx = x - m_mouse_position.x;
                        auto dy = y - m_mouse_position.y;

                        m_locked_mouse_position.x += dx;
                        m_locked_mouse_position.x += dy;

                        WindowImplBase::mouseMoveEvent(m_locked_mouse_position.x,
                                                       m_locked_mouse_position.y);
                    }
                    m_mouse_position.x = x;
                    m_mouse_position.y = y;
                } else {
                    m_mouse_position.x = x;
                    m_mouse_position.y = y;
                    WindowImplBase::mouseMoveEvent(m_mouse_position.x, m_mouse_position.y);
                }

                break;
            }
            case XCB_BUTTON_PRESS: {
                auto button_event = reinterpret_cast<xcb_button_press_event_t *>(xevent);

                auto button = button_event->detail;
                WindowImplBase::mouseDownEvent(x11MouseButtonToStormMouseButton(button),
                                               button_event->event_x,
                                               button_event->event_y);
                break;
            }
            case XCB_BUTTON_RELEASE: {
                auto button_event = reinterpret_cast<xcb_button_press_event_t *>(xevent);

                auto button = button_event->detail;
                WindowImplBase::mouseUpEvent(x11MouseButtonToStormMouseButton(button),
                                             button_event->event_x,
                                             button_event->event_y);
                break;
            }
            case XCB_CONFIGURE_NOTIFY: {
                auto configure_event = reinterpret_cast<xcb_configure_notify_event_t *>(xevent);

                if ((configure_event->width != m_extent.width) ||
                    (configure_event->height != m_extent.height)) {
                    m_extent.width  = configure_event->width;
                    m_extent.height = configure_event->height;
                    WindowImplBase::resizeEvent(configure_event->width, configure_event->height);
                }
                break;
            }
            case XCB_ENTER_NOTIFY: WindowImplBase::mouseEnteredEvent(); break;
            case XCB_LEAVE_NOTIFY: WindowImplBase::mouseExitedEvent(); break;
            case XCB_VISIBILITY_NOTIFY: // TODO XCB_VISIBILITY_NOTIFY
                /*if(xevent.xvisibility.wsi.== m_window
                        m_is_visible = xevent.xvisibility.state !=
                   VisibilityFullyObscured;*/
                break;
            case XCB_DESTROY_NOTIFY: // TODO XCB_DESTROY_NOTIFY
                break;
            case XCB_CLIENT_MESSAGE: {
                auto close_atom = getXCBAtom(WM_DELETE_WINDOW, false);
                if (!close_atom) [[unlikely]]
                    elog("Failed to get atom {}\n    > reason: {}",
                         WM_DELETE_WINDOW,
                         close_atom.error().message);

                if (reinterpret_cast<xcb_client_message_event_t *>(xevent)->data.data32[0] ==
                    *close_atom)
                    WindowImplBase::closeEvent();
                break;
            }
            case XCB_MAPPING_NOTIFY: {
                auto mapping_notify_event = reinterpret_cast<xcb_mapping_notify_event_t *>(xevent);

                if (mapping_notify_event->request != XCB_MAPPING_POINTER) {
                    xcb_refresh_keyboard_mapping(m_key_symbols.get(), mapping_notify_event);
                    updateKeymap();
                }
                break;
            }
            case XCB_GE_GENERIC: {
                if (isExtEvent(xevent, m_xi_opcode)) {
                    auto xievent = reinterpret_cast<xcb_ge_generic_event_t *>(xevent);
                    switch (xievent->event_type) {
                        case XCB_INPUT_KEY_PRESS: [[fallthrough]];
                        case XCB_INPUT_RAW_KEY_PRESS: {
                            auto xievent = reinterpret_cast<xcb_input_key_press_event_t *>(xevent);
                            handleKeyEvent(xievent->detail, true);
                            break;
                        }
                        case XCB_INPUT_KEY_RELEASE: [[fallthrough]];
                        case XCB_INPUT_RAW_KEY_RELEASE: {
                            auto xievent = reinterpret_cast<xcb_input_key_press_event_t *>(xevent);
                            handleKeyEvent(xievent->detail, false);
                            break;
                        }
                        case XCB_INPUT_BUTTON_PRESS: [[fallthrough]];
                        case XCB_INPUT_RAW_BUTTON_PRESS: {
                            auto button_event =
                                reinterpret_cast<xcb_input_button_press_event_t *>(xevent);

                            auto button = button_event->detail;
                            WindowImplBase::mouseDownEvent(x11MouseButtonToStormMouseButton(button),
                                                           button_event->event_x,
                                                           button_event->event_y);
                            break;
                        }
                        case XCB_INPUT_BUTTON_RELEASE: [[fallthrough]];
                        case XCB_INPUT_RAW_BUTTON_RELEASE: {
                            auto button_event =
                                reinterpret_cast<xcb_input_button_release_event_t *>(xevent);

                            auto button = button_event->detail;
                            WindowImplBase::mouseDownEvent(x11MouseButtonToStormMouseButton(button),
                                                           button_event->event_x,
                                                           button_event->event_y);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::updateKeymap() -> void {
        const auto device_id = xkb_x11_get_core_keyboard_device_id(m_connection);

        m_xkb_keymap.reset(xkb_x11_keymap_new_from_device(m_xkb_context,
                                                          m_connection,
                                                          device_id,
                                                          XKB_KEYMAP_COMPILE_NO_FLAGS));

        if (!m_xkb_keymap) {
            elog("Failed to compile a keymap");
            return;
        }

        m_xkb_state.reset(
            xkb_x11_state_new_from_device(m_xkb_keymap.get(), m_connection, device_id));

        if (!m_xkb_state) {
            elog("Failed to create XKB state");
            return;
        }

        m_handles.state = m_xkb_state.get();

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
    auto WindowImpl::handleKeyEvent(xcb_keycode_t keycode, bool down) noexcept -> void {
        auto symbol = xkb_keysym_t {};

        auto character = char {};
        symbol         = xkb_state_key_get_one_sym(m_xkb_state.get(), keycode);
        xkb_state_key_get_utf8(m_xkb_state.get(),
                               keycode,
                               reinterpret_cast<char *>(&character),
                               sizeof(char));

        auto key = xkbKeyToStormkitKey(symbol);

        if (down) WindowImplBase::keyDownEvent(key, character);
        else
            WindowImplBase::keyUpEvent(key, character);
    }
} // namespace stormkit::wsi::details::x11
