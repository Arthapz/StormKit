#include "InputHandlerImpl.hpp"
#include "Utils.hpp"

/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

using namespace storm;
using namespace storm::window;

struct Handles {
    xcb_connection_t *connection;
    xcb_window_t window;
};

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl(const Window &window) : AbstractInputHandler { window } {
}

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::~InputHandlerImpl() = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl(InputHandlerImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl &InputHandlerImpl::operator=(InputHandlerImpl &&) = default;

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isKeyPressed(Key key) const noexcept {
    const auto key_sym = stormkeyToX11Key(key);

    auto default_screen_id = 0;
    auto connection        = xcb_connect(nullptr, &default_screen_id);
    auto hot_keys_symbols  = xcb_key_symbols_alloc(connection);
    const auto key_loc     = xcb_key_symbols_get_keycode(hot_keys_symbols, key_sym);

    auto is_pressed = false;

    auto cookie = xcb_query_keymap(connection);
    auto reply  = xcb_query_keymap_reply(connection, cookie, nullptr);

    is_pressed = ((reply->keys[*key_loc / 8] & (1 << (*key_loc % 8))) != 0);

    xcb_key_symbols_free(hot_keys_symbols);
    xcb_disconnect(connection);

    return is_pressed;
}

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isMouseButtonPressed(MouseButton button) const noexcept {
    auto default_screen_id = 0;
    auto connection        = xcb_connect(nullptr, &default_screen_id);
    auto root_window       = defaultRootWindow(connection, default_screen_id);

    auto cookie = xcb_query_pointer(connection, root_window);
    auto reply  = xcb_query_pointer_reply(connection, cookie, nullptr);

    auto is_pressed = false;
    switch (button) {
        case MouseButton::Left: is_pressed = (reply->mask & XCB_BUTTON_MASK_1); break;
        case MouseButton::Middle: is_pressed = (reply->mask & XCB_BUTTON_MASK_2); break;
        case MouseButton::Right: is_pressed = (reply->mask & XCB_BUTTON_MASK_3); break;
        case MouseButton::Button1: is_pressed = (reply->mask & XCB_BUTTON_MASK_4); break;
        case MouseButton::Button2: is_pressed = (reply->mask & XCB_BUTTON_MASK_5); break;
        case MouseButton::Unknow: is_pressed = false; break;
    }

    xcb_disconnect(connection);

    return is_pressed;
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePositionOnDesktop(core::Position2u position) noexcept {
    auto default_screen_id = 0;
    auto connection        = xcb_connect(nullptr, &default_screen_id);
    auto root_window       = defaultRootWindow(connection, default_screen_id);

    xcb_warp_pointer(connection, 0, root_window, 0, 0, 0, 0, position->x, position->y);

    xcb_flush(connection);

    xcb_disconnect(connection);
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandlerImpl::getMousePositionOnDesktop() const noexcept {
    auto default_screen_id = 0;
    auto connection        = xcb_connect(nullptr, &default_screen_id);
    auto root_window       = defaultRootWindow(connection, default_screen_id);

    auto cookie = xcb_query_pointer(connection, root_window);
    auto reply  = xcb_query_pointer_reply(connection, cookie, nullptr);

    const auto position = core::Vector2u { reply->win_x, reply->win_y };

    xcb_disconnect(connection);

    return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePositionOnWindow(core::Position2i position) noexcept {
    auto native_handle = static_cast<Handles *>(m_window->nativeHandle());

    xcb_warp_pointer(native_handle->connection,
                     0,
                     native_handle->window,
                     0,
                     0,
                     0,
                     0,
                     position->x,
                     position->y);

    xcb_flush(native_handle->connection);
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2i InputHandlerImpl::getMousePositionOnWindow() const noexcept {
    auto native_handle = static_cast<Handles *>(m_window->nativeHandle());

    auto cookie = xcb_query_pointer(native_handle->connection, native_handle->window);
    auto reply  = xcb_query_pointer_reply(native_handle->connection, cookie, nullptr);

    const auto position = core::Vector2i { reply->win_x, reply->win_y };

    return core::makeNamed<core::Position2i>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept {
    // not supported
}