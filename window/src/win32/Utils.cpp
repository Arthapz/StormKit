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
            return scancode == l_shift ? Key::LSHIFT : Key::RSHIFT;
        }
        case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? Key::RALT : Key::LALT;
        case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? Key::RCONTROL : Key::LCONTROL;
        case VK_LWIN: return Key::LSYSTEM;
        case VK_RWIN: return Key::RSYSTEM;
        case VK_APPS: return Key::MENU;
        case VK_OEM_1: return Key::SEMICOLON;
        case VK_OEM_2: return Key::SLASH;
        case VK_OEM_PLUS: return Key::EQUAL;
        case VK_OEM_MINUS: return Key::HYPHEN;
        case VK_OEM_4: return Key::LBRACKET;
        case VK_OEM_6: return Key::RBRACKET;
        case VK_OEM_COMMA: return Key::COMMA;
        case VK_OEM_PERIOD: return Key::PERIOD;
        case VK_OEM_7: return Key::QUOTE;
        case VK_OEM_5: return Key::BACKSLASH;
        case VK_OEM_3: return Key::TILDE;
        case VK_ESCAPE: return Key::ESCAPE;
        case VK_SPACE: return Key::SPACE;
        case VK_RETURN: return Key::ENTER;
        case VK_BACK: return Key::BACKSPACE;
        case VK_TAB: return Key::TAB;
        case VK_PRIOR: return Key::PAGEUP;
        case VK_NEXT: return Key::PAGEDOWN;
        case VK_END: return Key::END;
        case VK_HOME: return Key::HOME;
        case VK_INSERT: return Key::INSERT;
        case VK_DELETE: return Key::DELETE;
        case VK_ADD: return Key::ADD;
        case VK_SUBTRACT: return Key::SUBSTRACT;
        case VK_MULTIPLY: return Key::MULTIPLY;
        case VK_DIVIDE: return Key::DIVIDE;
        case VK_PAUSE: return Key::PAUSE;
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
        case VK_LEFT: return Key::LEFT;
        case VK_RIGHT: return Key::RIGHT;
        case VK_UP: return Key::UP;
        case VK_DOWN: return Key::DOWN;
        case VK_NUMPAD0: return Key::NUMPAD0;
        case VK_NUMPAD1: return Key::NUMPAD1;
        case VK_NUMPAD2: return Key::NUMPAD2;
        case VK_NUMPAD3: return Key::NUMPAD3;
        case VK_NUMPAD4: return Key::NUMPAD4;
        case VK_NUMPAD5: return Key::NUMPAD5;
        case VK_NUMPAD6: return Key::NUMPAD6;
        case VK_NUMPAD7: return Key::NUMPAD7;
        case VK_NUMPAD8: return Key::NUMPAD8;
        case VK_NUMPAD9: return Key::NUMPAD9;
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
        case '0': return Key::NUM0;
        case '1': return Key::NUM1;
        case '2': return Key::NUM2;
        case '3': return Key::NUM3;
        case '4': return Key::NUM4;
        case '5': return Key::NUM5;
        case '6': return Key::NUM6;
        case '7': return Key::NUM7;
        case '8': return Key::NUM8;
        case '9': return Key::NUM9;
    }

    return Key::UNKNOW;
}
