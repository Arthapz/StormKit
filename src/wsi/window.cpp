// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/contract_macro.hpp>

module stormkit.wsi;

import std;

import stormkit.core;

import :window;

#if defined(STORMKIT_OS_WINDOWS)
import :win32.window;

namespace impl = stormkit::wsi::win32;
#elif defined(STORMKIT_OS_LINUX)
import :linux.window;

namespace impl = stormkit::wsi::linux;
#elif defined(STORMKIT_OS_MACOS)
import :macos.window;

namespace impl = stormkkit::wsi::macos;
#elif defined(STORMKIT_OS_IOS)
import :ios.window;

namespace impl = stormkit::wsi::ios;
#else
    #error "OS not supported !"
#endif

using namespace std::literals;

namespace stormkit::wsi {
    class WindowImpl: public impl::Window {
      public:
        using impl::Window::Window;
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Window::Window() noexcept : m_wm { wsi::wm() }, m_impl { m_wm } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Window::~Window() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Window::Window(Window&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::operator=(Window&&) noexcept -> Window& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::open(std::string title, const math::Extent2<u32>& size, WindowFlag flags) noexcept
      -> Window {
        auto window = Window {};
        window.m_impl->open(std::move(title), size, flags);
        return window;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::close() noexcept -> void {
        m_impl->close();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::clear(const rgbcolor<u8>& color) noexcept -> void {
        m_impl->clear(color);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::fill_framebuffer(std::span<const rgbcolor<u8>> colors) noexcept -> void {
        m_impl->fill_framebuffer(colors);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_open() const noexcept -> bool {
        return m_impl->is_open();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::handle_events() noexcept -> void {
        m_impl->handle_events();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::visible() const noexcept -> bool {
        return m_impl->visible();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::current_monitor() const noexcept -> const Monitor& {
        return m_impl->current_monitor();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::set_title(std::string title) noexcept -> void {
        m_impl->set_title(std::move(title));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::title() const noexcept -> const std::string& {
        return m_impl->title();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::set_extent(const math::Extent2<u32>& extent) noexcept -> void {
        m_impl->set_extent(extent);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::extent() const noexcept -> const math::Extent2<u32>& {
        return m_impl->extent();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::framebuffer_extent() const noexcept -> math::Extent2<u32> {
        return m_impl->framebuffer_extent();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::set_fullscreen(bool fullscreen) noexcept -> void {
        m_impl->set_fullscreen(fullscreen);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::fullscreen() const noexcept -> bool {
        return m_impl->fullscreen();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::confine_mouse(bool confine, u32 mouse_id) noexcept -> void {
        m_impl->confine_mouse(confine, mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_mouse_confined(u32 mouse_id) const noexcept -> bool {
        return m_impl->is_mouse_confined(mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::lock_mouse(bool locked, u32 mouse_id) noexcept -> void {
        m_impl->lock_mouse(locked, mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_mouse_locked(u32 mouse_id) const noexcept -> bool {
        return m_impl->is_mouse_locked(mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::hide_mouse(bool hidden, u32 mouse_id) noexcept -> void {
        m_impl->hide_mouse(hidden, mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_mouse_hidden(u32 mouse_id) const noexcept -> bool {
        return m_impl->is_mouse_hidden(mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::set_relative_mouse(bool enabled, u32 mouse_id) noexcept -> void {
        m_impl->set_relative_mouse(enabled, mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_mouse_relative(u32 mouse_id) const noexcept -> bool {
        return m_impl->is_mouse_relative(mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::set_key_repeat(bool enabled, u32 keyboard_id) noexcept -> void {
        return m_impl->set_key_repeat(enabled, keyboard_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_key_repeat_enabled(u32 keyboard_id) const noexcept -> bool {
        return m_impl->is_key_repeat_enabled(keyboard_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::show_virtual_keyboard(bool visible) noexcept -> void {
        m_impl->show_virtual_keyboard(visible);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::is_virtual_keyboard_visible() const noexcept -> bool {
        return m_impl->is_virtual_keyboard_visible();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::set_mouse_position(const math::vec2i& position, u32 mouse_id) noexcept -> void {
        m_impl->set_mouse_position(position, mouse_id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::native_handle() const noexcept -> NativeHandle {
        return m_impl->native_handle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_closed(ClosedEventFunc&& callback) noexcept -> void {
        m_impl->closed_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_monitor_changed(MonitorChangedEventFunc&& callback) noexcept -> void {
        m_impl->monitor_changed_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_resized(ResizedEventFunc&& callback) noexcept -> void {
        m_impl->resized_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_restored(RestoredEventFunc&& callback) noexcept -> void {
        m_impl->restored_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_minimized(MinimizedEventFunc&& callback) noexcept -> void {
        m_impl->minimized_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_maximized(MaximizedEventFunc&& callback) noexcept -> void {
        m_impl->maximized_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_key_down(KeyDownEventFunc&& callback) noexcept -> void {
        m_impl->key_down_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_key_up(KeyUpEventFunc&& callback) noexcept -> void {
        m_impl->key_up_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_mouse_button_down(MouseButtonDownEventFunc&& callback) noexcept -> void {
        m_impl->mouse_button_down_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_mouse_button_up(MouseButtonUpEventFunc&& callback) noexcept -> void {
        m_impl->mouse_button_up_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_mouse_moved(MouseMovedEventFunc&& callback) noexcept -> void {
        m_impl->mouse_moved_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_activate(ActivateEventFunc&& callback) noexcept -> void {
        m_impl->activate_event = std::move(callback);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::on_deactivate(DeactivateEventFunc&& callback) noexcept -> void {
        m_impl->deactivate_event = std::move(callback);
    }
} // namespace stormkit::wsi
