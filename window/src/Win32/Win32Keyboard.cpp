#include "Win32Keyboard.hpp"
#include "Win32Window.hpp"
#include "Utils.hpp"

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
Win32Keyboard::Win32Keyboard(const AbstractWindow &window) noexcept : Keyboard { window } {
}

/////////////////////////////////////
/////////////////////////////////////
Win32Keyboard::~Win32Keyboard() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Win32Keyboard::Win32Keyboard(Win32Keyboard &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Win32Keyboard::operator=(Win32Keyboard &&) noexcept -> Win32Keyboard & = default;

/////////////////////////////////////
/////////////////////////////////////
auto Win32Keyboard::isKeyPressed(Key key) const noexcept -> bool {
    auto vkey = stormKitKeyToWin32(key);

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Keyboard::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept -> void {
    // not supported
}
