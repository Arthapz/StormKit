// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.functional.utils;

import std;

import stormkit.core.meta;
import stormkit.core.types;
import stormkit.core.typesafe;

namespace stormkit { inline namespace core { namespace details {
    struct EitherFunc {
        [[nodiscard]]
        static constexpr auto operator()(bool condition, std::invocable auto&& true_, std::invocable auto&& false_) noexcept
          -> decltype(false_());

        template<typename T>
        using ForwardArg = meta::forward_like<T, meta::ContainedType<meta::to_plain_type<T>>>;

        template<meta::pointer T>
        [[nodiscard]]
        static constexpr auto operator()(T                                            value,
                                         std::invocable<meta::pointed_type<T>&> auto&& true_,
                                         std::invocable auto&&                        false_) noexcept -> decltype(false_());

        template<meta::IsContainer T>
            requires(meta::convertible_to<bool, T>)
        [[nodiscard]]
        static constexpr auto operator()(T&&                                  value,
                                         std::invocable<ForwardArg<T>> auto&& true_,
                                         std::invocable auto&&                false_) noexcept -> decltype(false_());
    };
}}} // namespace stormkit::core::details

export namespace stormkit { inline namespace core {
    inline constexpr auto either = details::EitherFunc {};

    using std::bind_back;
    using std::bind_front;

    template<typename T, typename Func, typename... Ts>
        requires(std::invocable<Func, T&, Ts...> and meta::is<std::invoke_result_t<Func, T&, Ts...>, void>)
    [[nodiscard]]
    constexpr auto init_by(Func&& func, Ts&&... args) noexcept -> T;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    namespace details {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        constexpr auto EitherFunc::operator()(bool condition, std::invocable auto&& true_, std::invocable auto&& false_) noexcept
          -> decltype(false_()) {
            if (condition) return true_();
            return false_();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::pointer T>
        STORMKIT_FORCE_INLINE
        constexpr auto EitherFunc::operator()(T                                            value,
                                              std::invocable<meta::pointed_type<T>&> auto&& true_,
                                              std::invocable auto&& false_) noexcept -> decltype(false_()) {
            if (static_cast<bool>(value)) return true_(unref(value));
            return false_();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::IsContainer T>
            requires(meta::convertible_to<bool, T>)
        STORMKIT_FORCE_INLINE
        constexpr auto EitherFunc::operator()(T&&                                  value,
                                              std::invocable<ForwardArg<T>> auto&& true_,
                                              std::invocable auto&&                false_) noexcept -> decltype(false_()) {
            if (static_cast<bool>(value)) return true_(std::forward_like<T>(unref(value)));
            return false_();
        }
    } // namespace details

#if not(defined(__cpp_lib_bind_back) and __cpp_lib_bind_back >= 202306L)
    /////////////////////////////////////
    /////////////////////////////////////
    template<auto Func, typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto bind_back(Ts&&... args) noexcept -> decltype(auto) {
        return std::bind_back<Func>(std::forward<Ts>(args)...);
        using FuncType = decltype(Func);
        if constexpr (meta::pointer<FuncType> or std::is_member_pointer_v<FuncType>) static_assert(Func != nullptr);
        return
          [... bound_args(std::forward<Ts>(args))]<typename Self, typename... CallTs>(
            this Self&&,
            CallTs&&... call_args) noexcept(std::is_nothrow_invocable_v<FuncType,
                                                                          CallTs...,
                                                                          meta::forward_like<Self, meta::to_decayed_type<Ts>>...>)
            -> decltype(auto) {
              return std::invoke(Func, std::forward<CallTs>(call_args)..., std::forward_like<Self>(bound_args)...);
          };
    }
#endif

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Func, typename... Ts>
        requires(std::invocable<Func, T&, Ts...> and meta::is<std::invoke_result_t<Func, T&, Ts...>, void>)
    STORMKIT_CONST STORMKIT_FORCE_INLINE
    constexpr auto init_by(Func&& func, Ts&&... args) noexcept -> T {
        auto out = T {};
        std::invoke(std::forward<Func>(func), out, std::forward<Ts>(args)...);
        return out;
    }
}} // namespace stormkit::core
