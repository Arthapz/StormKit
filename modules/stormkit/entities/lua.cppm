// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/luau/lua.hpp>

export module stormkit.entities:lua;

import std;

import stormkit.core;

namespace lb = luabridge;

export namespace stormkit::entities::lua {
    inline auto init_lua(lb::Namespace global_namespace) noexcept -> void;
}

namespace stormkit::entities::lua {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto init_lua(lb::Namespace) noexcept -> void {
    }
} // namespace stormkit::entities::lua
