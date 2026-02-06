// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/lua/api.hpp>
#include <stormkit/lua/lua.hpp>

export module stormkit.lua;

import std;

import stormkit.core;

namespace stdfs = std::filesystem;

export namespace stormkit::lua {
    struct Modules {
        bool log      = false;
        bool image    = false;
        bool entities = false;
        bool wsi      = false;
        bool gpu      = false;
    };

    class STORMKIT_LUA_API Engine {
      public:
        ~Engine() noexcept;

        Engine(const Engine&)                    = delete;
        auto operator=(const Engine&) -> Engine& = delete;

        Engine(Engine&& other) noexcept;
        auto operator=(Engine&& other) noexcept -> Engine&;

        auto lua_main() noexcept -> std::expected<void, std::string>;

        template<typename T>
        auto global_state(this T& self) noexcept -> decltype(auto);

        static auto create(const stdfs::path& file, Modules modules = {}) noexcept -> Engine;

      private:
        explicit Engine(Modules) noexcept;

        auto load(const stdfs::path&) noexcept -> void;

        sol::state       m_global_state;
        sol::load_result m_script;
    };
} // namespace stormkit::lua

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::lua {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::create(const stdfs::path& file, Modules modules) noexcept -> Engine {
        auto engine = Engine { std::move(modules) };
        engine.load(file);
        return engine;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    inline auto Engine::global_state(this T& self) noexcept -> decltype(auto) {
        return std::forward_like<T&>(self.m_global_state);
    }
} // namespace stormkit::lua
