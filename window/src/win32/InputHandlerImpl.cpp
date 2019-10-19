#include "InputHandlerImpl.hpp"
#include "Utils.hpp"

#include <storm/window/Window.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
InputHandlerImpl::InputHandlerImpl() = default;

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
bool InputHandlerImpl::isKeyPressed(Key key) {
    int vkey = 0;
    switch (key) {
        default: vkey = 0; break;
        case Key::A: vkey = 'A'; break;
        case Key::B: vkey = 'B'; break;
        case Key::C: vkey = 'C'; break;
        case Key::D: vkey = 'D'; break;
        case Key::E: vkey = 'E'; break;
        case Key::F: vkey = 'F'; break;
        case Key::G: vkey = 'G'; break;
        case Key::H: vkey = 'H'; break;
        case Key::I: vkey = 'I'; break;
        case Key::J: vkey = 'J'; break;
        case Key::K: vkey = 'K'; break;
        case Key::L: vkey = 'L'; break;
        case Key::M: vkey = 'M'; break;
        case Key::N: vkey = 'N'; break;
        case Key::O: vkey = 'O'; break;
        case Key::P: vkey = 'P'; break;
        case Key::Q: vkey = 'Q'; break;
        case Key::R: vkey = 'R'; break;
        case Key::S: vkey = 'S'; break;
        case Key::T: vkey = 'T'; break;
        case Key::U: vkey = 'U'; break;
        case Key::V: vkey = 'V'; break;
        case Key::W: vkey = 'W'; break;
        case Key::X: vkey = 'X'; break;
        case Key::Y: vkey = 'Y'; break;
        case Key::Z: vkey = 'Z'; break;
        case Key::NUM0: vkey = '0'; break;
        case Key::NUM1: vkey = '1'; break;
        case Key::NUM2: vkey = '2'; break;
        case Key::NUM3: vkey = '3'; break;
        case Key::NUM4: vkey = '4'; break;
        case Key::NUM5: vkey = '5'; break;
        case Key::NUM6: vkey = '6'; break;
        case Key::NUM7: vkey = '7'; break;
        case Key::NUM8: vkey = '8'; break;
        case Key::NUM9: vkey = '9'; break;
        case Key::ESCAPE: vkey = VK_ESCAPE; break;
        case Key::LCONTROL: vkey = VK_LCONTROL; break;
        case Key::LSHIFT: vkey = VK_LSHIFT; break;
        case Key::LALT: vkey = VK_LMENU; break;
        case Key::LSYSTEM: vkey = VK_LWIN; break;
        case Key::RCONTROL: vkey = VK_RCONTROL; break;
        case Key::RSHIFT: vkey = VK_RSHIFT; break;
        case Key::RALT: vkey = VK_RMENU; break;
        case Key::RSYSTEM: vkey = VK_RWIN; break;
        case Key::MENU: vkey = VK_APPS; break;
        case Key::LBRACKET: vkey = VK_OEM_4; break;
        case Key::RBRACKET: vkey = VK_OEM_6; break;
        case Key::SEMICOLON: vkey = VK_OEM_1; break;
        case Key::COMMA: vkey = VK_OEM_COMMA; break;
        case Key::PERIOD: vkey = VK_OEM_PERIOD; break;
        case Key::QUOTE: vkey = VK_OEM_7; break;
        case Key::SLASH: vkey = VK_OEM_2; break;
        case Key::BACKSLASH: vkey = VK_OEM_5; break;
        case Key::TILDE: vkey = VK_OEM_3; break;
        case Key::EQUAL: vkey = VK_OEM_PLUS; break;
        case Key::HYPHEN: vkey = VK_OEM_MINUS; break;
        case Key::SPACE: vkey = VK_SPACE; break;
        case Key::ENTER: vkey = VK_RETURN; break;
        case Key::BACKSPACE: vkey = VK_BACK; break;
        case Key::TAB: vkey = VK_TAB; break;
        case Key::PAGEUP: vkey = VK_PRIOR; break;
        case Key::PAGEDOWN: vkey = VK_NEXT; break;
        case Key::END: vkey = VK_END; break;
        case Key::HOME: vkey = VK_HOME; break;
        case Key::INSERT: vkey = VK_INSERT; break;
        case Key::DELETE: vkey = VK_DELETE; break;
        case Key::ADD: vkey = VK_ADD; break;
        case Key::SUBSTRACT: vkey = VK_SUBTRACT; break;
        case Key::MULTIPLY: vkey = VK_MULTIPLY; break;
        case Key::DIVIDE: vkey = VK_DIVIDE; break;
        case Key::LEFT: vkey = VK_LEFT; break;
        case Key::RIGHT: vkey = VK_RIGHT; break;
        case Key::UP: vkey = VK_UP; break;
        case Key::DOWN: vkey = VK_DOWN; break;
        case Key::NUMPAD0: vkey = VK_NUMPAD0; break;
        case Key::NUMPAD1: vkey = VK_NUMPAD1; break;
        case Key::NUMPAD2: vkey = VK_NUMPAD2; break;
        case Key::NUMPAD3: vkey = VK_NUMPAD3; break;
        case Key::NUMPAD4: vkey = VK_NUMPAD4; break;
        case Key::NUMPAD5: vkey = VK_NUMPAD5; break;
        case Key::NUMPAD6: vkey = VK_NUMPAD6; break;
        case Key::NUMPAD7: vkey = VK_NUMPAD7; break;
        case Key::NUMPAD8: vkey = VK_NUMPAD8; break;
        case Key::NUMPAD9: vkey = VK_NUMPAD9; break;
        case Key::F1: vkey = VK_F1; break;
        case Key::F2: vkey = VK_F2; break;
        case Key::F3: vkey = VK_F3; break;
        case Key::F4: vkey = VK_F4; break;
        case Key::F5: vkey = VK_F5; break;
        case Key::F6: vkey = VK_F6; break;
        case Key::F7: vkey = VK_F7; break;
        case Key::F8: vkey = VK_F8; break;
        case Key::F9: vkey = VK_F9; break;
        case Key::F10: vkey = VK_F10; break;
        case Key::F11: vkey = VK_F11; break;
        case Key::F12: vkey = VK_F12; break;
        case Key::F13: vkey = VK_F13; break;
        case Key::F14: vkey = VK_F14; break;
        case Key::F15: vkey = VK_F15; break;
        case Key::PAUSE: vkey = VK_PAUSE; break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isMouseButtonPressed(MouseButton button) {
    auto vkey = 0;
    switch (button) {
        case MouseButton::LEFT:
            vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
            break;
        case MouseButton::RIGHT:
            vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
            break;
        case MouseButton::MIDDLE: vkey = VK_MBUTTON; break;
        case MouseButton::BUTTON1: vkey = VK_XBUTTON1; break;
        case MouseButton::BUTTON2: vkey = VK_XBUTTON2; break;
        default: vkey = 0; break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePosition(core::Position2u position) {
    SetCursorPos(position->x, position->y);
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePosition(core::Position2i position, const Window &relative_to) {
    auto handle = reinterpret_cast<HWND>(relative_to.nativeHandle());
    if (handle) {
        auto point =
            POINT { gsl::narrow_cast<long>(position->x), gsl::narrow_cast<long>(position->y) };
        ClientToScreen(handle, &point);
        SetCursorPos(point.x, point.y);
    }
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandlerImpl::getMousePosition() {
    auto point = POINT {};
    GetCursorPos(&point);

    const auto position = core::Vector2u { point.x, point.y };

    return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2i InputHandlerImpl::getMousePosition(const Window &relative_to) {
    auto handle = reinterpret_cast<HWND>(relative_to.nativeHandle());

    auto point = POINT {};
    GetCursorPos(&point);
    ScreenToClient(handle, &point);

    const auto position = core::Vector2i { point.x, point.y };

    return core::makeNamed<core::Position2i>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setVirtualKeyboardVisible([[maybe_unused]] bool visible) {
    // not supported
}
