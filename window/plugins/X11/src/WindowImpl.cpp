// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WindowImpl.hpp"
#include "Utils.hpp"

#include <cstddef>
#include <stdexcept>

#include <storm/window/VideoSettings.hpp>

#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
WindowImpl::WindowImpl() = default;

/////////////////////////////////////
/////////////////////////////////////
WindowImpl::~WindowImpl() {
    if (m_key_symbols) xcb_key_symbols_free(m_key_symbols);

    if (m_window) {
        free(m_protocol_reply);
        free(m_close_reply);

        xcb_destroy_window(m_connection.get(), m_window);
        xcb_flush(m_connection.get());
    }
}

/////////////////////////////////////
/////////////////////////////////////
WindowImpl::WindowImpl(const std::string &title, const VideoSettings &settings, WindowStyle style)
    : m_window(0), m_key_symbols(nullptr), m_is_open(false), m_is_visible(false) {
    create(title, settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::create(const std::string &title,
                        const VideoSettings &settings,
                        WindowStyle style) {
    static constexpr const auto EVENTS =
        XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
        XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_VISIBILITY_CHANGE |
        XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_EXPOSURE;

    m_connection.reset(xcb_connect(nullptr, nullptr));

    auto screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection.get())).data;
    m_window    = xcb_generate_id(m_connection.get());

    core::UInt32 value_list[] = { screen->black_pixel, screen->black_pixel, EVENTS };
    xcb_create_window(m_connection.get(),
                      XCB_COPY_FROM_PARENT,
                      m_window,
                      screen->root,
                      0,
                      0,
                      settings.size.w,
                      settings.size.h,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK,
                      value_list);

    // init key_symbol map, this is needed to extract the keysymbol from event
    m_key_symbols = xcb_key_symbols_alloc(m_connection.get());

    // set the window name
    setTitle(title);

    // set the window's style
    if ((style & WindowStyle::Fullscreen) == WindowStyle::Fullscreen) {
        auto wm_state       = xcb_intern_atom(m_connection.get(), 0, 13, "_NET_WM_STATE");
        auto wm_state_reply = xcb_intern_atom_reply(m_connection.get(), wm_state, nullptr);
        auto wm_state_fs = xcb_intern_atom(m_connection.get(), 0, 24, "_NET_WM_STATE_FULLSCREEN");
        auto wm_state_fs_reply = xcb_intern_atom_reply(m_connection.get(), wm_state_fs, nullptr);

        xcb_change_property(m_connection.get(),
                            XCB_PROP_MODE_REPLACE,
                            m_window,
                            wm_state_reply->atom,
                            XCB_ATOM_ATOM,
                            32,
                            1,
                            &wm_state_fs_reply->atom);

        free(wm_state_reply);
        free(wm_state_fs_reply);
    } else {
        // checking if the window manager support window decoration
        static const auto window_manager_hints_str = std::string_view("_MOTIF_WM_HINTS");

        const auto atom_request = xcb_intern_atom(m_connection.get(),
                                                  0,
                                                  window_manager_hints_str.length(),
                                                  window_manager_hints_str.data());
        const auto atom_reply   = xcb_intern_atom_reply(m_connection.get(), atom_request, nullptr);

        if (atom_reply && (style & WindowStyle::Fullscreen) != WindowStyle::Fullscreen) {
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

            struct {
                uint32_t flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
                uint32_t functions   = 0;
                uint32_t decorations = 0;
                int32_t input_mode   = 0;
                uint32_t state       = 0;
            } hints;

            if ((style & WindowStyle::TitleBar) == WindowStyle::TitleBar) {
                hints.decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU;
                hints.functions |= MWM_FUNC_MOVE;
            }

            if ((style & WindowStyle::Close) == WindowStyle::Minimizable) {
                hints.decorations |= 0;
                hints.functions |= MWM_FUNC_CLOSE;
            }

            if ((style & WindowStyle::Minimizable) == WindowStyle::Minimizable) {
                hints.decorations |= MWM_DECOR_MINIMIZE;
                hints.functions |= MWM_FUNC_MINIMIZE;
            }

            if ((style & WindowStyle::Resizable) == WindowStyle::Resizable) {
                hints.decorations |= MWM_DECOR_RESIZE | MWM_DECOR_MAXIMIZE;
                hints.functions |= MWM_FUNC_RESIZE | MWM_FUNC_MAXIMIZE;
            }

            // applying hints
            xcb_change_property(m_connection.get(),
                                XCB_PROP_MODE_REPLACE,
                                m_window,
                                atom_reply->atom,
                                XCB_ATOM_WM_HINTS,
                                32,
                                5,
                                reinterpret_cast<std::byte *>(&hints));

            free(atom_reply);
        }

        // hack to force some windows managers to disable resizing
        if ((style & WindowStyle::Resizable) != WindowStyle::Resizable) {
            auto size_hints      = xcb_size_hints_t {};
            size_hints.flags     = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE;
            size_hints.min_width = size_hints.max_width = settings.size.w;
            size_hints.min_height = size_hints.max_height = settings.size.h;
            xcb_icccm_set_wm_normal_hints(m_connection.get(), m_window, &size_hints);
        }
    }

    xcb_flush(m_connection.get());

    m_is_open    = true;
    m_is_visible = true;

    handles.window     = m_window;
    handles.connection = m_connection.get();

    m_protocol_cookie = xcb_intern_atom(m_connection.get(), 1, 12, "WM_PROTOCOLS");
    m_protocol_reply  = xcb_intern_atom_reply(m_connection.get(), m_protocol_cookie, nullptr);

    m_close_cookie   = xcb_intern_atom(m_connection.get(), 0, 16, "WM_DELETE_WINDOW");
    m_close_reply    = xcb_intern_atom_reply(m_connection.get(), m_close_cookie, nullptr);
    m_video_settings = settings;

    xcb_change_property(m_connection.get(),
                        XCB_PROP_MODE_REPLACE,
                        m_window,
                        m_protocol_reply->atom,
                        4,
                        32,
                        1,
                        &m_close_reply->atom);

    xcb_map_window(m_connection.get(), m_window);

    xcb_flush(m_connection.get());
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::close() noexcept {
    m_is_open    = false;
    m_is_visible = false;
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::pollEvent(Event &event, void *native_event) noexcept {
    xcb_generic_event_t *xevent;
    if (xevent = xcb_poll_for_event(m_connection.get()); !xevent) return false;

    processEvents(*xevent, native_event);

    return AbstractWindow::pollEvent(event, native_event);
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::waitEvent(Event &event, void *native_event) noexcept {
    xcb_generic_event_t *xevent;
    if (xevent = xcb_poll_for_event(m_connection.get()); !xevent) return false;

    processEvents(*xevent, native_event);

    return AbstractWindow::waitEvent(event, native_event);
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::setTitle(const std::string &title) noexcept {
    xcb_change_property(m_connection.get(),
                        XCB_PROP_MODE_REPLACE,
                        m_window,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        static_cast<core::UInt32>(title.length()),
                        title.c_str());

    xcb_flush(m_connection.get());
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::setVideoSettings(const storm::window::VideoSettings &settings) noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
storm::core::Extentu WindowImpl::size() const noexcept {
    const auto reply = xcb_get_geometry_reply(m_connection.get(),
                                              xcb_get_geometry(m_connection.get(), m_window),
                                              nullptr);

    auto result = core::Extentu { reply->width, reply->height };
    free(reply);

    return result;
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::isOpen() const noexcept {
    return m_is_open;
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::isVisible() const noexcept {
    return m_is_visible;
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::NativeHandle WindowImpl::nativeHandle() const noexcept {
    return reinterpret_cast<storm::window::NativeHandle>(const_cast<Handles *>(&handles));
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::processEvents(xcb_generic_event_t xevent, void *native_event) {
    switch (xevent.response_type & ~0x80) {
        case XCB_KEY_PRESS: {
            auto keyboard_event = reinterpret_cast<xcb_key_press_event_t *>(&xevent);

            auto symbol = xcb_key_press_lookup_keysym(m_key_symbols, keyboard_event, 0);

            auto key = x11keyToStormKey(symbol);

            AbstractWindow::keyDownEvent(key);
            break;
        }
        case XCB_KEY_RELEASE: {
            auto keyboard_event = reinterpret_cast<xcb_key_release_event_t *>(&xevent);

            auto symbol = xcb_key_release_lookup_keysym(m_key_symbols, keyboard_event, 0);

            auto key = x11keyToStormKey(symbol);

            AbstractWindow::keyUpEvent(key);
            break;
        }
        case XCB_MOTION_NOTIFY: {
            auto mouse_event = reinterpret_cast<xcb_motion_notify_event_t *>(&xevent);
            if (m_mouse_x == mouse_event->event_x && m_mouse_y == mouse_event->event_y) {
                break;
            }

            m_mouse_x = mouse_event->event_x;
            m_mouse_y = mouse_event->event_y;
            AbstractWindow::mouseMoveEvent(mouse_event->event_x, mouse_event->event_y);
            break;
        }
        case XCB_BUTTON_PRESS: {
            auto button_event = reinterpret_cast<xcb_button_press_event_t *>(&xevent);

            auto button = button_event->detail;
            AbstractWindow::mouseDownEvent(x11MouseButtonToStormMouseButton(button),
                                           button_event->event_x,
                                           button_event->event_y);
            break;
        }
        case XCB_BUTTON_RELEASE: {
            auto button_event = reinterpret_cast<xcb_button_press_event_t *>(&xevent);

            auto button = button_event->detail;
            AbstractWindow::mouseUpEvent(x11MouseButtonToStormMouseButton(button),
                                         button_event->event_x,
                                         button_event->event_y);
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            auto configure_event = reinterpret_cast<xcb_configure_notify_event_t *>(&xevent);

            if ((configure_event->width != m_video_settings.size.w) ||
                (configure_event->height != m_video_settings.size.h))
                AbstractWindow::resizeEvent(configure_event->width, configure_event->height);

            break;
        }
        case XCB_ENTER_NOTIFY: AbstractWindow::mouseEnteredEvent(); break;
        case XCB_LEAVE_NOTIFY: AbstractWindow::mouseExitedEvent(); break;
        case XCB_VISIBILITY_NOTIFY:
            /*if(xevent.xvisibility.window == m_window)
                    m_is_visible = xevent.xvisibility.state !=
               VisibilityFullyObscured;*/
            break;
        case XCB_DESTROY_NOTIFY:

            break;
            // TODO intercept close resize minimize maximize events
        case XCB_CLIENT_MESSAGE:
            if (reinterpret_cast<xcb_client_message_event_t *>(&xevent)->data.data32[0] ==
                m_close_reply->atom)
                AbstractWindow::closeEvent();
            break;
    }

    if (native_event) *reinterpret_cast<xcb_generic_event_t *>(native_event) = xevent;
}
