// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/lua/lua.hpp>

module stormkit.lua:core;

import std;

import stormkit.core;

namespace lb = luabridge;

namespace stormkit::lua::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto bind_color(lb::Namespace&& global_namespace) noexcept -> lb::Namespace {
        return global_namespace.beginClass<rgbcolor<f32>>("fcolor").endClass().beginClass<rgbcolor<u8>>("ucolor").endClass();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto bind_extent(lb::Namespace&& global_namespace) noexcept -> lb::Namespace {
        return global_namespace.beginNamespace("math")
          .beginClass<math::uextent2>("uextent2")
          .endClass()
          .beginClass<math::uextent3>("uextent3")
          .endClass()
          .beginClass<math::extent2<f32>>("fextent2")
          .endClass()
          .beginClass<math::extent3<f32>>("fextent3")
          .endClass()
          .endNamespace();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    export inline auto init_lua(lb::Namespace&& global_namespace) noexcept -> lb::Namespace {
        global_namespace = bind_color(std::move(global_namespace));
        global_namespace = bind_extent(std::move(global_namespace));
        return global_namespace;
    }
} // namespace stormkit::lua::core
