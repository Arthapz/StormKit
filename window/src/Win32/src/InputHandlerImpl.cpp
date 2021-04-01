#include "InputHandlerImpl.hpp"
#include "Utils.hpp"

/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

using namespace storm;
using namespace storm::window;

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
        case Key::Num0: vkey = '0'; break;
        case Key::Num1: vkey = '1'; break;
        case Key::Num2: vkey = '2'; break;
        case Key::Num3: vkey = '3'; break;
        case Key::Num4: vkey = '4'; break;
        case Key::Num5: vkey = '5'; break;
        case Key::Num6: vkey = '6'; break;
        case Key::Num7: vkey = '7'; break;
        case Key::Num8: vkey = '8'; break;
        case Key::Num9: vkey = '9'; break;
        case Key::Escape: vkey = VK_ESCAPE; break;
        case Key::L_Control: vkey = VK_LCONTROL; break;
        case Key::L_Shift: vkey = VK_LSHIFT; break;
        case Key::L_Alt: vkey = VK_LMENU; break;
        case Key::L_System: vkey = VK_LWIN; break;
        case Key::R_Control: vkey = VK_RCONTROL; break;
        case Key::R_Shift: vkey = VK_RSHIFT; break;
        case Key::R_Alt: vkey = VK_RMENU; break;
        case Key::R_System: vkey = VK_RWIN; break;
        case Key::Menu: vkey = VK_APPS; break;
        case Key::L_Bracket: vkey = VK_OEM_4; break;
        case Key::R_Bracket: vkey = VK_OEM_6; break;
        case Key::Semi_Colon: vkey = VK_OEM_1; break;
        case Key::Comma: vkey = VK_OEM_COMMA; break;
        case Key::Period: vkey = VK_OEM_PERIOD; break;
        case Key::Quote: vkey = VK_OEM_7; break;
        case Key::Slash: vkey = VK_OEM_2; break;
        case Key::Back_Slash: vkey = VK_OEM_5; break;
        case Key::Tilde: vkey = VK_OEM_3; break;
        case Key::Equal: vkey = VK_OEM_PLUS; break;
        case Key::Hyphen: vkey = VK_OEM_MINUS; break;
        case Key::Space: vkey = VK_SPACE; break;
        case Key::Enter: vkey = VK_RETURN; break;
        case Key::Back_Space: vkey = VK_BACK; break;
        case Key::Tab: vkey = VK_TAB; break;
        case Key::Page_Up: vkey = VK_PRIOR; break;
        case Key::Page_Down: vkey = VK_NEXT; break;
        case Key::End: vkey = VK_END; break;
        case Key::Home: vkey = VK_HOME; break;
        case Key::Insert: vkey = VK_INSERT; break;
        case Key::Delete: vkey = VK_DELETE; break;
        case Key::Add: vkey = VK_ADD; break;
        case Key::Substract: vkey = VK_SUBTRACT; break;
        case Key::Multiply: vkey = VK_MULTIPLY; break;
        case Key::Divide: vkey = VK_DIVIDE; break;
        case Key::Left: vkey = VK_LEFT; break;
        case Key::Right: vkey = VK_RIGHT; break;
        case Key::Up: vkey = VK_UP; break;
        case Key::Down: vkey = VK_DOWN; break;
        case Key::Numpad0: vkey = VK_NUMPAD0; break;
        case Key::Numpad1: vkey = VK_NUMPAD1; break;
        case Key::Numpad2: vkey = VK_NUMPAD2; break;
        case Key::Numpad3: vkey = VK_NUMPAD3; break;
        case Key::Numpad4: vkey = VK_NUMPAD4; break;
        case Key::Numpad5: vkey = VK_NUMPAD5; break;
        case Key::Numpad6: vkey = VK_NUMPAD6; break;
        case Key::Numpad7: vkey = VK_NUMPAD7; break;
        case Key::Numpad8: vkey = VK_NUMPAD8; break;
        case Key::Numpad9: vkey = VK_NUMPAD9; break;
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
        case Key::Pause: vkey = VK_PAUSE; break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

/////////////////////////////////////
/////////////////////////////////////
bool InputHandlerImpl::isMouseButtonPressed(MouseButton button) const noexcept {
    auto vkey = 0;
    switch (button) {
        case MouseButton::Left:
            vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
            break;
        case MouseButton::Right:
            vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
            break;
        case MouseButton::Middle: vkey = VK_MBUTTON; break;
        case MouseButton::Button1: vkey = VK_XBUTTON1; break;
        case MouseButton::Button2: vkey = VK_XBUTTON2; break;
        default: vkey = 0; break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
    return false;
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandlerImpl::getMousePositionOnDesktop() const noexcept {
    auto point = POINT {};
    GetCursorPos(&point);

    const auto position = core::Vector2u { point.x, point.y };

    return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePositionOnDesktop(core::Position2u position) noexcept {
    SetCursorPos(position->x, position->y);
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2i InputHandlerImpl::getMousePositionOnWindow() const noexcept {
    auto handle = reinterpret_cast<HWND>(m_window->nativeHandle());

    auto point = POINT {};
    GetCursorPos(&point);
    ScreenToClient(handle, &point);

    const auto position = core::Vector2i { point.x, point.y };

    return core::makeNamed<core::Position2i>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setMousePositionOnWindow(core::Position2i position) noexcept {
    auto handle = reinterpret_cast<HWND>(m_window->nativeHandle());
    if (handle) {
        auto point =
            POINT { gsl::narrow_cast<long>(position->x), gsl::narrow_cast<long>(position->y) };
        ClientToScreen(handle, &point);
        SetCursorPos(point.x, point.y);
    }
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandlerImpl::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept {
    // not supported
}