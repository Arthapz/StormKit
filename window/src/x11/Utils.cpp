// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Utils.hpp"

#include <X11/keysym.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

/////////////////////////////////////
/////////////////////////////////////
storm::window::Key x11keyToStormKey(xcb_keysym_t key) {
    using namespace storm::window;

    switch (key) {
        case XK_a: return Key::A;
        case XK_b: return Key::B;
        case XK_c: return Key::C;
        case XK_d: return Key::D;
        case XK_e: return Key::E;
        case XK_f: return Key::F;
        case XK_g: return Key::G;
        case XK_h: return Key::H;
        case XK_i: return Key::I;
        case XK_j: return Key::J;
        case XK_k: return Key::K;
        case XK_l: return Key::L;
        case XK_m: return Key::M;
        case XK_n: return Key::N;
        case XK_o: return Key::O;
        case XK_p: return Key::P;
        case XK_q: return Key::Q;
        case XK_r: return Key::R;
        case XK_s: return Key::S;
        case XK_t: return Key::T;
        case XK_u: return Key::U;
        case XK_v: return Key::V;
        case XK_w: return Key::W;
        case XK_x: return Key::X;
        case XK_y: return Key::Y;
        case XK_z: return Key::Z;
        case XK_0: return Key::NUM0;
        case XK_1: return Key::NUM1;
        case XK_2: return Key::NUM2;
        case XK_3: return Key::NUM3;
        case XK_4: return Key::NUM4;
        case XK_5: return Key::NUM5;
        case XK_6: return Key::NUM6;
        case XK_7: return Key::NUM7;
        case XK_8: return Key::NUM8;
        case XK_9: return Key::NUM9;
        case XK_Escape: return Key::ESCAPE;
        case XK_Control_L: return Key::LCONTROL;
        case XK_Shift_L: return Key::LSHIFT;
        case XK_Alt_L: return Key::LALT;
        case XK_Super_L: return Key::LSYSTEM;
        case XK_Control_R: return Key::RCONTROL;
        case XK_Shift_R: return Key::RSHIFT;
        case XK_Alt_R: return Key::RALT;
        case XK_Super_R: return Key::RSYSTEM;
        case XK_Menu: return Key::MENU;
        case XK_bracketleft: return Key::LBRACKET;
        case XK_bracketright: return Key::RBRACKET;
        case XK_semicolon: return Key::SEMICOLON;
        case XK_comma: return Key::COMMA;
        case XK_period: return Key::PERIOD;
        case XK_quoteright: [[fallthrough]];
        case XK_quoteleft: return Key::QUOTE;
        case XK_slash: return Key::SLASH;
        case XK_backslash: return Key::BACKSLASH;
        case XK_dead_grave: return Key::TILDE;
        case XK_equal: return Key::EQUAL;
        case XK_hyphen: return Key::HYPHEN;
        case XK_space: return Key::SPACE;
        case XK_Return: return Key::ENTER;
        case XK_BackSpace: return Key::BACKSPACE;
        case XK_Tab: return Key::TAB;
        case XK_Page_Up: return Key::PAGEUP;
        case XK_Page_Down: return Key::PAGEDOWN;
        case XK_Begin: return Key::BEGIN;
        case XK_End: return Key::END;
        case XK_Home: return Key::HOME;
        case XK_Insert: return Key::INSERT;
        case XK_Delete: return Key::DELETE;
        case XK_KP_Add: return Key::ADD;
        case XK_KP_Subtract: return Key::SUBSTRACT;
        case XK_KP_Multiply: return Key::MULTIPLY;
        case XK_KP_Divide: return Key::DIVIDE;
        case XK_Left: return Key::LEFT;
        case XK_Right: return Key::RIGHT;
        case XK_Up: return Key::UP;
        case XK_Down: return Key::DOWN;
        case XK_KP_0: return Key::NUMPAD0;
        case XK_KP_1: return Key::NUMPAD1;
        case XK_KP_2: return Key::NUMPAD2;
        case XK_KP_3: return Key::NUMPAD3;
        case XK_KP_4: return Key::NUMPAD4;
        case XK_KP_5: return Key::NUMPAD5;
        case XK_KP_6: return Key::NUMPAD6;
        case XK_KP_7: return Key::NUMPAD7;
        case XK_KP_8: return Key::NUMPAD8;
        case XK_KP_9: return Key::NUMPAD9;
        case XK_F1: return Key::F1;
        case XK_F2: return Key::F2;
        case XK_F3: return Key::F3;
        case XK_F4: return Key::F4;
        case XK_F5: return Key::F5;
        case XK_F6: return Key::F6;
        case XK_F7: return Key::F7;
        case XK_F8: return Key::F8;
        case XK_F9: return Key::F9;
        case XK_F10: return Key::F10;
        case XK_F11: return Key::F11;
        case XK_F12: return Key::F12;
        case XK_F13: return Key::F13;
        case XK_F14: return Key::F14;
        case XK_F15: return Key::F15;
        case XK_Pause: return Key::PAUSE;
    }

    return Key::UNKNOW;
}

/////////////////////////////////////
/////////////////////////////////////
xcb_keysym_t stormkeyToX11Key(storm::window::Key key) {
    using namespace storm::window;

    switch (key) {
        case Key::A: return XK_a;
        case Key::B: return XK_b;
        case Key::C: return XK_c;
        case Key::D: return XK_d;
        case Key::E: return XK_e;
        case Key::F: return XK_f;
        case Key::G: return XK_g;
        case Key::H: return XK_h;
        case Key::I: return XK_i;
        case Key::J: return XK_j;
        case Key::K: return XK_k;
        case Key::L: return XK_l;
        case Key::M: return XK_m;
        case Key::N: return XK_n;
        case Key::O: return XK_o;
        case Key::P: return XK_p;
        case Key::Q: return XK_q;
        case Key::R: return XK_r;
        case Key::S: return XK_s;
        case Key::T: return XK_t;
        case Key::U: return XK_u;
        case Key::V: return XK_v;
        case Key::W: return XK_w;
        case Key::X: return XK_x;
        case Key::Y: return XK_y;
        case Key::Z: return XK_z;
        case Key::NUM0: return XK_0;
        case Key::NUM1: return XK_1;
        case Key::NUM2: return XK_2;
        case Key::NUM3: return XK_3;
        case Key::NUM4: return XK_4;
        case Key::NUM5: return XK_5;
        case Key::NUM6: return XK_6;
        case Key::NUM7: return XK_7;
        case Key::NUM8: return XK_8;
        case Key::NUM9: return XK_9;
        case Key::ESCAPE: return XK_Escape;
        case Key::LCONTROL: return XK_Control_L;
        case Key::LSHIFT: return XK_Shift_L;
        case Key::LALT: return XK_Alt_L;
        case Key::LSYSTEM: return XK_Super_L;
        case Key::RCONTROL: return XK_Control_R;
        case Key::RSHIFT: return XK_Shift_R;
        case Key::RALT: return XK_Alt_R;
        case Key::RSYSTEM: return XK_Super_R;
        case Key::MENU: return XK_Menu;
        case Key::LBRACKET: return XK_bracketleft;
        case Key::RBRACKET: return XK_bracketright;
        case Key::SEMICOLON: return XK_semicolon;
        case Key::COMMA: return XK_comma;
        case Key::PERIOD: return XK_period;
        case Key::QUOTE: return XK_quoteleft;
        case Key::SLASH: return XK_slash;
        case Key::BACKSLASH: return XK_backslash;
        case Key::TILDE: return XK_dead_grave;
        case Key::EQUAL: return XK_equal;
        case Key::HYPHEN: return XK_hyphen;
        case Key::SPACE: return XK_space;
        case Key::ENTER: return XK_Return;
        case Key::BACKSPACE: return XK_BackSpace;
        case Key::TAB: return XK_Tab;
        case Key::PAGEUP: return XK_Page_Up;
        case Key::PAGEDOWN: return XK_Page_Down;
        case Key::BEGIN: return XK_Begin;
        case Key::END: return XK_End;
        case Key::HOME: return XK_Home;
        case Key::INSERT: return XK_Insert;
        case Key::DELETE: return XK_Delete;
        case Key::ADD: return XK_KP_Add;
        case Key::SUBSTRACT: return XK_KP_Subtract;
        case Key::MULTIPLY: return XK_KP_Multiply;
        case Key::DIVIDE: return XK_KP_Divide;
        case Key::LEFT: return XK_Left;
        case Key::RIGHT: return XK_Right;
        case Key::UP: return XK_Up;
        case Key::DOWN: return XK_Down;
        case Key::NUMPAD0: return XK_KP_0;
        case Key::NUMPAD1: return XK_KP_1;
        case Key::NUMPAD2: return XK_KP_2;
        case Key::NUMPAD3: return XK_KP_3;
        case Key::NUMPAD4: return XK_KP_4;
        case Key::NUMPAD5: return XK_KP_5;
        case Key::NUMPAD6: return XK_KP_6;
        case Key::NUMPAD7: return XK_KP_7;
        case Key::NUMPAD8: return XK_KP_8;
        case Key::NUMPAD9: return XK_KP_9;
        case Key::F1: return XK_F1;
        case Key::F2: return XK_F2;
        case Key::F3: return XK_F3;
        case Key::F4: return XK_F4;
        case Key::F5: return XK_F5;
        case Key::F6: return XK_F6;
        case Key::F7: return XK_F7;
        case Key::F8: return XK_F8;
        case Key::F9: return XK_F9;
        case Key::F10: return XK_F10;
        case Key::F11: return XK_F11;
        case Key::F12: return XK_F12;
        case Key::F13: return XK_F13;
        case Key::F14: return XK_F14;
        case Key::F15: return XK_F15;
        case Key::PAUSE: return XK_Pause;
        case Key::UNKNOW: return {};
    }

    return {};
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::MouseButton x11MouseButtonToStormMouseButton(xcb_button_t button) {
    switch (button) {
        case XCB_BUTTON_INDEX_1: return storm::window::MouseButton::LEFT;
        case XCB_BUTTON_INDEX_2: return storm::window::MouseButton::MIDDLE;
        case XCB_BUTTON_INDEX_3: return storm::window::MouseButton::RIGHT;
        case XCB_BUTTON_INDEX_4: return storm::window::MouseButton::BUTTON1;
        case XCB_BUTTON_INDEX_5: return storm::window::MouseButton::BUTTON2;
    }

    return storm::window::MouseButton::UNKNOW;
}

xcb_screen_t *screen_of_display(xcb_connection_t *c, int screen) {
    auto iter = xcb_screen_iterator_t {};

    for (iter = xcb_setup_roots_iterator(xcb_get_setup(c)); iter.rem;
         --screen, xcb_screen_next(&iter))
        if (screen == 0) return iter.data;

    return nullptr;
}

xcb_window_t defaultRootWindow(xcb_connection_t *connection, int32_t screen_id) {
    auto screen = screen_of_display(connection, screen_id);

    return screen->root;
}
