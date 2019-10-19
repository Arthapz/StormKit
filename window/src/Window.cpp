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
#elif defined(STORM_OS_IOS)
    #include "iOS/WindowImpl.hpp"
#endif

using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
Window::Window() noexcept : m_impl(nullptr) {
    m_impl = std::make_unique<WindowImpl>();
}

/////////////////////////////////////
/////////////////////////////////////
Window::Window(const std::string &title, const VideoSettings &settings, WindowStyle style) noexcept
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
Window::~Window() = default;

/////////////////////////////////////
/////////////////////////////////////
Window::Window(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Window &Window::operator=(Window &&) = default;
