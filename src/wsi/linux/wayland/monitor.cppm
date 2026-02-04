// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.wsi:linux.wayland.monitor;

import std;

import :core;
import :monitor;

import :linux.wayland.context;

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace stormkit::wsi::linux::wayland {
    /////////////////////////////////////
    /////////////////////////////////////
    auto get_monitors(WM, bool update) noexcept -> std::span<const Monitor> {
        thread_local auto monitors = std::vector<Monitor> {};
        if (update or stdr::empty(monitors)) {
            auto& globals = wl::get_globals();
            monitors      = globals.monitors
                       | stdv::transform([](const wl::WaylandMonitor& pair) static noexcept { return pair.monitor; })
                       | stdr::to<std::vector>();
        }

        return monitors;
    }
} // namespace stormkit::wsi::linux::wayland
