// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/main/main_macro.hpp>

#include <stormkit/log/log_macro.hpp>

#include <stormkit/core/try_expected.hpp>

import std;

import stormkit;

LOGGER("lua-Events");

#ifndef RESOURCE_DIR
    #define RESOURCE_DIR "../share/stormkit"
#endif

namespace stdfs = std::filesystem;

static const auto LUA_FILE = stdfs::path { RESOURCE_DIR } / stdfs::path { "lua/events.luau" };

using namespace stormkit;

////////////////////////////////////////
////////////////////////////////////////
auto main(std::span<const std::string_view> args) -> int {
    wsi::parse_args(args);
    log::parse_args(args);

    auto logger = log::Logger::create_logger_instance<log::ConsoleLogger>();

    auto engine = lua::Engine::create(LUA_FILE, { .wsi = true });
    engine.lua_main();

    return 0;
}
