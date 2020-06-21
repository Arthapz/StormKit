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
        case XK_0: return Key::Num0;
        case XK_1: return Key::Num1;
        case XK_2: return Key::Num2;
        case XK_3: return Key::Num3;
        case XK_4: return Key::Num4;
        case XK_5: return Key::Num5;
        case XK_6: return Key::Num6;
        case XK_7: return Key::Num7;
        case XK_8: return Key::Num8;
        case XK_9: return Key::Num9;
        case XK_Escape: return Key::Escape;
        case XK_Control_L: return Key::L_Control;
        case XK_Shift_L: return Key::L_Shift;
        case XK_Alt_L: return Key::L_Alt;
        case XK_Super_L: return Key::L_System;
        case XK_Control_R: return Key::R_Control;
        case XK_Shift_R: return Key::R_Shift;
        case XK_Alt_R: return Key::R_Alt;
        case XK_Super_R: return Key::R_System;
        case XK_Menu: return Key::Menu;
        case XK_bracketleft: return Key::L_Bracket;
        case XK_bracketright: return Key::R_Bracket;
        case XK_semicolon: return Key::Semi_Colon;
        case XK_comma: return Key::Comma;
        case XK_period: return Key::Period;
        case XK_quoteright: [[fallthrough]];
        case XK_quoteleft: return Key::Quote;
        case XK_slash: return Key::Slash;
        case XK_backslash: return Key::Back_Slash;
        case XK_dead_grave: return Key::Tilde;
        case XK_equal: return Key::Equal;
        case XK_hyphen: return Key::Hyphen;
        case XK_space: return Key::Space;
        case XK_Return: return Key::Enter;
        case XK_BackSpace: return Key::Back_Space;
        case XK_Tab: return Key::Tab;
        case XK_Page_Up: return Key::Page_Up;
        case XK_Page_Down: return Key::Page_Down;
        case XK_Begin: return Key::Begin;
        case XK_End: return Key::End;
        case XK_Home: return Key::Home;
        case XK_Insert: return Key::Insert;
        case XK_Delete: return Key::Delete;
        case XK_KP_Add: return Key::Add;
        case XK_KP_Subtract: return Key::Substract;
        case XK_KP_Multiply: return Key::Multiply;
        case XK_KP_Divide: return Key::Divide;
        case XK_Left: return Key::Left;
        case XK_Right: return Key::Right;
        case XK_Up: return Key::Up;
        case XK_Down: return Key::Down;
        case XK_KP_0: return Key::Numpad0;
        case XK_KP_1: return Key::Numpad1;
        case XK_KP_2: return Key::Numpad2;
        case XK_KP_3: return Key::Numpad3;
        case XK_KP_4: return Key::Numpad4;
        case XK_KP_5: return Key::Numpad5;
        case XK_KP_6: return Key::Numpad6;
        case XK_KP_7: return Key::Numpad7;
        case XK_KP_8: return Key::Numpad8;
        case XK_KP_9: return Key::Numpad9;
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
        case XK_Pause: return Key::Pause;
    }

    return Key::Unknow;
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
        case Key::Num0: return XK_0;
        case Key::Num1: return XK_1;
        case Key::Num2: return XK_2;
        case Key::Num3: return XK_3;
        case Key::Num4: return XK_4;
        case Key::Num5: return XK_5;
        case Key::Num6: return XK_6;
        case Key::Num7: return XK_7;
        case Key::Num8: return XK_8;
        case Key::Num9: return XK_9;
        case Key::Escape: return XK_Escape;
        case Key::L_Control: return XK_Control_L;
        case Key::L_Shift: return XK_Shift_L;
        case Key::L_Alt: return XK_Alt_L;
        case Key::L_System: return XK_Super_L;
        case Key::R_Control: return XK_Control_R;
        case Key::R_Shift: return XK_Shift_R;
        case Key::R_Alt: return XK_Alt_R;
        case Key::R_System: return XK_Super_R;
        case Key::Menu: return XK_Menu;
        case Key::L_Bracket: return XK_bracketleft;
        case Key::R_Bracket: return XK_bracketright;
        case Key::Semi_Colon: return XK_semicolon;
        case Key::Comma: return XK_comma;
        case Key::Period: return XK_period;
        case Key::Quote: return XK_quoteleft;
        case Key::Slash: return XK_slash;
        case Key::Back_Slash: return XK_backslash;
        case Key::Tilde: return XK_dead_grave;
        case Key::Equal: return XK_equal;
        case Key::Hyphen: return XK_hyphen;
        case Key::Space: return XK_space;
        case Key::Enter: return XK_Return;
        case Key::Back_Space: return XK_BackSpace;
        case Key::Tab: return XK_Tab;
        case Key::Page_Up: return XK_Page_Up;
        case Key::Page_Down: return XK_Page_Down;
        case Key::Begin: return XK_Begin;
        case Key::End: return XK_End;
        case Key::Home: return XK_Home;
        case Key::Insert: return XK_Insert;
        case Key::Delete: return XK_Delete;
        case Key::Add: return XK_KP_Add;
        case Key::Substract: return XK_KP_Subtract;
        case Key::Multiply: return XK_KP_Multiply;
        case Key::Divide: return XK_KP_Divide;
        case Key::Left: return XK_Left;
        case Key::Right: return XK_Right;
        case Key::Up: return XK_Up;
        case Key::Down: return XK_Down;
        case Key::Numpad0: return XK_KP_0;
        case Key::Numpad1: return XK_KP_1;
        case Key::Numpad2: return XK_KP_2;
        case Key::Numpad3: return XK_KP_3;
        case Key::Numpad4: return XK_KP_4;
        case Key::Numpad5: return XK_KP_5;
        case Key::Numpad6: return XK_KP_6;
        case Key::Numpad7: return XK_KP_7;
        case Key::Numpad8: return XK_KP_8;
        case Key::Numpad9: return XK_KP_9;
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
        case Key::Pause: return XK_Pause;
        case Key::Unknow:
        default: return {};
    }

    return {};
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::MouseButton x11MouseButtonToStormMouseButton(xcb_button_t button) {
    switch (button) {
        case XCB_BUTTON_INDEX_1: return storm::window::MouseButton::Left;
        case XCB_BUTTON_INDEX_2: return storm::window::MouseButton::Middle;
        case XCB_BUTTON_INDEX_3: return storm::window::MouseButton::Right;
        case XCB_BUTTON_INDEX_4: return storm::window::MouseButton::Button1;
        case XCB_BUTTON_INDEX_5: return storm::window::MouseButton::Button2;
    }

    return storm::window::MouseButton::Unknow;
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
