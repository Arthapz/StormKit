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
        using InitUserLibrariesClosure = FunctionRef<void(sol::state&)>;
        ~Engine() noexcept;

        Engine(const Engine&)                    = delete;
        auto operator=(const Engine&) -> Engine& = delete;

        Engine(Engine&& other) noexcept;
        auto operator=(Engine&& other) noexcept -> Engine&;

        static auto run(stdfs::path              file,
                        Modules                  modules             = {},
                        InitUserLibrariesClosure init_user_libraries = monadic::noop()) noexcept -> Engine;

      private:
        Engine() noexcept;

        auto load(stdfs::path&&, Modules&&, InitUserLibrariesClosure&&) noexcept -> void;

        auto init_libraries(Modules&&, sol::state&) noexcept -> void;
    };

    template<typename... Args>
    auto luacall(const sol::protected_function& function, Args&&... args) noexcept -> decltype(function());
} // namespace stormkit::lua

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::lua {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Engine::Engine() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Engine::Engine(Engine&& other) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::operator=(Engine&& other) noexcept -> Engine& = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Engine::~Engine() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::run(stdfs::path file, Modules modules, InitUserLibrariesClosure init_user_libraries) noexcept -> Engine {
        auto engine = Engine {};
        engine.load(std::move(file), std::move(modules), std::move(init_user_libraries));
        return engine;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    inline auto luacall(const sol::protected_function& function, Args&&... args) noexcept -> decltype(function()) {
        auto result = function(std::forward<Args>(args)...);
        if (not result.valid())
            ensures(false,
                    std::format("lua runtime error!\n-----------------------------------------\n{}",
                                sol::error { result }.what()));
        return result;
    }
} // namespace stormkit::lua
