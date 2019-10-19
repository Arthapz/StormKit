#include <storm/core/Platform.hpp>
#include <storm/window/Window.hpp>

#if defined(STORM_OS_MACOS)
#include "macOS/WindowImpl.h"
#elif defined(STORM_OS_LINUX)
#if defined(STORM_X11)
#include "x11/WindowImpl.hpp"
#elif defined(STORM_WAYLAND)
#include "wayland/WindowImpl.hpp"
#endif
#elif defined(STORM_OS_WINDOWS)
#include "win32/WindowImpl.hpp"
#endif

using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
storm::window::Window::Window() noexcept : m_impl(nullptr) {
	m_impl = std::make_unique<WindowImpl>();
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::Window::Window(const std::string &title,
							  const VideoSettings &settings,
							  WindowStyle style) noexcept
	: m_impl(nullptr) {
	m_impl = std::make_unique<WindowImpl>(title, settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
void Window::display() noexcept {
	m_impl->display();
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::Window::~Window() = default;

/////////////////////////////////////
/////////////////////////////////////
storm::window::Window::Window(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
storm::window::Window &storm::window::Window::operator=(Window &&) = default;
