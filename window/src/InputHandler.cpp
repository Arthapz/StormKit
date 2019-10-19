#include <storm/window/InputHandler.hpp>

#if defined(STORM_OS_MACOS)
#include "macOS/InputHandlerImpl.h"
#elif defined(STORM_OS_LINUX)
#if defined(STORM_X11)
#include "x11/InputHandlerImpl.hpp"
#elif defined(STORM_WAYLAND)
#include "wayland/InputHandlerImpl.hpp"
#endif
#elif defined(STORM_OS_WINDOWS)
#include "win32/InputHandlerImpl.hpp"
#endif

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
InputHandler::InputHandler() : m_impl{} {
}

/////////////////////////////////////
/////////////////////////////////////
InputHandler::~InputHandler() = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandler::InputHandler(InputHandler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
InputHandler &InputHandler::operator=(InputHandler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
bool InputHandler::isKeyPressed(Key key) {
	return InputHandlerImpl::isKeyPressed(key);
}

/////////////////////////////////////
/////////////////////////////////////
bool InputHandler::isMouseButtonPressed(MouseButton button) {
	return InputHandlerImpl::isMouseButtonPressed(button);
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandler::setMousePosition(core::Position2u position) {
	InputHandlerImpl::setMousePosition(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandler::setMousePosition(core::Position2u position,
									const Window &relative_to) {
	InputHandlerImpl::setMousePosition(std::move(position), relative_to);
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandler::getMousePosition() {
	return InputHandlerImpl::getMousePosition();
}

/////////////////////////////////////
/////////////////////////////////////
core::Position2u InputHandler::getMousePosition(const Window &relative_to) {
	return InputHandlerImpl::getMousePosition(relative_to);
}

/////////////////////////////////////
/////////////////////////////////////
void InputHandler::setVirtualKeyboardVisible(bool visible) {
	InputHandlerImpl::setVirtualKeyboardVisible(visible);
}
