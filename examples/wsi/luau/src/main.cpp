// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/luau/lua.hpp>

import std;

import stormkit.core;
import stormkit.main;
import stormkit.log;
import stormkit.wsi;
import stormkit.luau;

#include <stormkit/main/main_macro.hpp>

#include <stormkit/log/log_macro.hpp>

LOGGER("Luau-Events");

#ifndef LUAU_DIR
    #define LUAU_DIR "../luau"
#endif

using namespace stormkit;

////////////////////////////////////////
////////////////////////////////////////
auto main(std::span<const std::string_view> args) -> int {
    wsi::parse_args(args);
    auto logger = log::Logger::create_logger_instance<log::ConsoleLogger>();

    auto engine = luau::Engine::create(LUAU_DIR "/events.luau");
    wsi::lua::init_lua(engine.global_namespace());
    engine.lua_main().transform_error(monadic::assert("lua runtime error!\n-------------------------------\n"));

    return 0;
}
