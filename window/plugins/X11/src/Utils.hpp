#pragma once

/////////// - StormKit::window - ///////////
#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb.h>

xcb_keysym_t stormkeyToX11Key(storm::window::Key key);
storm::window::Key x11keyToStormKey(xcb_keysym_t key);
storm::window::MouseButton x11MouseButtonToStormMouseButton(xcb_button_t button);

xcb_window_t defaultRootWindow(xcb_connection_t *connection, storm::core::Int32 screen_id);
