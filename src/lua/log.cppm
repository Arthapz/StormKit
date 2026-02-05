// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/lua/lua.hpp>

module stormkit.lua:log;

import std;

import stormkit.core;
import stormkit.log;

namespace lb = luabridge;

export namespace stormkit::lua::log {
    inline auto init_lua(lb::Namespace&& global_namespace) noexcept -> lb::Namespace {
        return global_namespace;
    }
} // namespace stormkit::lua::log
