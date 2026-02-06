// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/lua/lua.hpp>

module stormkit.lua;

import std;

import stormkit.core;

namespace stormkit::lua::core {
    namespace {
        ////////////////////////////////////////
        ////////////////////////////////////////
        template<typename T, typename... Constructors>
        auto _bind_extent(std::string_view name, auto& parent, auto... values) {
            auto metatable = parent.template new_usertype<T>(name, sol::constructors<T(), Constructors...> {});

            metatable[sol::meta_function::to_string] = &math::to_string<T>;
            metatable[sol::meta_function::equal_to]  = +[](const T& first, const T& second) static noexcept {
                return first == second;
            };

            metatable["rank"] = sol::var(std::cref(T::RANK));

            (
              [metatable, &values] mutable {
                  auto&& [k, v] = std::forward<decltype(values)>(values);
                  metatable[k]  = v;
              }(),
              ...);
        }

        ////////////////////////////////////////
        ////////////////////////////////////////
        template<typename T, typename... Constructors>
        auto _bind_vector(std::string_view name, auto& parent, auto... values) {
            auto metatable = parent.template new_usertype<T>(name, sol::constructors<T(), Constructors...> {});

            metatable[sol::meta_function::to_string] = +[](const T& value) static noexcept { return to_string(value); };
            // metatable[sol::meta_function::equal_to]  = +[](const T& first, const T& second) static noexcept {
            //     return first == second;
            // };

            (
              [metatable, &values] mutable {
                  auto&& [k, v] = std::forward<decltype(values)>(values);
                  metatable[k]  = v;
              }(),
              ...);
        }
    } // namespace

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto bind_extent(sol::table& metatable) noexcept -> void {
        _bind_extent<math::uextent2, math::uextent2(u32, u32)>("uextent2",
                                                               metatable,
                                                               std::pair { "width", &math::uextent2::width },
                                                               std::pair { "height", &math::uextent2::height });
        _bind_extent<math::uextent3, math::uextent3(u32, u32, u32)>("math::uextent3",
                                                                    metatable,
                                                                    std::pair { "width", &math::uextent3::width },
                                                                    std::pair { "height", &math::uextent3::height },
                                                                    std::pair { "depth", &math::uextent3::depth });
        _bind_extent<math::fextent2, math::fextent2(f32, f32)>("fextent2",
                                                               metatable,
                                                               std::pair { "width", &math::fextent2::width },
                                                               std::pair { "height", &math::fextent2::height });
        _bind_extent<math::fextent3, math::fextent3(f32, f32, f32)>("fextent3",
                                                                    metatable,
                                                                    std::pair { "width", &math::fextent3::width },
                                                                    std::pair { "height", &math::fextent3::height },
                                                                    std::pair { "depth", &math::fextent3::depth });
        _bind_extent<math::iextent2, math::iextent2(i32, i32)>("iextent2",
                                                               metatable,
                                                               std::pair { "width", &math::iextent2::width },
                                                               std::pair { "height", &math::iextent2::height });
        _bind_extent<math::iextent3, math::iextent3(i32, i32, i32)>("iextent3",
                                                                    metatable,
                                                                    std::pair { "width", &math::iextent3::width },
                                                                    std::pair { "height", &math::iextent3::height },
                                                                    std::pair { "depth", &math::iextent3::depth });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto bind_linear_vector(sol::table& metatable) noexcept -> void {
        _bind_vector<math::fvec2, math::fvec2(f32, f32)>("fvec2",
                                                         metatable,
                                                         std::pair { "x", &math::fvec2::x },
                                                         std::pair { "y", &math::fvec2::y });
        _bind_vector<math::fvec3, math::fvec3(f32, f32, f32)>("fvec3",
                                                              metatable,
                                                              std::pair { "x", &math::fvec3::x },
                                                              std::pair { "y", &math::fvec3::y },
                                                              std::pair { "z", &math::fvec3::z });
        // _bind_vector<math::fvec4, math::fvec4(f32, f32)>("fvec4",
        //                                                  metatable,
        //                                                  std::pair { "x", &math::fvec4::x },
        //                                                  std::pair { "y", &math::fvec4::y },
        //                                                  std::pair { "z", &math::fvec4::z },
        //                                                  std::pair { "w", &math::fvec4::w });
        _bind_vector<math::uvec2, math::uvec2(u32, u32)>("uvec2",
                                                         metatable,
                                                         std::pair { "x", &math::uvec2::x },
                                                         std::pair { "y", &math::uvec2::y });
        _bind_vector<math::uvec3, math::uvec3(u32, u32, u32)>("uvec3",
                                                              metatable,
                                                              std::pair { "x", &math::uvec3::x },
                                                              std::pair { "y", &math::uvec3::y },
                                                              std::pair { "z", &math::uvec3::z });
        // _bind_vector<math::uvec4, math::uvec4(u32, u32)>("uvec4",
        //                                                  metatable,
        //                                                  std::pair { "x", &math::uvec4::x },
        //                                                  std::pair { "y", &math::uvec4::y },
        //                                                  std::pair { "z", &math::uvec4::z },
        //                                                  std::pair { "w", &math::uvec4::w });
        _bind_vector<math::ivec2, math::ivec2(i32, i32)>("ivec2",
                                                         metatable,
                                                         std::pair { "x", &math::ivec2::x },
                                                         std::pair { "y", &math::ivec2::y });
        _bind_vector<math::ivec3, math::ivec3(i32, i32, i32)>("ivec3",
                                                              metatable,
                                                              std::pair { "x", &math::ivec3::x },
                                                              std::pair { "y", &math::ivec3::y },
                                                              std::pair { "z", &math::ivec3::z });
        // _bind_vector<math::ivec4, math::ivec4(i32, i32)>("ivec4",
        //                                                  metatable,
        //                                                  std::pair { "x", &math::ivec4::x },
        //                                                  std::pair { "y", &math::ivec4::y },
        //                                                  std::pair { "z", &math::ivec4::z },
        //                                                  std::pair { "w", &math::ivec4::w });

        metatable["add"] = sol::overload(&math::add<math::fvec2>,
                                         &math::add<math::fvec3>,
                                         // &math::add<math::fvec4>,
                                         &math::add<math::uvec2>,
                                         &math::add<math::uvec3>,
                                         // &math::add<math::uvec4>,
                                         &math::add<math::ivec2>,
                                         &math::add<math::ivec3>);
        // &math::add<math::ivec4>);
        metatable["sub"] = sol::overload(&math::sub<math::fvec2>,
                                         &math::sub<math::fvec3>,
                                         // &math::sub<math::fvec4>,
                                         &math::sub<math::uvec2>,
                                         &math::sub<math::uvec3>,
                                         // &math::sub<math::uvec4>,
                                         &math::sub<math::ivec2>,
                                         &math::sub<math::ivec3>);
        // &math::sub<math::ivec4>);
        metatable["mul"] = sol::overload(&math::mul<math::fvec2>,
                                         &math::mul<math::fvec3>,
                                         // &math::mul<math::fvec4>,
                                         &math::mul<math::uvec2>,
                                         &math::mul<math::uvec3>,
                                         // &math::mul<math::uvec4>,
                                         &math::mul<math::ivec2>,
                                         &math::mul<math::ivec3>);
        // &math::mul<math::ivec4>);
        metatable["div"] = sol::overload(&math::div<math::fvec2>,
                                         &math::div<math::fvec3>,
                                         // &math::div<math::fvec4>,
                                         &math::div<math::uvec2>,
                                         &math::div<math::uvec3>,
                                         // &math::div<math::uvec4>,
                                         &math::div<math::ivec2>,
                                         &math::div<math::ivec3>);
        // &math::div<math::ivec4>);
        metatable["dot"] = sol::overload(&math::dot<math::fvec2>,
                                         &math::dot<math::fvec3>,
                                         // &math::dot<math::fvec4>,
                                         &math::dot<math::uvec2>,
                                         &math::dot<math::uvec3>,
                                         // &math::dot<math::uvec4>,
                                         &math::dot<math::ivec2>,
                                         &math::dot<math::ivec3>);
        // &math::dot<math::ivec4>);
        metatable["cross"] = sol::overload(
          +[](const math::fvec3& first, const math::fvec3& second) static noexcept { return math::cross(first, second); },
          +[](const math::uvec3& first, const math::uvec3& second) static noexcept { return math::cross(first, second); },
          +[](const math::ivec3& first, const math::ivec3& second) static noexcept { return math::cross(first, second); });

        // &math::cross<math::ivec4>);
        metatable["normalize"] = sol::overload(&math::normalize<math::fvec2>,
                                               &math::normalize<math::fvec3>,
                                               // &math::normalize<math::fvec4>,
                                               &math::normalize<math::uvec2>,
                                               &math::normalize<math::uvec3>,
                                               // &math::normalize<math::uvec4>,
                                               &math::normalize<math::ivec2>,
                                               &math::normalize<math::ivec3>);
        // &math::normalize<math::ivec4>);
    }

    auto bind_math(sol::state& global_state) noexcept -> void {
        auto math_metatable = global_state["math"].get_or_create<sol::table>();
        bind_extent(math_metatable);
        bind_linear_vector(math_metatable);
    }
} // namespace stormkit::lua::core
