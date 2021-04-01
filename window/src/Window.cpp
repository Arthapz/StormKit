/////////// - StormKit::window - ///////////
#include <storm/core/Assert.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

/////////// - StormKit::module - ///////////
#include <storm/module/Module.hpp>

#include "Log.hpp"

#ifdef STORMKIT_OS_LINUX
    #include "Linux/X11/X11Keyboard.hpp"
    #include "Linux/X11/X11Mouse.hpp"
    #include "Linux/X11/X11Window.hpp"
#endif

using namespace storm;
using namespace storm::window;
using namespace std::literals;

/////////////////////////////////////
/////////////////////////////////////
Window::Window() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Window::Window(std::string title, const VideoSettings &settings, WindowStyle style) noexcept {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
Window::~Window() {
}

/////////////////////////////////////
/////////////////////////////////////
Window::Window(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Window &Window::operator=(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
auto Window::create(std::string title, const VideoSettings &settings, WindowStyle style) noexcept
    -> void {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
            dlog("Using Win32 window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::X11: dlog("Using XCB window backend");
#ifdef STORMKIT_OS_LINUX
            m_impl = details::X11Window::allocateOwned(std::move(title), settings, style);
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland:
            dlog("Using Wayland window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::macOS:
            dlog("Using macOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::iOS:
            dlog("Using iOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Android:
            dlog("Using Android window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Switch:
            dlog("Using Nintendo Switch window backend");
            ASSERT(true, "Not yet implemented");
            break;
        default: ASSERT(true, "Unhandled platform");
    }
}
/////////////////////////////////////
/////////////////////////////////////
auto Window::close() noexcept -> void {
    m_impl->close();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::pollEvent(Event &event) noexcept -> bool {
    return m_impl->pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::waitEvent(Event &event) noexcept -> bool {
    return m_impl->waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::setTitle(std::string title) noexcept -> void {
    m_impl->setTitle(std::move(title));
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::setVideoSettings(const VideoSettings &settings) noexcept -> void {
    m_impl->setVideoSettings(settings);
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::size() const noexcept -> const core::Extentu & {
    return m_impl->size();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::title() const noexcept -> std::string_view {
    return m_impl->title();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::videoSettings() const noexcept -> const VideoSettings & {
    return m_impl->videoSettings();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::isOpen() const noexcept -> bool {
    return m_impl->isOpen();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::isVisible() const noexcept -> bool {
    return m_impl->isVisible();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::nativeHandle() const noexcept -> NativeHandle {
    return m_impl->nativeHandle();
}

/////////////////////////////////////
/////////////////////////////////////
KeyboardOwnedPtr Window::createKeyboardPtr() const {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
            dlog("Using Win32 window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::X11: dlog("Using XCB window backend");
#ifdef STORMKIT_OS_LINUX
            return details::X11Keyboard::allocateOwned(*m_impl);
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland:
            dlog("Using Wayland window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::macOS:
            dlog("Using macOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::iOS:
            dlog("Using iOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Android:
            dlog("Using Android window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Switch:
            dlog("Using Nintendo Switch window backend");
            ASSERT(true, "Not yet implemented");
            break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
MouseOwnedPtr Window::createMousePtr() const {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
            dlog("Using Win32 window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::X11: dlog("Using XCB window backend");
#ifdef STORMKIT_OS_LINUX
            return details::X11Mouse::allocateOwned(*m_impl);
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland:
            dlog("Using Wayland window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::macOS:
            dlog("Using macOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::iOS:
            dlog("Using iOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Android:
            dlog("Using Android window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Switch:
            dlog("Using Nintendo Switch window backend");
            ASSERT(true, "Not yet implemented");
            break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
Window::WM Window::detectWM() noexcept {
#if defined(STORMKIT_OS_WINDOWS)
    return WM::Win32;
#elif defined(STORMKIT_OS_MACOS)
    return WM::macOS;
#elif defined(STORMKIT_OS_IOS)
    return WM::iOS;
#elif defined(STORMKIT_OS_ANDROID)
    return WM::Android;
#elif defined(STORMKIT_OS_SWITCH)
    return WM::Switch;
#elif defined(STORMKIT_OS_LINUX)
    auto is_wayland = std::getenv("WAYLAND_SESSION") != nullptr;

    if (is_wayland) return WM::Wayland;
    else
        return WM::X11;
#endif
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<VideoSettings> Window::getDesktopModes() {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
            dlog("Using Win32 window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::X11: dlog("Using XCB window backend");
#ifdef STORMKIT_OS_LINUX
            return details::X11Window::getDesktopModes();
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland:
            dlog("Using Wayland window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::macOS:
            dlog("Using macOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::iOS:
            dlog("Using iOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Android:
            dlog("Using Android window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Switch:
            dlog("Using Nintendo Switch window backend");
            ASSERT(true, "Not yet implemented");
            break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
VideoSettings Window::getDesktopFullscreenSize() {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
            dlog("Using Win32 window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::X11: dlog("Using XCB window backend");
#ifdef STORMKIT_OS_LINUX
            return details::X11Window::getDesktopFullscreenSize();
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland:
            dlog("Using Wayland window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::macOS:
            dlog("Using macOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::iOS:
            dlog("Using iOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Android:
            dlog("Using Android window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Switch:
            dlog("Using Nintendo Switch window backend");
            ASSERT(true, "Not yet implemented");
            break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}
