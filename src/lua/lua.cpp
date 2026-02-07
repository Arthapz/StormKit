// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#define NO_CONSTANTS
#include <stormkit/core/config.hpp>

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/log/log_macro.hpp>

#include <stormkit/lua/lua.hpp>

module stormkit.lua;

import stormkit.core;
import :core;

import stormkit.log;
import :log;

#if STORMKIT_LIB_ENTITIES_ENABLED
import stormkit.entities;
import :entities;
#endif
#if STORMKIT_LIB_IMAGE_ENABLED
import stormkit.image;
import :image;
#endif
#if STORMKIT_LIB_WSI_ENABLED
import stormkit.wsi;
import :wsi;
#endif
#if STORMKIT_LIB_GPU_ENABLED
import stormkit.gpu;
import :gpu;
#endif

namespace stdfs = std::filesystem;
namespace stdr  = std::ranges;

LOGGER("stormkit.lua")

namespace stormkit::lua {
    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine::Engine(Modules modules) noexcept {
        for (auto lib : {
               sol::lib::base,
               sol::lib::bit32,
               sol::lib::debug,
               sol::lib::io,
               sol::lib::os,
               sol::lib::string,
               sol::lib::table,
             })
            m_global_state.open_libraries(lib);

        auto tostring              = sol::protected_function { m_global_state["tostring"] };
        m_global_state["tostring"] = [format = std::move(tostring)](sol::object v) noexcept {
            if (not v.valid()) return std::string {};
            if (v.is<sol::lua_table>()) {
                auto out = std::string { "[lua_table " };
                for (auto&& [key, value] : v.as<sol::table>()) {
                    auto key_as_string   = sol::object { luacall(format, key) }.as<std::string>();
                    auto value_as_string = sol::object { luacall(format, value) }.as<std::string>();
                    out += key_as_string;
                    out += ": ";
                    out += value_as_string;
                    out += ", ";
                }
                out = out.substr(0, stdr::size(out) - 2);
                out += "]";
                return out;
            }
            return sol::object { luacall(format, v) }.as<std::string>();
        };
        m_global_state["print"] = [this](std::string_view str, sol::variadic_args args) noexcept {
            const auto format = sol::protected_function { m_global_state["format"] };
            const auto result = luacall(format, str, std::move(args));
            const auto out    = sol::object { result }.as<std::string>();
            std::println("{}", out);
        };

        m_global_state["format"] = [this](std::string_view str, sol::variadic_args args) noexcept {
            auto slices = split(str, "{}");
            if (stdr::size(slices) == 1) { return std::format("{}", str); }

            expects(args.size() == stdr::size(slices) - 1,
                    std::format("Invalid count of args! should be {}, got {}", stdr::size(slices) - 1, args.size()));

            auto out = std::string {};
            out.reserve(stdr::size(str));
            auto it = stdr::begin(slices);
            out += *it;
            ++it;
            for (auto v : args) {
                auto       format = sol::protected_function { m_global_state["tostring"] };
                const auto result = luacall(m_global_state["tostring"], v);
                out += sol::object { result }.as<std::string>();

                out += *it;
                ++it;

                if (it == stdr::cend(slices)) break;
            }

            return out;
        };

        core::init_lua(m_global_state);
        if (modules.log) {
#if STORMKIT_LIB_LOG_ENABLED
            dlog("Log module enabled");
            log::init_lua(m_global_state);
#else
            elog("Trying to bind log module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.entities) {
#if STORMKIT_LIB_ENTITIES_ENABLED
            dlog("Entities module enabled");
            entities::init_lua(m_global_state);
#else
            elog("Trying to bind entities module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.image) {
#if STORMKIT_LIB_IMAGE_ENABLED
            dlog("Image module enabled");
            image::init_lua(m_global_state);
#else
            elog("Trying to bind image module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.wsi) {
#if STORMKIT_LIB_WSI_ENABLED
            dlog("Wsi module enabled");
            wsi::init_lua(m_global_state);
#else
            elog("Trying to bind wsi module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.gpu) {
#if STORMKIT_LIB_GPU_ENABLED
            dlog("Gpu module enabled");
            gpu::init_lua(m_global_state);
#else
            elog("Trying to bind gpu module while disabled in this stormkit distribution!");
#endif
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine::Engine(Engine&& other) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Engine::operator=(Engine&& other) noexcept -> Engine& = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine::~Engine() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Engine::load(const stdfs::path& file) noexcept -> void {
        const auto code = TryAssert(io::read_text<io::Mode::AINSI>(file), std::format("Failed to load {}", file.string()));
        m_script        = m_global_state.load(std::string_view { stdr::data(code), stdr::size(code) });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Engine::lua_main() noexcept -> void {
        const auto function = sol::protected_function { m_script };
        luacall(function);
    }
} // namespace stormkit::lua
