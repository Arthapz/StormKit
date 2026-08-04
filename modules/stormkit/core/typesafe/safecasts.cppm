// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.typesafe.safecasts;

import std;

import stormkit.core.meta;
import stormkit.core.contract;
import stormkit.core.types;

export {
    namespace stormkit { inline namespace core {
        struct Underlying {};

        struct Empty {};

        struct Equal {};

        struct Error {};

        template<typename To>
        struct as_fn final {
            template<typename From>
                requires(not meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
            static constexpr auto operator()(From&&,
                                             const std::source_location& = std::source_location::
                                               current()) noexcept = delete ("As caster not defined for these types To, From!");

            template<meta::ShouldPassByValue From>
                requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From                        from,
                                             const std::source_location& location = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&>;

            template<meta::Not<meta::ShouldPassByValue> From>
                requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
            [[nodiscard]]
            static constexpr auto operator()(From&&                      from,
                                             const std::source_location& location = std::source_location::current()) noexcept
              -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&>;
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

            template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
                requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(First first, Second second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
                requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(const First& first, const Second& second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
                requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(const First& first, Second second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
                requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(First first, const Second& second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
                requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(First first, Second second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
                requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(const First& first, const Second& second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
                requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(const First& first, Second second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);

            template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
                requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
            [[nodiscard]]
            static constexpr auto operator()(First first, const Second& second) noexcept -> bool
                requires(meta::SameAs<T, Equal>);
        };

        template<typename T>
        inline constexpr auto is_cpo = is_fn<T> {};

        template<typename T, typename U>
        constexpr auto is(const U& value) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
        constexpr auto is(First first, Second second) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
        constexpr auto is(First first, Second&& second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
        constexpr auto is(const First& first, Second second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
        constexpr auto is(const First& first, Second&& second) noexcept -> bool;

        template<typename To, meta::ShouldPassByValue From>
            requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
        [[nodiscard]]
        constexpr auto unchecked_narrow(From from) noexcept -> To;

        template<typename To, meta::Not<meta::ShouldPassByValue> From>
            requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
        [[nodiscard]]
        constexpr auto unchecked_narrow(const From& from) noexcept -> To;

        template<typename To, meta::ShouldPassByValue From>
            requires(meta::SameAs<To, From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From value, const std::source_location&) noexcept -> To;

        template<typename To, meta::Not<meta::ShouldPassByValue> From>
            requires(meta::SameAs<To, meta::ToPlainType<From>>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> meta::ForwardLike<From, To>;

        ////////////////////////////////////////////////////////////////////
        ///                      PREDICATE                               ///
        ////////////////////////////////////////////////////////////////////
        template<meta::ShouldPassByValue First, meta::IsUnaryPredicate<First> Predicate>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First value, Predicate&& predicate) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::IsUnaryPredicate<First> Predicate>
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& value, Predicate&& predicate) noexcept -> bool;

        ////////////////////////////////////////////////////////////////////
        ///                      ARITHMETIC                              ///
        ////////////////////////////////////////////////////////////////////
        template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
            requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First first, Second second) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
            requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First first, const Second& second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
            requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& first, Second second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
            requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& first, const Second& second) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
            requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First first, Second second) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
            requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First first, const Second& second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
            requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& first, Second second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
            requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& first, const Second& second) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
            requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First first, Second second) noexcept -> bool;

        template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
            requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, First first, const Second& second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
            requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& first, Second second) noexcept -> bool;

        template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
            requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
        [[nodiscard]]
        constexpr auto tag_invoke(is_fn<Equal>, const First& first, const Second& second) noexcept -> bool;

        template<meta::IsArithmetic To, meta::IsArithmetic From>
            requires(meta::ShouldPassByValue<From> and not meta::SameAs<To, From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From value, const std::source_location&) noexcept -> To;

        template<meta::IsArithmetic To, meta::IsArithmetic From>
            requires(not meta::ShouldPassByValue<From> and not meta::SameAs<To, From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, const From& value, const std::source_location&) noexcept -> To;

        ////////////////////////////////////////////////////////////////////
        ///                           BYTES                              ///
        ////////////////////////////////////////////////////////////////////
        template<meta::IsArithmetic From>
            requires(meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<byte>, From value, const std::source_location&) noexcept -> byte;

        template<meta::IsArithmetic From>
            requires(not meta::ShouldPassByValue<meta::ToPlainType<From>>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<byte>, From&& value, const std::source_location&) noexcept -> byte;

        template<meta::IsArithmetic To>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, byte value, const std::source_location&) noexcept -> To;

        template<meta::IsEnumeration From>
            requires(meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<byte>, From value, const std::source_location&) noexcept -> byte;

        template<meta::IsEnumeration From>
            requires(not meta::ShouldPassByValue<meta::ToPlainType<From>>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<byte>, From&& value, const std::source_location&) noexcept -> byte;

        template<meta::IsEnumeration To>
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, byte value, const std::source_location&) noexcept -> To;

        ////////////////////////////////////////////////////////////////////
        ///                       ENUMERATION                            ///
        ////////////////////////////////////////////////////////////////////
        template<meta::IsArithmetic To, meta::IsEnumeration From>
            requires(meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From value, const std::source_location&) noexcept -> To;

        template<meta::IsArithmetic To, meta::IsEnumeration From>
            requires(not meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, const From& value, const std::source_location&) noexcept -> To;

        template<meta::IsEnumeration From>
            requires(meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<Underlying>, From value, const std::source_location&) noexcept
          -> std::underlying_type_t<From>;

        template<meta::IsEnumeration From>
            requires(not meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<Underlying>, const From& value, const std::source_location&) noexcept
          -> std::underlying_type_t<From>;

        template<meta::IsEnumeration To, meta::IsArithmetic From>
            requires(meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, From value, const std::source_location&) noexcept -> To;

        template<meta::IsEnumeration To, meta::IsArithmetic From>
            requires(not meta::ShouldPassByValue<From>)
        [[nodiscard]]
        constexpr auto tag_invoke(as_fn<To>, const From& value, const std::source_location&) noexcept -> To;

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
    template<meta::ShouldPassByValue From>
        requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From from, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, from, location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To>
    template<meta::Not<meta::ShouldPassByValue> From>
        requires(meta::Is_tag_invocable<as_fn<To>, From, const std::source_location&>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_fn<To>::operator()(From&& from, const std::source_location& location) noexcept
      -> meta::tag_invoke_result<as_fn<To>, From, const std::source_location&> {
        return meta::tag_invoke_cpo(as_fn<To> {}, std::forward<From>(from), location);
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
    template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
        requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(First first, Second second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
        requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(const First& first, const Second& second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
        requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(const First& first, Second second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
        requires(not meta::Is_tag_invocable<is_fn<T>, First, Second> and meta::HasEqualityOperator<First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(First first, const Second& second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
        requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(First first, Second second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return meta::tag_invoke_cpo(is_fn<T> {}, first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(const First& first, const Second& second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return meta::tag_invoke_cpo(is_fn<T> {}, first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
        requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(const First& first, Second second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return meta::tag_invoke_cpo(is_fn<T> {}, first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::Is_tag_invocable<is_fn<T>, First, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto is_fn<T>::operator()(First first, const Second& second) noexcept -> bool
        requires(meta::SameAs<T, Equal>)
    {
        return meta::tag_invoke_cpo(is_fn<T> {}, first, second);
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
    template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
    constexpr auto is(First first, Second second) noexcept -> bool {
        return is_cpo<Equal>(first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
    constexpr auto is(First first, Second&& second) noexcept -> bool {
        return is_cpo<Equal>(first, std::forward<Second>(second));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
    constexpr auto is(const First& first, Second second) noexcept -> bool {
        return is_cpo<Equal>(first, second);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
    constexpr auto is(const First& first, Second&& second) noexcept -> bool {
        return is_cpo<Equal>(first, std::forward<Second>(second));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::ShouldPassByValue From>
        requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto unchecked_narrow(From from) noexcept -> To {
        return static_cast<To>(from);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::Not<meta::ShouldPassByValue> From>
        requires(meta::IsNarrowing<To, From> or (meta::IsArithmetic<To> and meta::IsArithmetic<From>))
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto unchecked_narrow(const From& from) noexcept -> To {
        return static_cast<To>(from);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::ShouldPassByValue From>
        requires(meta::SameAs<To, From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto tag_invoke(as_fn<To>, From value, const std::source_location&) noexcept -> To {
        return value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, meta::Not<meta::ShouldPassByValue> From>
        requires(meta::SameAs<To, meta::ToPlainType<From>>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, From&& value, const std::source_location&) noexcept -> meta::ForwardLike<From, To> {
        return std::forward<From>(value);
    }

    ////////////////////////////////////////////////////////////////////
    ///                      PREDICATE                               ///
    ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::IsUnaryPredicate<First> Predicate>
        STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<Equal>, First value, Predicate&& predicate) noexcept -> bool {
        return std::forward<Predicate>(predicate)(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::IsUnaryPredicate<First> Predicate>
        STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<Equal>, const First& value, Predicate&& predicate) noexcept -> bool {
        return std::forward<Predicate>(predicate)(value);
    }

    ////////////////////////////////////////////////////////////////////
    ///                      ARITHMETIC                              ///
    ////////////////////////////////////////////////////////////////////
    static constexpr auto
      NARROWING_ERROR_MSG = "Lossy narrowing detected, use unchecked_narrow() to explicitly force the conversion and possibly "
                            "lose information!";

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
        requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First first, Second second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First first, const Second& second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
        requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, const First& first, Second second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::IsFloatingPoint<First> and meta::IsFloatingPoint<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, const First& first, const Second& second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
        requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First first, Second second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First first, const Second& second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
        requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, const First& first, Second second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::IsFloatingPoint<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, const First& first, const Second& second) noexcept -> bool {
        const auto second_ = as<First>(second);

        const auto diff = std::abs(first - second_);
        const auto a    = std::abs(first);
        const auto b    = std::abs(second_);
        const auto ab   = (a > b) ? b : a;
        return diff <= (ab * std::numeric_limits<First>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::ShouldPassByValue Second>
        requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First first, Second second) noexcept -> bool {
        if constexpr (meta::IsNarrowing<First, Second>) {
            using SafeT        = meta::SafeNarrowHelperType<First, Second>;
            const auto _first  = as<SafeT>(first);
            const auto _second = as<SafeT>(second);
            return _first == _second;
        } else
            return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ShouldPassByValue First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, First first, const Second& second) noexcept -> bool {
        if constexpr (meta::IsNarrowing<First, Second>) {
            using SafeT        = meta::SafeNarrowHelperType<First, Second>;
            const auto _first  = as<SafeT>(first);
            const auto _second = as<SafeT>(second);
            return _first == _second;
        } else
            return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::ShouldPassByValue Second>
        requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, const First& first, Second second) noexcept -> bool {
        if constexpr (meta::IsNarrowing<First, Second>) {
            using SafeT        = meta::SafeNarrowHelperType<First, Second>;
            const auto _first  = as<SafeT>(first);
            const auto _second = as<SafeT>(second);
            return _first == _second;
        } else
            return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Not<meta::ShouldPassByValue> First, meta::Not<meta::ShouldPassByValue> Second>
        requires(meta::IsIntegral<First> and meta::IsIntegral<Second>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(is_fn<Equal>, const First& first, const Second& second) noexcept -> bool {
        if constexpr (meta::IsNarrowing<First, Second>) {
            using SafeT        = meta::SafeNarrowHelperType<First, Second>;
            const auto _first  = as<SafeT>(first);
            const auto _second = as<SafeT>(second);
            return _first == _second;
        } else
            return first == second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic To, meta::IsArithmetic From>
        requires(meta::ShouldPassByValue<From> and not meta::SameAs<To, From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, From value, [[maybe_unused]] const std::source_location& location) noexcept -> To {
        if constexpr (meta::IsNarrowing<To, From>) expects(details::is_safe_narrowing<To>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic To, meta::IsArithmetic From>
        requires(not meta::ShouldPassByValue<From> and not meta::SameAs<To, From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, const From& value, [[maybe_unused]] const std::source_location& location) noexcept
      -> To {
        if constexpr (meta::IsNarrowing<To, From>) expects(details::is_safe_narrowing<To>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
    }

    ////////////////////////////////////////////////////////////////////
    ///                      BYTES                                   ///
    ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic From>
        requires(meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<byte>, From value, const std::source_location& location) noexcept -> byte {
        if constexpr (meta::IsNarrowing<byte, From>)
            expects(details::is_safe_narrowing<byte>(value), NARROWING_ERROR_MSG, location);

        return static_cast<byte>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic From>
        requires(not meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<byte>, const From& value, const std::source_location& location) noexcept -> byte {
        if constexpr (meta::IsNarrowing<byte, From>)
            expects(details::is_safe_narrowing<byte>(value), NARROWING_ERROR_MSG, location);

        return static_cast<byte>(value);
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
    template<meta::IsEnumeration From>
        requires(meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<byte>, From value, const std::source_location& location) noexcept -> byte {
        if constexpr (meta::IsNarrowing<byte, From>)
            expects(details::is_safe_narrowing<byte>(value), NARROWING_ERROR_MSG, location);

        return static_cast<byte>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration From>
        requires(not meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<byte>, const From& value, const std::source_location& location) noexcept -> byte {
        if constexpr (meta::IsNarrowing<byte, From>)
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
    template<meta::IsArithmetic To, meta::IsEnumeration From>
        requires(meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, From value, [[maybe_unused]] const std::source_location& location) noexcept -> To {
        if constexpr (meta::IsNarrowing<To, From>) expects(details::is_safe_narrowing<To>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsArithmetic To, meta::IsEnumeration From>
        requires(not meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<To>, const From& value, [[maybe_unused]] const std::source_location& location) noexcept
      -> To {
        if constexpr (meta::IsNarrowing<To, From>) expects(details::is_safe_narrowing<To>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration From>
        requires(meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<Underlying>, From value, [[maybe_unused]] const std::source_location&) noexcept
      -> std::underlying_type_t<From> {
        return std::to_underlying(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration From>
        requires(not meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto tag_invoke(as_fn<Underlying>, const From& value, [[maybe_unused]] const std::source_location&) noexcept
      -> std::underlying_type_t<From> {
        return std::to_underlying(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration To, meta::IsArithmetic From>
        requires(meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>, From value, const std::source_location& location) noexcept -> To {
        if constexpr (meta::IsNarrowing<std::underlying_type_t<To>, From>)
            expects(details::is_safe_narrowing<std::underlying_type_t<To>>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration To, meta::IsArithmetic From>
        requires(not meta::ShouldPassByValue<From>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>, const From& value, const std::source_location& location) noexcept -> To {
        if constexpr (meta::IsNarrowing<std::underlying_type_t<To>, From>)
            expects(details::is_safe_narrowing<std::underlying_type_t<To>>(value), NARROWING_ERROR_MSG, location);

        return static_cast<To>(value);
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
