
// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <wayland-client.h>
#include <xdg-shell.h>

#include <pointer-constraints-unstable-v1.h>
#include <relative-pointer-unstable-v1.h>
#include <xdg-decoration-unstable-v1.h>

#include <stormkit/core/flags_macro.hpp>

export module stormkit.wsi:linux.wayland.context;

import std;

import stormkit.core;

import :linux.common.fd;
import :linux.common.xkb;
import :linux.wayland;
import :linux.wayland.input;

export namespace stormkit::wsi::linux::wayland {
    class Window;

    namespace wl {
        struct WaylandMonitor {
            uptr         id;
            wsi::Monitor monitor;
        };

        struct Globals {
            bool                         initialized = false;
            wl::Display                  display     = wl::Display::empty();
            wl::Registry                 registry    = wl::Registry::empty();
            wl::Compositor               compositor  = wl::Compositor::empty();
            dyn_array<wl::Output>        outputs;
            wl::XDGWmBase                xdg_wm_base                 = wl::XDGWmBase::empty();
            wl::Shm                      shm                         = wl::Shm::empty();
            wl::XDGDecorationManager     decoration_manager          = wl::XDGDecorationManager::empty();
            wl::Seat                     seat                        = wl::Seat::empty();
            wl::SinglePixelBufferManager single_pixel_buffer_manager = wl::SinglePixelBufferManager::empty();
            wl::Viewporter               viewporter                  = wl::Viewporter::empty();
            wl::CursorShapeManager       cursor_shape_manager        = wl::CursorShapeManager::empty();
            wl::CursorShapeDevice        cursor_shape_device         = wl::CursorShapeDevice::empty();
            wl::PointerWarp              pointer_warp                = wl::PointerWarp::empty();
            wl::PointerConstraints       pointer_constraints         = wl::PointerConstraints::empty();
            wl::ContentTypeManager       content_type_manager        = wl::ContentTypeManager::empty();

            wl::CursorTheme cursor_theme          = wl::CursorTheme::empty();
            wl::CursorTheme cursor_theme_high_dpi = wl::CursorTheme::empty();

            dyn_array<std::pair<Keyboard, KeyboardState>> keyboards;
            dyn_array<std::pair<Pointer, PointerState>>   pointers;
            dyn_array<std::pair<Touch, TouchState>>       touchs;

            wl::RelativePointerManager relative_pointer_manager = wl::RelativePointerManager::empty();

            dyn_array<WaylandMonitor> monitors;

            dyn_array<std::pair<wl_surface*, Window*>> windows;

            common::xkb::Context xkb_context = common::xkb::Context::empty();
        };

        auto init() noexcept -> bool;
        auto get_globals() noexcept -> Globals&;
        auto get_monitor(Globals& _globals, void* output) noexcept -> Monitor&;
    } // namespace wl
} // namespace stormkit::wsi::linux::wayland

FLAG_ENUM(stormkit::wsi::linux::wayland::wl::PointerState::Flag);
