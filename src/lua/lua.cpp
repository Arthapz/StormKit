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

namespace lb = luabridge;

LOGGER("stormkit.lua")

namespace stormkit::lua {
    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine::Engine(Modules modules) noexcept {
        m_global_state = luaL_newstate();
        luaL_openlibs(m_global_state);

        auto global_namespace = lb::getGlobalNamespace(m_global_state);

        global_namespace = core::init_lua(std::move(global_namespace));
        if (modules.log) {
#if STORMKIT_LIB_LOG_ENABLED
            dlog("Log module enabled");
            global_namespace = log::init_lua(std::move(global_namespace));
#else
            elog("Trying to bind log module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.entities) {
#if STORMKIT_LIB_ENTITIES_ENABLED
            dlog("Entities module enabled");
            global_namespace = entities::init_lua(std::move(global_namespace));
#else
            elog("Trying to bind entities module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.image) {
#if STORMKIT_LIB_IMAGE_ENABLED
            dlog("Image module enabled");
            global_namespace = image::init_lua(std::move(global_namespace));
#else
            elog("Trying to bind image module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.wsi) {
#if STORMKIT_LIB_WSI_ENABLED
            dlog("Wsi module enabled");
            global_namespace = wsi::init_lua(std::move(global_namespace));
#else
            elog("Trying to bind wsi module while disabled in this stormkit distribution!");
#endif
        }
        if (modules.gpu) {
#if STORMKIT_LIB_GPU_ENABLED
            dlog("Gpu module enabled");
            global_namespace = gpu::init_lua(std::move(global_namespace));
#else
            elog("Trying to bind gpu module while disabled in this stormkit distribution!");
#endif
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine::Engine(Engine&& other) noexcept
        : m_global_state { std::exchange(other.m_global_state, nullptr) },
          m_main_thread { std::exchange(other.m_main_thread, nullptr) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Engine::operator=(Engine&& other) noexcept -> Engine& {
        if (&other == this) [[unlikely]]
            return *this;
        m_global_state = std::exchange(other.m_global_state, nullptr);
        m_main_thread  = std::exchange(other.m_main_thread, nullptr);

        return *this;
    };

    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine::~Engine() noexcept {
        if (m_main_thread) m_main_thread = nullptr;

        if (m_global_state) {
            lua_close(m_global_state);
            m_global_state = nullptr;
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Engine::load(const stdfs::path& file) noexcept -> void {
        auto data = TryAssert(io::read_text<io::Mode::AINSI>(file), std::format("Failed to load {}", file.string()));

        auto bytecode_size = 0_usize;
        auto bytecode      = luau_compile(stdr::data(data), stdr::size(data), nullptr, &bytecode_size);
        auto result        = luau_load(m_global_state, "main", bytecode, bytecode_size, 0);
        std::free(bytecode);

        if (result != 0) {
            auto len = usize { 0 };
            auto msg = lua_tolstring(m_global_state, -1, &len);

            ensures(result == 0,
                    std::format("Lua compilation error!\n-------------------------------\n{}", std::string_view { msg, len }));
        }

        // m_main_thread = lua_newthread(m_global_state);
        // ensures(m_main_thread);

        // lua_pushvalue(m_global_state, -2);
        // lua_remove(m_global_state, -3);
        // lua_xmove(m_global_state, m_main_thread, 1);
    }

    static int traceback(lua_State* L) {
        if (!lua_isstring(L, 1)) /* 'message' not a string? */
            return 1;            /* keep it intact */
        lua_getfield(L, LUA_GLOBALSINDEX, "debug");
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            return 1;
        }
        lua_getfield(L, -1, "traceback");
        if (!lua_isfunction(L, -1)) {
            lua_pop(L, 2);
            return 1;
        }
        lua_pushvalue(L, 1);   /* pass error message */
        lua_pushinteger(L, 2); /* skip this function and traceback */
        lua_call(L, 2, 1);     /* call debug.traceback */
        return 1;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Engine::lua_main() noexcept -> std::expected<void, std::string> {
        // EXPECTS(m_main_thread);

        auto out = std::expected<void, std::string> {};

        traceback(m_global_state);

        luaL_sandbox(m_global_state);
        luaL_sandboxthread(m_global_state);

        auto status = lua_resume(m_global_state, nullptr, 0);
        if (status == 0) {
            //  if (const auto n = lua_gettop(m_global_state); n) {
            //      luaL_checkstack(m_global_state, LUA_MINSTACK, "too many results to print");
            //      lua_getglobal(m_global_state, "print");
            //      lua_insert(m_global_state, 1);
            //      lua_pcall(m_global_state, n, 0, 0);
            //  }

            // lua_pop(m_global_state, 1);
        } else {
            traceback(m_global_state);
            auto error = std::string { lua_tostring(m_global_state, -1) };
            out        = std::unexpected { std::move(error) };
        }

        return out;
    }
} // namespace stormkit::lua
