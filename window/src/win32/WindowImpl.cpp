#include "WindowImpl.hpp"
#include "Utils.hpp"

#include <locale>
#include <string>

#include <WinUser.h>

using namespace storm;
using namespace storm::window;

std::wstring fromStdString(std::string_view str) {
    const auto size = gsl::narrow_cast<std::size_t>(
        MultiByteToWideChar(CP_UTF8, 0, std::data(str), -1, nullptr, 0));
    auto wstr = std::wstring {};
    wstr.resize(size);

    MultiByteToWideChar(CP_UTF8,
                        0,
                        std::data(str),
                        -1,
                        std::data(wstr),
                        gsl::narrow_cast<int>(size));

    return wstr;
}

/////////////////////////////////////
/////////////////////////////////////
WindowImpl::WindowImpl() = default;

/////////////////////////////////////
/////////////////////////////////////
WindowImpl::~WindowImpl() {
    if (m_window_handle) DestroyWindow(m_window_handle);

    UnregisterClassW(CLASS_NAME, GetModuleHandleW(nullptr));
}

/////////////////////////////////////
/////////////////////////////////////
WindowImpl::WindowImpl(const std::string &title, const VideoSettings &settings, WindowStyle style)
    : m_is_open { false }, m_is_visible { false } {
    create(title, settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::create(const std::string &title,
                        const VideoSettings &settings,
                        WindowStyle style) {
    registerWindowClass();

    auto _style     = DWORD { WS_BORDER };
    auto h_instance = GetModuleHandleA(nullptr);

    if ((style & WindowStyle::TitleBar) == WindowStyle::TitleBar) _style |= WS_CAPTION;
    if ((style & WindowStyle::Close) == WindowStyle::Close) _style |= WS_SYSMENU;
    if ((style & WindowStyle::Minimizable) == WindowStyle::Minimizable) _style |= WS_MINIMIZEBOX;
    if ((style & WindowStyle::Resizable) == WindowStyle::Resizable)
        _style |= WS_MAXIMIZEBOX | WS_THICKFRAME;

    m_window_handle = CreateWindowExW(0l,
                                      CLASS_NAME,
                                      fromStdString(title).c_str(),
                                      _style,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      settings.size.w,
                                      settings.size.h,
                                      nullptr,
                                      nullptr,
                                      h_instance,
                                      this);

    if ((style & WindowStyle::Fullscreen) == WindowStyle::Fullscreen) {
        auto mode         = DEVMODE {};
        mode.dmSize       = sizeof(DEVMODE);
        mode.dmBitsPerPel = settings.bpp;
        mode.dmPelsWidth  = settings.size.width;
        mode.dmPelsHeight = settings.size.height;
        mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

        ChangeDisplaySettings(&mode, CDS_FULLSCREEN);

        SetWindowLongW(m_window_handle, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        SetWindowLongW(m_window_handle, GWL_EXSTYLE, WS_EX_APPWINDOW);

        SetWindowPos(m_window_handle,
                     HWND_TOP,
                     0,
                     0,
                     settings.size.width,
                     settings.size.height,
                     SWP_FRAMECHANGED);
    }

    ShowWindow(m_window_handle, SW_SHOW);

    m_is_open       = true;
    m_is_visible    = true;
    m_is_fullscreen = false;
    m_resizing      = false;
    m_mouse_inside  = false;

    m_last_size = settings.size;
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::close() noexcept {
    m_is_open    = false;
    m_is_visible = false;
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::display() noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::pollEvent(Event &event, void *native_event) noexcept {
    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return AbstractWindow::pollEvent(event, native_event);
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::waitEvent(Event &event, void *native_event) noexcept {
    MSG message;
    auto has_message = WaitMessage();
    while (has_message && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return AbstractWindow::waitEvent(event, native_event);
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::setTitle(const std::string &title) noexcept {
    SetWindowTextW(m_window_handle, fromStdString(title).c_str());
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::setVideoSettings(const storm::window::VideoSettings &settings) noexcept {
    auto rectangle =
        RECT { 0, 0, static_cast<long>(settings.size.w), static_cast<long>(settings.size.h) };

    AdjustWindowRect(&rectangle, GetWindowLongW(m_window_handle, GWL_STYLE), false);
    auto width  = rectangle.right - rectangle.left;
    auto height = rectangle.bottom - rectangle.top;

    SetWindowPos(m_window_handle, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

/////////////////////////////////////
/////////////////////////////////////
core::Extentu WindowImpl::size() const noexcept {
    RECT rect;
    GetClientRect(m_window_handle, &rect);

    return { gsl::narrow_cast<core::UInt32>(rect.right - rect.left),
             gsl::narrow_cast<core::UInt32>(rect.bottom - rect.top) };
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::isOpen() const noexcept {
    return m_is_open;
}

/////////////////////////////////////
/////////////////////////////////////
bool WindowImpl::isVisible() const noexcept {
    return m_is_visible;
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::NativeHandle WindowImpl::nativeHandle() const noexcept {
    return static_cast<storm::window::NativeHandle>(m_window_handle);
}

/////////////////////////////////////
/////////////////////////////////////
LRESULT
WindowImpl::globalOnEvent(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) {
    if (message == WM_CREATE) {
        auto lp_create_params = reinterpret_cast<CREATESTRUCT *>(l_param)->lpCreateParams;

        SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lp_create_params));
    }

    auto window =
        handle ? reinterpret_cast<WindowImpl *>(GetWindowLongPtrW(handle, GWLP_USERDATA)) : nullptr;
    if (window) { window->processEvents(message, w_param, l_param); }

    if (message == WM_CLOSE) return 0;

    if ((message == WM_SYSCOMMAND) && (w_param == SC_KEYMENU)) return 0;

    return DefWindowProcW(handle, message, w_param, l_param);
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::registerWindowClass() {
    auto window_class = WNDCLASSW {};
    std::memset(&window_class, 0, sizeof(window_class));

    window_class.lpfnWndProc   = &WindowImpl::globalOnEvent;
    window_class.hInstance     = GetModuleHandleW(nullptr);
    window_class.lpszClassName = CLASS_NAME;

    RegisterClassW(&window_class);
}

/////////////////////////////////////
/////////////////////////////////////
void WindowImpl::processEvents(UINT message, WPARAM w_param, LPARAM l_param) {
    if (!m_window_handle) return;

    switch (message) {
        case WM_CLOSE: closeEvent(); break;
        case WM_SIZE:
            if ((w_param != SIZE_MINIMIZED) && !m_resizing && (m_last_size != size())) {
                m_last_size = size();

                resizeEvent(m_last_size.w, m_last_size.h);
            }
            break;
        case WM_ENTERSIZEMOVE: m_resizing = true; break;
        case WM_EXITSIZEMOVE:
            m_resizing = false;

            if (m_last_size != size()) {
                m_last_size = size();

                resizeEvent(m_last_size.w, m_last_size.h);
            }
            break;
        case WM_LBUTTONDOWN: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseDownEvent(MouseButton::LEFT, x, y);
            break;
        }
        case WM_LBUTTONUP: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseUpEvent(MouseButton::LEFT, x, y);
            break;
        }
        case WM_RBUTTONDOWN: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseDownEvent(MouseButton::RIGHT, x, y);
            break;
        }
        case WM_RBUTTONUP: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseUpEvent(MouseButton::RIGHT, x, y);
            break;
        }
        case WM_MBUTTONDOWN: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseDownEvent(MouseButton::MIDDLE, x, y);
            break;
        }
        case WM_MBUTTONUP: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseUpEvent(MouseButton::MIDDLE, x, y);
            break;
        }
        case WM_XBUTTONDOWN: {
            auto x      = static_cast<core::Int16>(LOWORD(l_param));
            auto y      = static_cast<core::Int16>(HIWORD(l_param));
            auto button = HIWORD(w_param);
            if (button == XBUTTON1) mouseDownEvent(MouseButton::BUTTON1, x, y);
            else if (button == XBUTTON2)
                mouseDownEvent(MouseButton::BUTTON1, x, y);
            break;
        }
        case WM_XBUTTONUP: {
            auto x      = static_cast<core::Int16>(LOWORD(l_param));
            auto y      = static_cast<core::Int16>(HIWORD(l_param));
            auto button = HIWORD(w_param);
            if (button == XBUTTON1) mouseUpEvent(MouseButton::BUTTON2, x, y);
            else if (button == XBUTTON2)
                mouseUpEvent(MouseButton::BUTTON2, x, y);
            break;
        }
        case WM_MOUSEMOVE: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));

            auto area = RECT {};
            GetClientRect(m_window_handle, &area);

            if ((w_param & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) ==
                0) {
                if (GetCapture() == m_window_handle) ReleaseCapture();
            } else if (GetCapture() != m_window_handle)
                SetCapture(m_window_handle);

            if ((x < area.left) || (x > area.right) || (y < area.top) || (y > area.bottom)) {
                if (m_mouse_inside) {
                    m_mouse_inside = false;

                    auto mouseEvent        = TRACKMOUSEEVENT {};
                    mouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
                    mouseEvent.dwFlags     = TME_CANCEL;
                    mouseEvent.hwndTrack   = m_window_handle;
                    mouseEvent.dwHoverTime = HOVER_DEFAULT;
                    TrackMouseEvent(&mouseEvent);

                    mouseExitedEvent();
                }
            } else {
                if (!m_mouse_inside) {
                    m_mouse_inside = true;

                    auto mouseEvent        = TRACKMOUSEEVENT {};
                    mouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
                    mouseEvent.dwFlags     = TME_LEAVE;
                    mouseEvent.hwndTrack   = m_window_handle;
                    mouseEvent.dwHoverTime = HOVER_DEFAULT;
                    TrackMouseEvent(&mouseEvent);

                    mouseEnteredEvent();
                }
            }

            mouseMoveEvent(x, y);
            break;
        }
        case WM_KEYDOWN: [[fallthrough]];
        case WM_SYSKEYDOWN: {
            auto key = win32KeyToStormKey(w_param, l_param);

            keyDownEvent(key);
            break;
        }
        case WM_KEYUP: [[fallthrough]];
        case WM_SYSKEYUP: {
            auto key = win32KeyToStormKey(w_param, l_param);

            keyUpEvent(key);
            break;
        }
    }
}
