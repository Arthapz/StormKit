// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Utils.hpp"

storm::window::Key win32KeyToStormKey(WPARAM key, LPARAM flags) {
    using namespace storm::window;

    switch (key) {
        case VK_SHIFT: {
            static const auto l_shift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
            auto scancode             = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
            return scancode == l_shift ? Key::L_Shift : Key::R_Shift;
        }
        case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? Key::R_Alt : Key::L_Alt;
        case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? Key::R_Control : Key::L_Control;
        case VK_LWIN: return Key::L_System;
        case VK_RWIN: return Key::R_System;
        case VK_APPS: return Key::Menu;
        case VK_OEM_1: return Key::Semi_Colon;
        case VK_OEM_2: return Key::Slash;
        case VK_OEM_PLUS: return Key::Equal;
        case VK_OEM_MINUS: return Key::Hyphen;
        case VK_OEM_4: return Key::L_Bracket;
        case VK_OEM_6: return Key::R_Bracket;
        case VK_OEM_COMMA: return Key::Comma;
        case VK_OEM_PERIOD: return Key::Period;
        case VK_OEM_7: return Key::Quote;
        case VK_OEM_5: return Key::BACKSLASH;
        case VK_OEM_3: return Key::Tilde;
        case VK_ESCAPE: return Key::Escape;
        case VK_SPACE: return Key::Space;
        case VK_RETURN: return Key::Enter;
        case VK_BACK: return Key::Back_Space;
        case VK_TAB: return Key::Tab;
        case VK_PRIOR: return Key::Page_Up;
        case VK_NEXT: return Key::Page_Down;
        case VK_END: return Key::End;
        case VK_HOME: return Key::Home;
        case VK_INSERT: return Key::Insert;
        case VK_DELETE: return Key::Delete;
        case VK_ADD: return Key::Add;
        case VK_SUBTRACT: return Key::Substract;
        case VK_MULTIPLY: return Key::Multiply;
        case VK_DIVIDE: return Key::Divide;
        case VK_PAUSE: return Key::Pause;
        case VK_F1: return Key::F1;
        case VK_F2: return Key::F2;
        case VK_F3: return Key::F3;
        case VK_F4: return Key::F4;
        case VK_F5: return Key::F5;
        case VK_F6: return Key::F6;
        case VK_F7: return Key::F7;
        case VK_F8: return Key::F8;
        case VK_F9: return Key::F9;
        case VK_F10: return Key::F10;
        case VK_F11: return Key::F11;
        case VK_F12: return Key::F12;
        case VK_F13: return Key::F13;
        case VK_F14: return Key::F14;
        case VK_F15: return Key::F15;
        case VK_LEFT: return Key::Left;
        case VK_RIGHT: return Key::Right;
        case VK_UP: return Key::Up;
        case VK_DOWN: return Key::Down;
        case VK_NUMPAD0: return Key::Numpad0;
        case VK_NUMPAD1: return Key::Numpad1;
        case VK_NUMPAD2: return Key::Numpad2;
        case VK_NUMPAD3: return Key::Numpad3;
        case VK_NUMPAD4: return Key::Numpad4;
        case VK_NUMPAD5: return Key::Numpad5;
        case VK_NUMPAD6: return Key::Numpad6;
        case VK_NUMPAD7: return Key::Numpad7;
        case VK_NUMPAD8: return Key::Numpad8;
        case VK_NUMPAD9: return Key::Numpad9;
        case 'A': return Key::A;
        case 'Z': return Key::Z;
        case 'E': return Key::E;
        case 'R': return Key::R;
        case 'T': return Key::T;
        case 'Y': return Key::Y;
        case 'U': return Key::U;
        case 'I': return Key::I;
        case 'O': return Key::O;
        case 'P': return Key::P;
        case 'Q': return Key::Q;
        case 'S': return Key::S;
        case 'D': return Key::D;
        case 'F': return Key::F;
        case 'G': return Key::G;
        case 'H': return Key::H;
        case 'J': return Key::J;
        case 'K': return Key::K;
        case 'L': return Key::L;
        case 'M': return Key::M;
        case 'W': return Key::W;
        case 'X': return Key::X;
        case 'C': return Key::C;
        case 'V': return Key::V;
        case 'B': return Key::B;
        case 'N': return Key::N;
        case '0': return Key::Num0;
        case '1': return Key::Num1;
        case '2': return Key::Num2;
        case '3': return Key::Num3;
        case '4': return Key::Num4;
        case '5': return Key::Num5;
        case '6': return Key::Num6;
        case '7': return Key::Num7;
        case '8': return Key::Num8;
        case '9': return Key::Num9;
    }

    return Key::Unknow;
}
