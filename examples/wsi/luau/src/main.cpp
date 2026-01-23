// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

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

auto open_window(std::string name, u32 width, u32 height, wsi::WindowFlag flags) noexcept {
    return wsi::Window::open(std::move(name), { width, height }, flags);
}

auto closed() {
    return wsi::EventType::CLOSED;
}

////////////////////////////////////////
////////////////////////////////////////
auto main(std::span<const std::string_view> args) -> int {
    wsi::parse_args(args);
    auto logger = log::Logger::create_logger_instance<log::ConsoleLogger>();

    auto engine = luau::Engine::create(LUAU_DIR "/events.luau");

    engine.global_namespace()
      .beginNamespace("wsi")
      .beginNamespace("WindowFlag")
      .addProperty(
        "DEFAULT",
        +[] static noexcept { return wsi::WindowFlag::DEFAULT; })
      .addProperty(
        "BORDERLESS",
        +[] static noexcept { return wsi::WindowFlag::BORDERLESS; })
      .addProperty(
        "RESIZEABLE",
        +[] static noexcept { return wsi::WindowFlag::RESIZEABLE; })
      .addProperty(
        "EXTERNAL_CONTEXT",
        +[] static noexcept { return wsi::WindowFlag::EXTERNAL_CONTEXT; })
      .endNamespace()
      .beginNamespace("EventType")
      .addProperty(
        "NONE",
        +[] static noexcept { return wsi::EventType::NONE; })
      .addProperty(
        "CLOSED",
        +[] static noexcept { return wsi::EventType::CLOSED; })
      .addProperty(
        "MONITOR_CHANGED",
        +[] static noexcept { return wsi::EventType::MONITOR_CHANGED; })
      .addProperty(
        "RESIZED",
        +[] static noexcept { return wsi::EventType::RESIZED; })
      .addProperty(
        "RESTORED",
        +[] static noexcept { return wsi::EventType::RESTORED; })
      .addProperty(
        "MINIMIZED",
        +[] static noexcept { return wsi::EventType::MINIMIZED; })
      .addProperty(
        "KEY_DOWN",
        +[] static noexcept { return wsi::EventType::KEY_DOWN; })
      .addProperty(
        "KEY_UP",
        +[] static noexcept { return wsi::EventType::KEY_UP; })
      .addProperty(
        "MOUSE_BUTTON_DOWN",
        +[] static noexcept { return wsi::EventType::MOUSE_BUTTON_DOWN; })
      .addProperty(
        "MOUSE_BUTTON_UP",
        +[] static noexcept { return wsi::EventType::MOUSE_BUTTON_UP; })
      .addProperty(
        "MOUSE_MOVED",
        +[] static noexcept { return wsi::EventType::MOUSE_MOVED; })
      .addProperty(
        "ACTIVATE",
        +[] static noexcept { return wsi::EventType::ACTIVATE; })
      .addProperty(
        "DEACTIVATE",
        +[] static noexcept { return wsi::EventType::DEACTIVATE; })
      .endNamespace()
      .beginClass<wsi::Window>("window")
      .addFunction("wm", &wsi::Window::wm)
      .endClass()
      .addFunction("open_window", open_window)
      .endNamespace();

    engine.lua_main().transform_error(monadic::assert("lua runtime error!\n-------------------------------\n"));

    return 0;
}
