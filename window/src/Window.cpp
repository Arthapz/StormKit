/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

/////////// - StormKit::module - ///////////
#include <storm/module/Module.hpp>

using namespace storm;
using namespace storm::window;
using namespace std::literals;

#define SUFFIX
#ifdef STORM_BUILD_DEBUG
    #undef SUFFIX
    #define SUFFIX "-debug"
#endif

static constexpr auto WIN32_MODULE_FILEPATH   = "libStormKit-window-win32" SUFFIX ".dll"sv;
static constexpr auto MACOS_MODULE_FILEPATH   = "libStormKit-window-macos" SUFFIX ".dylib"sv;
static constexpr auto X11_MODULE_FILEPATH     = "libStormKit-window-x11" SUFFIX ".so"sv;
static constexpr auto WAYLAND_MODULE_FILEPATH = "libStormKit-window-wayland" SUFFIX ".so"sv;
static constexpr auto IOS_MODULE_FILEPATH     = "libStormKit-window-ios" SUFFIX ".dylib"sv;
static constexpr auto ANDROID_MODULE_FILEPATH = "libStormKit-window-android" SUFFIX ".so"sv;

#undef SUFFIX

namespace details {
    module::ModuleOwnedPtr window_plugin;

    std::function<AbstractWindow *()> window_create_func;
    std::function<void(AbstractWindow *)> window_destroy_func;

    std::function<AbstractInputHandler *(const Window &window)> input_handle_create_func;
    std::function<void(AbstractInputHandler *)> input_handle_destroy_func;

    std::function<const VideoSettings *(core::ArraySize &size)> get_desktop_modes_func;
} // namespace details

/////////////////////////////////////
/////////////////////////////////////
Window::Window() noexcept {
    initPlugin();

    m_impl = core::makeObserver(details::window_create_func());
}

/////////////////////////////////////
/////////////////////////////////////
Window::Window(const std::string &title, const VideoSettings &settings, WindowStyle style) noexcept
    : Window {} {
    create(title, settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
Window::~Window() {
    if (m_impl) details::window_destroy_func(m_impl.get());
}

/////////////////////////////////////
/////////////////////////////////////
Window::Window(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Window &Window::operator=(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
core::span<const VideoSettings> Window::getDesktopModes() {
    initPlugin();

    auto size     = core::ArraySize { 0u };
    auto settings = details::get_desktop_modes_func(size);

    return { settings, size };
}

/////////////////////////////////////
/////////////////////////////////////
const VideoSettings &Window::getDesktopFullscreenSize() {
    initPlugin();

    auto size     = core::ArraySize { 0u };
    auto settings = details::get_desktop_modes_func(size);

    return *settings;
}

/////////////////////////////////////
/////////////////////////////////////
void Window::initPlugin() {
    if (details::window_plugin == nullptr) {
        const auto wm = detectWM();

        const auto &module_name = [wm]() {
            switch (wm) {
                case WM::Win32: return WIN32_MODULE_FILEPATH;
                case WM::macOS: return MACOS_MODULE_FILEPATH;
                case WM::X11: return X11_MODULE_FILEPATH;
                case WM::Wayland: return WAYLAND_MODULE_FILEPATH;
                case WM::iOS: return IOS_MODULE_FILEPATH;
                case WM::Android: return ANDROID_MODULE_FILEPATH;
            }

            STORM_ENSURES(true);

            return ""sv;
        }();

        details::window_plugin = std::make_unique<module::Module>(module_name);

        details::window_create_func =
            details::window_plugin->getFunc<AbstractWindow *()>("createWindow");
        details::window_destroy_func =
            details::window_plugin->getFunc<void(AbstractWindow *)>("destroyWindow");

        details::input_handle_create_func =
            details::window_plugin->getFunc<AbstractInputHandler *(const Window &)>(
                "createInputHandler");
        details::input_handle_destroy_func =
            details::window_plugin->getFunc<void(AbstractInputHandler *)>("destroyInputHandler");
    }
}

/////////////////////////////////////
/////////////////////////////////////
Window::WM Window::detectWM() noexcept {
#if defined(STORM_OS_WINDOWS)
    return WM::Win32;
#elif defined(STORM_OS_MACOS)
    return WM::macOS;
#elif defined(STORM_OS_IOS)
    return WM::iOS;
#elif defined(STORM_OS_ANDROID)
    return WM::Android;
#elif defined(STORM_OS_SWITCH)
    return WM::Switch;
#elif defined(STORM_OS_LINUX)
    auto is_wayland = std::getenv("WAYLAND_SESSION") != nullptr;

    if (is_wayland) return WM::Wayland;
    else
        return WM::X11;
#endif
}
