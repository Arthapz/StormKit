// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "X11Window.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include "X11Keyboard.hpp"
#include "X11Mouse.hpp"

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

#include <storm/window/VideoSettings.hpp>

extern "C" {
#include <xcb/randr.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xinput.h>

#define explicit _explicit
#include <xcb/xkb.h>
#undef explicit

#include <xkbcommon/xkbcommon-x11.h>
}

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
X11Window::X11Window() = default;

/////////////////////////////////////
/////////////////////////////////////
X11Window::~X11Window() {
    close();

    xcb_flush(m_connection.get());
}

/////////////////////////////////////
/////////////////////////////////////
X11Window::X11Window(std::string title, const VideoSettings &settings, WindowStyle style)
    : m_window { XCB_WINDOW_NONE }, m_key_symbols { nullptr }, m_is_open { false }, m_is_visible {
          false
      } {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
void X11Window::create(std::string title, const VideoSettings &settings, WindowStyle style) {
    static constexpr const auto EVENTS =
        XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW |
        XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_VISIBILITY_CHANGE |
        XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_EXPOSURE;

    m_connection.reset(xcb_connect(nullptr, nullptr));

    auto screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection.get())).data;
    m_window    = xcb_generate_id(m_connection.get());

    core::UInt32 value_list[] = { screen->white_pixel, screen->black_pixel, EVENTS };
    xcb_create_window(m_connection.get(),
                      XCB_COPY_FROM_PARENT,
                      m_window,
                      screen->root,
                      0,
                      0,
                      settings.size.width,
                      settings.size.height,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK,
                      value_list);

    m_extent = settings.size;

    // init key_symbol map, this is needed to extract the keysymbol from event
    m_key_symbols.reset(xcb_key_symbols_alloc(m_connection.get()));

    auto xkb_ext_reply = xcb_get_extension_data(m_connection.get(), &xcb_xkb_id);
    if (xkb_ext_reply) {
        auto cookie =
            xcb_xkb_use_extension(m_connection.get(), XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);
        auto reply = xcb_xkb_use_extension_reply(m_connection.get(), cookie, nullptr);

        if (!reply) elog("Coundn't enable XKB extension");

        if (!reply->supported) elog("The XKB extension is not supported on this X server");
        else {
            dlog("XKB initialized !");
            m_has_xkb = true;
        }

        if (reply) free(reply);
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
    xcb_xkb_select_events_checked(m_connection.get(),
                                  XCB_XKB_ID_USE_CORE_KBD,
                                  required_events,
                                  0,
                                  required_events,
                                  required_map_parts,
                                  required_map_parts,
                                  nullptr);

    // set the window name
    setTitle(std::move(title));

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
            size_hints.min_width = size_hints.max_width = settings.size.width;
            size_hints.min_height = size_hints.max_height = settings.size.height;
            xcb_icccm_set_wm_normal_hints(m_connection.get(), m_window, &size_hints);
        }
    }

    xcb_flush(m_connection.get());

    m_is_open    = true;
    m_is_visible = true;

    m_handles.connection  = m_connection.get();
    m_handles.window      = m_window;
    m_handles.key_symbols = m_key_symbols.get();

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
void X11Window::close() noexcept {
    m_is_open    = false;
    m_is_visible = false;

    if (m_protocol_reply) free(m_protocol_reply);
    if (m_close_reply) free(m_close_reply);
    if (m_window) xcb_destroy_window(m_connection.get(), m_window);

    m_protocol_reply = nullptr;
    m_close_reply    = nullptr;
    m_window         = XCB_WINDOW_NONE;
}

/////////////////////////////////////
/////////////////////////////////////
bool X11Window::pollEvent(Event &event) noexcept {
    xcb_generic_event_t *xevent;
    if (xevent = xcb_poll_for_event(m_connection.get()); !xevent) return false;

    processEvents(*xevent);

    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
bool X11Window::waitEvent(Event &event) noexcept {
    xcb_generic_event_t *xevent;
    if (xevent = xcb_poll_for_event(m_connection.get()); !xevent) return false;

    processEvents(*xevent);

    return AbstractWindow::waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
void X11Window::setTitle(std::string title) noexcept {
    xcb_change_property(m_connection.get(),
                        XCB_PROP_MODE_REPLACE,
                        m_window,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        static_cast<core::UInt32>(title.length()),
                        reinterpret_cast<const char *>(title.c_str()));

    xcb_flush(m_connection.get());

    m_title = std::move(title);
}

/////////////////////////////////////
/////////////////////////////////////
void X11Window::setVideoSettings(const storm::window::VideoSettings &settings) noexcept {
    dlog("setVideoSettings not yet implemented");
}

/////////////////////////////////////
/////////////////////////////////////
const storm::core::Extentu &X11Window::size() const noexcept {
    return m_extent;
}

/////////////////////////////////////
/////////////////////////////////////
bool X11Window::isOpen() const noexcept {
    return m_is_open;
}

/////////////////////////////////////
/////////////////////////////////////
bool X11Window::isVisible() const noexcept {
    return m_is_visible;
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::NativeHandle X11Window::nativeHandle() const noexcept {
    return reinterpret_cast<storm::window::NativeHandle>(const_cast<Handles *>(&m_handles));
}

/////////////////////////////////////
/////////////////////////////////////
void X11Window::processEvents(xcb_generic_event_t event) {
    auto *xevent             = &event;
    const auto response_type = xevent->response_type & ~0x80;

    switch (response_type) {
        case XCB_KEY_PRESS: {
            auto keyboard_event = reinterpret_cast<xcb_key_press_event_t *>(xevent);
            auto symbol         = xcb_keysym_t {};

            auto character = char {};
            if (m_has_xkb) {
                symbol = xkb_state_key_get_one_sym(m_xkb_state.get(), keyboard_event->detail);
                xkb_state_key_get_utf8(m_xkb_state.get(),
                                       keyboard_event->detail,
                                       reinterpret_cast<char *>(&character),
                                       sizeof(char));
            } else {
                symbol    = xcb_key_press_lookup_keysym(m_key_symbols.get(), keyboard_event, 0);
                character = x11KeyToChar(symbol);
            }

            auto key = x11keyToStormKey(symbol);

            AbstractWindow::keyDownEvent(key, character);
            break;
        }
        case XCB_KEY_RELEASE: {
            auto keyboard_event = reinterpret_cast<xcb_key_release_event_t *>(xevent);
            auto symbol         = xcb_keysym_t {};

            auto character = char {};
            if (m_has_xkb) {
                symbol = xkb_state_key_get_one_sym(m_xkb_state.get(), keyboard_event->detail);
                xkb_state_key_get_utf8(m_xkb_state.get(),
                                       keyboard_event->detail,
                                       reinterpret_cast<char *>(&character),
                                       sizeof(char));
            } else {
                symbol    = xcb_key_release_lookup_keysym(m_key_symbols.get(), keyboard_event, 0);
                character = x11KeyToChar(symbol);
            }

            auto key = x11keyToStormKey(symbol);

            AbstractWindow::keyUpEvent(key, character);
            break;
        }
        case XCB_MOTION_NOTIFY: {
            auto mouse_event = reinterpret_cast<xcb_motion_notify_event_t *>(xevent);
            if (m_mouse_x == mouse_event->event_x && m_mouse_y == mouse_event->event_y) { break; }

            m_mouse_x = mouse_event->event_x;
            m_mouse_y = mouse_event->event_y;
            AbstractWindow::mouseMoveEvent(mouse_event->event_x, mouse_event->event_y);
            break;
        }
        case XCB_BUTTON_PRESS: {
            auto button_event = reinterpret_cast<xcb_button_press_event_t *>(xevent);

            auto button = button_event->detail;
            AbstractWindow::mouseDownEvent(x11MouseButtonToStormMouseButton(button),
                                           button_event->event_x,
                                           button_event->event_y);
            break;
        }
        case XCB_BUTTON_RELEASE: {
            auto button_event = reinterpret_cast<xcb_button_press_event_t *>(xevent);

            auto button = button_event->detail;
            AbstractWindow::mouseUpEvent(x11MouseButtonToStormMouseButton(button),
                                         button_event->event_x,
                                         button_event->event_y);
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            auto configure_event = reinterpret_cast<xcb_configure_notify_event_t *>(xevent);

            if ((configure_event->width != m_video_settings.size.width) ||
                (configure_event->height != m_video_settings.size.height)) {
                m_extent.width  = configure_event->width;
                m_extent.height = configure_event->height;
                AbstractWindow::resizeEvent(configure_event->width, configure_event->height);
            }
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
            if (reinterpret_cast<xcb_client_message_event_t *>(xevent)->data.data32[0] ==
                m_close_reply->atom)
                AbstractWindow::closeEvent();
            break;
        case XCB_MAPPING_NOTIFY: {
            auto mapping_notify_event = reinterpret_cast<xcb_mapping_notify_event_t *>(xevent);

            if (!m_has_xkb && mapping_notify_event->request != XCB_MAPPING_POINTER) {
                xcb_refresh_keyboard_mapping(m_key_symbols.get(), mapping_notify_event);
                updateKeymap();
            }
            break;
        }
    }
}

void X11Window::updateKeymap() {
    if (!m_xkb_context) m_xkb_context.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));

    if (!m_xkb_context) {
        elog("Failed to create XKB context");

        return;
    }

    if (m_has_xkb) {
        const auto device_id = xkb_x11_get_core_keyboard_device_id(m_connection.get());

        m_xkb_keymap.reset(xkb_x11_keymap_new_from_device(m_xkb_context.get(),
                                                          m_connection.get(),
                                                          device_id,
                                                          XKB_KEYMAP_COMPILE_NO_FLAGS));

        if (!m_xkb_keymap) {
            elog("Failed to compile a keymap");
            return;
        }

        m_xkb_state.reset(
            xkb_x11_state_new_from_device(m_xkb_keymap.get(), m_connection.get(), device_id));

        if (!m_xkb_state) {
            elog("Failed to create XKB state");
            return;
        }
    }

    m_handles.state = m_xkb_state.get();

    updateXKBMods();
}

void X11Window::updateXKBMods() {
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

std::vector<VideoSettings> X11Window::getDesktopModes() {
    static auto video_settings = std::vector<VideoSettings> {};
    static auto init           = false;

    if (!init) {
        auto display = xcb_connect(nullptr, nullptr);
        auto screen  = xcb_setup_roots_iterator(xcb_get_setup(display)).data;
        auto root    = screen->root;

        auto reply = xcb_randr_get_screen_resources_current_reply(
            display,
            xcb_randr_get_screen_resources_current(display, root),
            nullptr);

        auto timestamp = reply->config_timestamp;

        auto len          = xcb_randr_get_screen_resources_current_outputs_length(reply);
        auto randr_output = xcb_randr_get_screen_resources_current_outputs(reply);

        video_settings.reserve(len);
        for (auto i = 0; i < len; ++i) {
            auto output = xcb_randr_get_output_info_reply(display,
                                                          xcb_randr_get_output_info(display,
                                                                                    randr_output[i],
                                                                                    timestamp),
                                                          nullptr);
            if (output == nullptr) continue;

            auto crtc = xcb_randr_get_crtc_info_reply(display,
                                                      xcb_randr_get_crtc_info(display,
                                                                              output->crtc,
                                                                              timestamp),
                                                      nullptr);
            if (crtc == nullptr) continue;

            auto video_setting =
                storm::window::VideoSettings { { crtc->width, crtc->height }, 32u };
            video_settings.emplace_back(std::move(video_setting));

            // free
        }

        xcb_disconnect(display);
        init = true;
    }

    return video_settings;
}

VideoSettings X11Window::getDesktopFullscreenSize() {
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
