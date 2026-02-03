// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/luau/lua.hpp>

module stormkit.wsi;

import std;

import stormkit.core;

import :core;

import :core;
import :mouse;
import :keyboard;
import :monitor;
import :window;

namespace lb = luabridge;

namespace stormkit::wsi::lua {
    using stormkit::wsi::EventType;
    using stormkit::wsi::Window;
    using stormkit::wsi::WindowFlag;
    using stormkit::wsi::WM;

    namespace {
        ////////////////////////////////////////
        ////////////////////////////////////////
        auto bind_core(lb::Namespace& global_namespace) noexcept -> lb::Namespace {
            return global_namespace.beginNamespace("wsi")
              .beginNamespace("wm")
              .addProperty("WIN32", [] static noexcept { return WM::WIN32; })
              .addProperty("WAYLAND", [] static noexcept { return WM::WAYLAND; })
              .addProperty("X11", [] static noexcept { return WM::X11; })
              .addProperty("ANDROID", [] static noexcept { return WM::ANDROID; })
              .addProperty("MACOS", [] static noexcept { return WM::MACOS; })
              .addProperty("IOS", [] static noexcept { return WM::IOS; })
              .addProperty("TVOS", [] static noexcept { return WM::TVOS; })
              .addProperty("SWITCH", [] static noexcept { return WM::SWITCH; })
              .endNamespace()
              .endNamespace();
        }

        ////////////////////////////////////////
        ////////////////////////////////////////
        template<wsi::EventType EVENT_TYPE, typename... Args>
    STORMKIT_FORCE_INLINE
        constexpr auto make_lua_closure() noexcept {
            return [](Window* window, lb::LuaRef func) static noexcept {
                window->on<EVENT_TYPE>([func = std::move(func)](Args&&... args) noexcept {
                    const auto result = func(std::forward<Args>(args)...);
                    ensures(result.wasOk(), result.errorMessage());
                });
            };
        }

        ////////////////////////////////////////
        ////////////////////////////////////////
        auto bind_window(lb::Namespace& global_namespace) noexcept -> lb::Namespace {
            return global_namespace.beginNamespace("wsi")
              .beginNamespace("window_flag")
              .addProperty("DEFAULT", [] static noexcept { return WindowFlag::DEFAULT; })
              .addProperty("BORDERLESS", [] static noexcept { return WindowFlag::BORDERLESS; })
              .addProperty("RESIZEABLE", [] static noexcept { return WindowFlag::RESIZEABLE; })
              .addProperty("EXTERNAL_CONTEXT", [] static noexcept { return WindowFlag::EXTERNAL_CONTEXT; })
              .endNamespace()
              .beginNamespace("EventType")
              .addProperty("NONE", [] static noexcept { return EventType::NONE; })
              .addProperty("CLOSED", [] static noexcept { return EventType::CLOSED; })
              .addProperty("MONITOR_CHANGED", [] static noexcept { return EventType::MONITOR_CHANGED; })
              .addProperty("RESIZED", [] static noexcept { return EventType::RESIZED; })
              .addProperty("RESTORED", [] static noexcept { return EventType::RESTORED; })
              .addProperty("MINIMIZED", [] static noexcept { return EventType::MINIMIZED; })
              .addProperty("KEY_DOWN", [] static noexcept { return EventType::KEY_DOWN; })
              .addProperty("KEY_UP", [] static noexcept { return EventType::KEY_UP; })
              .addProperty("MOUSE_BUTTON_DOWN", [] static noexcept { return EventType::MOUSE_BUTTON_DOWN; })
              .addProperty("MOUSE_BUTTON_UP", [] static noexcept { return EventType::MOUSE_BUTTON_UP; })
              .addProperty("MOUSE_MOVED", [] static noexcept { return EventType::MOUSE_MOVED; })
              .addProperty("ACTIVATE", [] static noexcept { return EventType::ACTIVATE; })
              .addProperty("DEACTIVATE", [] static noexcept { return EventType::DEACTIVATE; })
              .endNamespace()
              .beginClass<Window>("window")
              .addFunction("wm", &Window::wm)
              .addFunction("extent", &Window::extent)
              .addFunction("clear", [](Window* window) static noexcept { window->clear(); })
              .addFunction("event_loop",
                           [](Window* window, lb::LuaRef func) static noexcept {
                               window->event_loop([func = std::move(func)] noexcept {
                                   const auto result = func();
                                   ensures(result.wasOk(), result.errorMessage());
                               });
                           })
              .addFunction("on_closed",
                           [](Window* window, lb::LuaRef func) static noexcept {
                               expects(func.isCallable());

                               window->on<EventType::CLOSED>([func = std::move(func)] noexcept {
                                   const auto result = func();
                                   ensures(result.wasOk(), result.errorMessage());
                                   const auto return_value = result[0];
                                   ensures(return_value.isValid());
                                   ensures(return_value.isBool(), "on_closed closure must return a boolean value");
                                   return return_value.cast<bool>().value();
                               });
                           })
              .addFunction("on_resized", (make_lua_closure<EventType::RESIZED, const math::uextent2&>()))
              .addFunction("on_restored", (make_lua_closure<EventType::RESTORED>()))
              .addFunction("on_minimized", (make_lua_closure<EventType::MINIMIZED>()))
              .addFunction("on_activate", (make_lua_closure<EventType::ACTIVATE>()))
              .addFunction("on_deactivate", (make_lua_closure<EventType::DEACTIVATE>()))
              .endClass()
              .addFunction("open_window", &open_window)
              .endNamespace();
        }
    } // namespace

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto init_lua(lb::Namespace global_namespace) noexcept -> void {
        global_namespace = bind_core(global_namespace);
        global_namespace = bind_window(global_namespace);
    }
} // namespace stormkit::wsi::lua
