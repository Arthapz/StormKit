#import "View.h"
#import "ViewController.h"
#import "Window.hpp"

#import <UIKit/UIApplication.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UIWindow.h>

#include <storm/window/WindowStyle.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
Window::Window() noexcept
    : AbstractWindow {}, m_window { nil }, m_view_controller { nil }, m_view { nil } {
}

/////////////////////////////////////
/////////////////////////////////////
Window::Window(const std::string &title,
                       const storm::window::VideoSettings &settings,
                       const storm::window::WindowStyle style) noexcept
    : Window {} {
    create(title, settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
Window::~Window() {
}

/////////////////////////////////////
/////////////////////////////////////
void Window::create(const std::string &title,
                        const VideoSettings &settings,
                        WindowStyle style) noexcept {
    m_title          = title;
    m_video_settings = settings;
    m_style          = style;

    const auto scale_factor = [[UIScreen mainScreen] nativeScale];
    const auto screen_rect  = [UIScreen mainScreen].bounds;

    m_video_settings = VideoSettings::getDesktopModes()[0];

    m_window = [[UIWindow alloc] initWithFrame:screen_rect];

    m_view = [[View alloc] initWithFrame:screen_rect withScaleFactor:scale_factor];
    [m_view resignFirstResponder];

    m_view.backgroundColor = UIColor.redColor;

    m_view_controller = [ViewController alloc];
    m_view_controller.hideStatusBar
        = ((m_style & WindowStyle::Fullscreen) == WindowStyle::Fullscreen) ? YES : NO;
    m_view_controller.view = m_view;
    // m_view_controller.title = title;

    m_window.rootViewController = m_view_controller;
    [m_window makeKeyAndVisible];
}

/////////////////////////////////////
/////////////////////////////////////
void Window::close() noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
bool Window::poll_event(storm::window::Event &event, void *) noexcept {
    while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.0001, true) == kCFRunLoopRunHandledSource);

    return AbstractWindow::poll_event(event);
}

/////////////////////////////////////
/////////////////////////////////////
bool Window::wait_event(storm::window::Event &event, void *native_event) noexcept {
    ;
    while (!AbstractWindow::wait_event(event, native_event));

    return true;
}

/////////////////////////////////////
/////////////////////////////////////
void Window::set_title(const std::string &title) noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
void Window::setVideoSettings(const VideoSettings &settings) noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
storm::core::Extentu Window::size() const noexcept {
    return m_video_settings.size;
}

/////////////////////////////////////
/////////////////////////////////////
bool Window::is_open() const noexcept {
    return true;
}

/////////////////////////////////////
/////////////////////////////////////
bool Window::isVisible() const noexcept {
    return true;
}

/////////////////////////////////////
/////////////////////////////////////
storm::window::NativeHandle Window::native_handle() const noexcept {
    return m_view;
}
