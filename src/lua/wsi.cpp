// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/lua/lua.hpp>

module stormkit.lua;

import std;

import stormkit.core;
import stormkit.wsi;

namespace stormkit::lua::wsi {
    extern auto bind_core(sol::state&, sol::table&) noexcept -> void;
    extern auto bind_window(sol::state&, sol::table&) noexcept -> void;
    extern auto bind_monitor(sol::state&, sol::table&) noexcept -> void;
    extern auto bind_keyboard(sol::state&, sol::table&) noexcept -> void;
    extern auto bind_mouse(sol::state&, sol::table&) noexcept -> void;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto init_lua(sol::state& global_state) noexcept -> void {
        auto wsi_metatable = global_state["wsi"].get_or_create<sol::table>();
        bind_core(global_state, wsi_metatable);
        bind_window(global_state, wsi_metatable);
        bind_monitor(global_state, wsi_metatable);
        bind_keyboard(global_state, wsi_metatable);
        bind_mouse(global_state, wsi_metatable);
    }
} // namespace stormkit::lua::wsi
