// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.typesafe.safecasts;

import std;

import stormkit.core.contract;
import stormkit.core.meta.concepts;
import stormkit.core.meta.type_manipulation;
import stormkit.core.meta.tag_invoke;
import stormkit.core.meta.type_query;
import stormkit.core.meta.algorithms;
import stormkit.core.types;

export {
    namespace stormkit { inline namespace core {
        struct Underlying {};

        struct Empty {};

        struct Equal {};

        struct Error {};

        template<typename To>
        struct as_fn final {
            template<typename... Args>
                requires(not meta::Is_tag_invocable<as_fn<To>, Args...>)
            static constexpr auto operator()(Args&&...,
                                             const std::source_location& = std::source_location::
                                               current()) noexcept = delete ("As caster not defined for these types To, From!");

            /*





            */

            template<meta::arg::ShouldPassByValue From>
                requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From from, const std::source_location& = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&>;

            template<meta::arg::ShouldPassByRef From>
                requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From&& from, const std::source_location& = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&>;

            /*





            */

            template<meta::arg::ShouldPassByValue From, meta::arg::ShouldPassByValue Arg1>
                requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From from,
                                             Arg1 arg1,
                                             const std::source_location& = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&>;

            template<meta::arg::ShouldPassByValue From, meta::arg::ShouldPassByRef Arg1>
                requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From   from,
                                             Arg1&& arg1,
                                             const std::source_location& = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&>;

            /*





            */

            template<meta::arg::ShouldPassByRef From, meta::arg::ShouldPassByValue Arg1>
                requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From&& from,
                                             Arg1   arg1,
                                             const std::source_location& = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&>;

            template<meta::arg::ShouldPassByRef From, meta::arg::ShouldPassByRef Arg1>
                requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From&& from,
                                             Arg1&& arg1,
                                             const std::source_location& = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&>;
        };

        template<typename To>
        inline constexpr auto as = as_fn<To> {};

        template<typename T>
        struct is_fn final {
            template<typename... Args>
                requires(not meta::Is_tag_invocable<is_fn<T>, Args...>)
            static constexpr auto operator()(Args&&...) noexcept = delete ("Is queryier not defined for these types To, From!");

            template<typename U>
                requires(meta::Is_tag_invocable<is_fn<T>, U>)
            [[nodiscard]]
            static constexpr auto operator()(const U& from) noexcept -> bool;

            template<typename First, typename Second>
                requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(First&& first, Second&& second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<typename First, typename Second>
                requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(First&& first, Second&& second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);
        };

        template<typename T>
        inline constexpr auto is_cpo = is_fn<T> {};

        template<typename T, typename U>
        constexpr auto is(const U& value) noexcept -> bool;

        template<meta::arg::ShouldPassByValue First, meta::arg::ShouldPassByValue Second>
        constexpr auto is(First first, Second second) noexcept -> bool;

        template<meta::arg::ShouldPassByValue First, meta::arg::ShouldPassByRef Second>
        constexpr auto is(First first, Second&& second) noexcept -> bool;

        template<meta::arg::ShouldPassByRef First, meta::arg::ShouldPassByValue Second>
        constexpr auto is(const First& first, Second second) noexcept -> bool;

        template<meta::arg::ShouldPassByRef First, meta::arg::ShouldPassByRef Second>
        constexpr auto is(const First& first, Second&& second) noexcept -> bool;

        template<typename To, meta::arg::ShouldPassByValue From>
            requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
        [[nodiscard]]
        constexpr auto unchecked_narrow(From from) noexcept -> To;

        template<typename To, meta::arg::ShouldPassByRef From>
            requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
        [[nodiscard]]
        constexpr auto unchecked_narrow(const From& from) noexcept -> To;

        template<typename To, meta::arg::PlainTypeTo<meta::SameAs, To> From>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> meta::ForwardLike<From, To>;

        ////////////////////////////////////////////////////////////////////
        ///                      PREDICATE                               ///
        ////////////////////////////////////////////////////////////////////
        template<typename First, meta::arg::PlainTypeTo<meta::IsUnaryPredicate, First> Predicate>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First&& value, Predicate&& predicate) noexcept -> bool;

        ////////////////////////////////////////////////////////////////////
        ///                      ARITHMETIC                              ///
        ////////////////////////////////////////////////////////////////////
        template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> First, meta::arg::PlainTypeTo<meta::IsFloatingPoint> Second>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First&& first, Second&& second) noexcept -> bool;

        template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> First, meta::arg::PlainTypeTo<meta::IsIntegral> Second>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First&& first, Second&& second) noexcept -> bool;

        template<meta::arg::PlainTypeTo<meta::IsIntegral> First, meta::arg::PlainTypeTo<meta::IsIntegral> Second>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First&& first, Second&& second) noexcept -> bool;

        template<meta::IsArithmetic To, meta::arg::PlainTypeTo<meta::IsArithmetic> From>
            requires(not meta::PlainTypeTo<meta::SameAs, To, From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> To;

        ////////////////////////////////////////////////////////////////////
        ///                           BYTES                              ///
        ////////////////////////////////////////////////////////////////////
        template<meta::arg::PlainTypeTo<meta::IsArithmetic> From>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<byte>, From&& value, const std::source_location&) noexcept -> byte;

        template<meta::IsArithmetic To>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, byte value, const std::source_location&) noexcept -> To;

        template<meta::arg::PlainTypeTo<meta::IsEnumeration> From>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<byte>, From&& value, const std::source_location&) noexcept -> byte;

        template<meta::IsEnumeration To>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, byte value, const std::source_location&) noexcept -> To;

        ////////////////////////////////////////////////////////////////////
        ///                       ENUMERATION                            ///
        ////////////////////////////////////////////////////////////////////
        template<meta::IsArithmetic To, meta::arg::PlainTypeTo<meta::IsEnumeration> From>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> To;

        template<meta::arg::PlainTypeTo<meta::IsEnumeration> From>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<Underlying>, From&& value, const std::source_location&) noexcept
          -> std::underlying_type_t<From>;

        template<meta::IsEnumeration To, meta::arg::PlainTypeTo<meta::IsArithmetic> From>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> To;

        ////////////////////////////////////////////////////////////////////
        ///                          STL                                 ///
        ////////////////////////////////////////////////////////////////////
        template<typename T, meta::IsStdVariant Variant>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<T>, Variant variant) noexcept -> bool;

        template<typename To, typename From>
            requires(meta::IsStdVariant<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& variant, const std::source_location&) noexcept -> meta::ForwardLike<From, To>;

        template<typename T, meta::IsStdOptional Optional>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<T>, Optional optional) noexcept -> bool;

        template<typename To, typename From>
            requires(meta::IsStdOptional<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& optional, const std::source_location&) noexcept
          -> meta::ForwardLike<From, To>;

        template<meta::SameAs<Empty> T, meta::IsStdExpected Expected>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<T>, Expected expected) noexcept -> bool;

        template<meta::SameAs<Error> T, meta::IsStdExpected Expected>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<T>, Expected expected) noexcept -> bool;

        template<typename T, meta::IsStdExpected Expected>
            requires(meta::SameAs<T, meta::ValueType<Expected>>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<T>, Expected expected) noexcept -> bool;

        template<typename T, meta::IsStdExpected Expected>
            requires(meta::SameAs<T, std::unexpected<typename Expected::error_type>>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<T>, Expected expected) noexcept -> bool;

        template<typename To, typename Expected>
            requires(meta::IsStdExpected<Expected> and meta::SameAs<To, meta::ValueType<Expected>>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, Expected&& expected, const std::source_location&) noexcept
          -> meta::ForwardLike<Expected, To>;

        template<typename To, typename Expected>
            requires(meta::IsStdExpected<Expected> and meta::SameAs<To, std::unexpected<typename Expected::error_type>>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, Expected&& expected, const std::source_location&) noexcept
          -> meta::ForwardLike<Expected, To>;
    }} // namespace stormkit::core
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    namespace details {
        /////////////////////////////////////
        /////////////////////////////////////
        template<typename To, typename From>
            requires((meta::IsArithmetic<To> or meta::Isbyte<To> or meta::IsEnumeration<To>)
                     and (meta::IsArithmetic<From> or meta::Isbyte<From> or meta::IsEnumeration<From>))
        STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
        constexpr auto is_safe_narrowing(const From& from) noexcept -> bool {
            if constexpr (meta::IsArithmetic<From> and meta::IsArithmetic<To>)
                return (static_cast<From>(static_cast<To>(from)) == from)
                       or (meta::IsSigned<To> != meta::IsUnsigned<From>
                           and ((static_cast<To>(from) < To {}) == (from < From {})));
            else
                return (static_cast<From>(static_cast<To>(from)) == from);
        }
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::arg::ShouldPassByValue From>
        requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From from, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, auto { from }, location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::arg::ShouldPassByRef From>
        requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From&& from, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, std::forward<From>(from), location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::arg::ShouldPassByValue From, meta::arg::ShouldPassByValue Arg1>
        requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From from, Arg1 arg1, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, auto { from }, auto { arg1 }, location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::arg::ShouldPassByValue From, meta::arg::ShouldPassByRef Arg1>
        requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From from, Arg1&& arg1, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, auto { from }, std::forward<Arg1>(arg1), location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::arg::ShouldPassByRef From, meta::arg::ShouldPassByValue Arg1>
        requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From&& from, Arg1 arg1, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, std::forward<From>(from), auto { arg1 }, location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::arg::ShouldPassByRef From, meta::arg::ShouldPassByRef Arg1>
        requires(meta::Is_tag_invocable<as_fn<To>, From, Arg1, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From&& from, Arg1&& arg1, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, Arg1, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, std::forward<From>(from), std::forward<Arg1>(arg1), location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename U>
        requires(meta::Is_tag_invocable<is_fn<T>, U>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(const U& from) noexcept -> bool {
        return meta::tag_invoke_cpo(is_fn<T> {}, from);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename First, typename Second>
        requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(First&& first, Second&& second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename First, typename Second>
        requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(First&& first, Second&& second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return meta::tag_invoke_cpo(is_fn<T> {}, std::forward<First>(first), std::forward<Second>(second));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename U>
    STORMKIT_FORCE_INLINE
    constexpr auto is(const U& value) noexcept -> bool {
        return is_cpo<T>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::ShouldPassByValue First, meta::arg::ShouldPassByValue Second>
    constexpr auto is(First first, Second second) noexcept -> bool {
        return is_cpo<Equal>(first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::ShouldPassByValue First, meta::arg::ShouldPassByRef Second>
    constexpr auto is(First first, Second&& second) noexcept -> bool {
        return is_cpo<Equal>(first, std::forward<Second>(second));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::ShouldPassByRef First, meta::arg::ShouldPassByValue Second>
    constexpr auto is(const First& first, Second second) noexcept -> bool {
        return is_cpo<Equal>(first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::ShouldPassByRef First, meta::arg::ShouldPassByRef Second>
    constexpr auto is(const First& first, Second&& second) noexcept -> bool {
        return is_cpo<Equal>(first, std::forward<Second>(second));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::arg::ShouldPassByValue From>
        requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto unchecked_narrow(From from) noexcept -> To {
        return static_cast<To>(from);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::arg::ShouldPassByRef From>
        requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto unchecked_narrow(const From& from) noexcept -> To {
        return static_cast<To>(from);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::arg::PlainTypeTo<meta::SameAs, To> From>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> meta::ForwardLike<From, To> {
        return std::forward<From>(value);
    }

    ////////////////////////////////////////////////////////////////////
    ///                      PREDICATE                               ///
    ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename First, meta::arg::PlainTypeTo<meta::IsUnaryPredicate, First> Predicate>
        STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<Equal>, First&& value, Predicate&& predicate) noexcept -> bool {
        return std::forward<Predicate>(predicate)(std::forward<First>(value));
    }

    ////////////////////////////////////////////////////////////////////
    ///                      ARITHMETIC                              ///
    ////////////////////////////////////////////////////////////////////
    static constexpr auto
      NARROWING_ERROR_MSG = "Lossy narrowing detected, use unchecked_narrow() to explicitly force the conversion and possibly "
                            "lose information!";

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> First, meta::arg::PlainTypeTo<meta::IsFloatingPoint> Second>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First&& first, Second&& second) noexcept -> bool {
        using First_  = meta::ToPlainType<First>;
        using Second_ = meta::ToPlainType<Second>;

        const auto second_ = as<First_>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First_>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> First, meta::arg::PlainTypeTo<meta::IsIntegral> Second>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal> tag, First&& first, Second&& second) noexcept -> bool {
        return tag_invoke(tag, std::forward<First>(first), as<First>(std::forward<Second>(second)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsIntegral> First, meta::arg::PlainTypeTo<meta::IsIntegral> Second>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First&& first, Second&& second) noexcept -> bool {
        using First_  = meta::ToPlainType<First>;
        using Second_ = meta::ToPlainType<Second>;

        if constexpr (meta::IsNarrowing<First_, Second_>) {
            using SafeT        = meta::SafeNarrowHelperType<First_, Second_>;
            const auto _first  = as<SafeT>(first);
            const auto _second = as<SafeT>(second);
            return _first == _second;
        } else
            return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic To, meta::arg::PlainTypeTo<meta::IsArithmetic> From>
        requires(not meta::PlainTypeTo<meta::SameAs, To, From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, From&& value, [[maybe_unused]] const std::source_location& location) noexcept -> To {
        using From_ = meta::ToPlainType<From>;

        if constexpr (meta::IsNarrowing<To, From_>) expects(details::is_safe_narrowing<To>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
    }

    ////////////////////////////////////////////////////////////////////
    ///                      BYTES                                   ///
    ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsArithmetic> From>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<byte>, From&& value, const std::source_location& location) noexcept -> byte {
        using From_ = meta::ToPlainType<From>;

        if constexpr (meta::IsNarrowing<byte, From_>)
            expects(details::is_safe_narrowing<byte>(value), NARROWING_ERROR_MSG, location);

        return static_cast<byte>(std::forward<From>(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic To>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, byte value, const std::source_location&) noexcept -> To {
        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsEnumeration> From>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<byte>, From&& value, const std::source_location& location) noexcept -> byte {
        using From_ = meta::ToPlainType<From>;

        if constexpr (meta::IsNarrowing<byte, From_>)
            expects(details::is_safe_narrowing<byte>(value), NARROWING_ERROR_MSG, location);

        return static_cast<byte>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration To>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, byte value, const std::source_location&) noexcept -> To {
        return static_cast<To>(value);
    }

    ////////////////////////////////////////////////////////////////////
    ///                       ENUMERATION                            ///
    ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic To, meta::arg::PlainTypeTo<meta::IsEnumeration> From>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, From&& value, [[maybe_unused]] const std::source_location& location) noexcept -> To {
        using From_ = meta::ToPlainType<From>;

        if constexpr (meta::IsNarrowing<To, From_>) expects(details::is_safe_narrowing<To>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(std::forward<From>(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsEnumeration> From>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<Underlying>, From&& value, const std::source_location&) noexcept
      -> std::underlying_type_t<From> {
        return std::to_underlying(std::forward<From>(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration To, meta::arg::PlainTypeTo<meta::IsArithmetic> From>
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location& location) noexcept -> To {
        using From_ = meta::ToPlainType<From>;

        if constexpr (meta::IsNarrowing<std::underlying_type_t<To>, From_>)
            expects(details::is_safe_narrowing<std::underlying_type_t<To>>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(std::forward<From>(value));
    }

    ////////////////////////////////////////////////////////////////////
    ///                          STL                                 ///
    ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::IsStdVariant Variant>
            STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<T>, const Variant& variant) noexcept -> bool {
        if constexpr (meta::SameAs<T, Empty>) {
            if constexpr (requires { variant_contains_type<std::monostate>(std::declval<Variant>()) == true; })
                return std::get_if<std::monostate>(&variant) != nullptr;
            else
                return false;
        } else {
            return meta::variant_type_find_if(variant,
                                              [&variant]<typename It>(const It&) noexcept -> bool {
                                                  if (variant.index() == It::INDEX)
                                                      return meta::SameAs<T, std::variant_alternative_t<It::INDEX, Variant>>;
                                                  return false;
                                              })
                   != std::variant_npos;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename From>
        requires(meta::IsStdVariant<From>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, From&& variant, const std::source_location& location) noexcept
      -> meta::ForwardLike<From, To> {
        auto ptr = raw_ptr<meta::ForwardConst<From, To>> { nullptr };
        meta::variant_type_find_if(std::forward<From>(variant), [&ptr, &variant]<typename It>(const It&) noexcept -> bool {
            if constexpr (meta::SameAs<typename It::type, To>) {
                if (std::forward<From>(variant).index() == It::INDEX) ptr = &std::get<It::index>(variant);
                return true;
            }

            return false;
        });

        ensures(ptr != nullptr, "Bad variant access!", location);
        return std::forward_like<From>(*ptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::IsStdOptional Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<T>, const Optional& optional) noexcept -> bool {
        if (not optional.has_value()) return meta::SameAs<T, Empty>;
        return meta::Is<T, meta::ValueType<Optional>>;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename Optional>
        requires(meta::IsStdOptional<Optional>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, Optional&& optional, const std::source_location& location) noexcept
      -> meta::ForwardLike<Optional, To> {
        ensures(is<To>(optional), "Bad optional access!", location);

        return std::forward_like<Optional>(optional.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::SameAs<Empty> T, meta::IsStdExpected Expected>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<T>, const Expected& expected) noexcept -> bool {
        return not expected.has_value();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::SameAs<Error> T, meta::IsStdExpected Expected>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<T>, const Expected& expected) noexcept -> bool {
        return not expected.has_value();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::IsStdExpected Expected>
        requires(meta::SameAs<T, meta::ValueType<Expected>>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<T>, const Expected& expected) noexcept -> bool {
        return expected.has_value();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::IsStdExpected Expected>
        requires(meta::SameAs<T, std::unexpected<typename Expected::error_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<T>, const Expected& expected) noexcept -> bool {
        return not expected.has_value();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename Expected>
        requires(meta::IsStdExpected<Expected> and meta::SameAs<To, meta::ValueType<Expected>>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, Expected&& expected, const std::source_location& location) noexcept
      -> meta::ForwardLike<Expected, To> {
        ensures(expected.has_value(), "Bad expected access!", location);

        return std::forward_like<Expected>(expected.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename Expected>
        requires(meta::IsStdExpected<Expected> and meta::SameAs<To, std::unexpected<typename Expected::error_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, Expected&& expected, const std::source_location& location) noexcept
      -> meta::ForwardLike<Expected, To> {
        ensures(not expected.has_value(), "Bad expected access!", location);

        return std::forward_like<Expected>(expected.error());
    }
}} // namespace stormkit::core
