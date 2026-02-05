// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:typesafe.checked_value;

import std;

import :meta;

import :typesafe.integer;
import :typesafe.floating_point;

import :math.arithmetic;

import :utils.contract;

namespace stormkit { inline namespace core {
    struct PositiveTag;
    struct NegativeTag;

    namespace meta {
        template<typename T>
        concept IsCheckedValueArithmetic = meta::IsArithmetic<meta::UnderlyingType<meta::ToPlainType<T>>>;

        template<typename T, typename U>
        concept IsCheckedValueValueType = meta::PlainIs<T, meta::UnderlyingType<meta::ToPlainType<U>>>;
    } // namespace meta
}} // namespace stormkit::core

export namespace stormkit { inline namespace core {
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    struct CheckedValue final {
        using ValueType  = T;
        using value_type = ValueType;

        template<meta::PlainIs<T> U>
        constexpr CheckedValue(U&& _value) noexcept;

        template<meta::PlainIs<T> U>
        constexpr auto operator=(U&& _value) noexcept -> CheckedValue&;

        constexpr operator T() noexcept;

        constexpr auto operator+(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator+(meta::PlainIs<CheckedValue> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator+=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator+=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator-(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator-(meta::PlainIs<CheckedValue> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator-=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator-=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator*(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator*(meta::PlainIs<CheckedValue> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator*=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator*=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator/(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator/(meta::PlainIs<CheckedValue> auto&& other) const noexcept -> CheckedValue
            requires(meta::IsArithmetic<T>);

        constexpr auto operator/=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        constexpr auto operator/=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
            requires(meta::IsArithmetic<T>);

        T value;
    };

    namespace meta {
        template<typename T>
        concept IsCheckedValue = IsSpecializationOfNTTP_TTV<ToPlainType<T>, CheckedValue>;
    }

    template<meta::IsArithmetic T>
    using Positive = CheckedValue<T, PositiveTag, math::is_positive<T>>;

    template<meta::IsArithmetic T>
    using Negative = CheckedValue<T, NegativeTag, math::is_negative<T>>;

    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    constexpr auto operator+(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T>;

    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    constexpr auto operator-(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T>;

    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    constexpr auto operator*(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T>;

    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    constexpr auto operator/(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T>;

    template<meta::IsFormattable T, meta::IsPlain Tag, auto check_fn, typename FormatContext>
    constexpr auto format_as(const CheckedValue<T, Tag, check_fn>& val, FormatContext& ctx) -> decltype(ctx.out());
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    template<meta::PlainIs<T> U>
    STORMKIT_FORCE_INLINE
    constexpr CheckedValue<T, Tag, check_fn>::CheckedValue(U&& _value) noexcept
        : value { std::forward<U>(_value) } {
        EXPECTS(check_fn(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    template<meta::PlainIs<T> U>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator=(U&& _value) noexcept -> CheckedValue& {
        value = _value;
        ENSURES(check_fn(value));
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr CheckedValue<T, Tag, check_fn>::operator T() noexcept {
        return value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator+(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return { value + std::forward<decltype(other)>(other) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator+(meta::PlainIs<CheckedValue> auto&& other) const noexcept
      -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return operator+(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator+=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        value += std::forward<decltype(other)>(other);
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator+=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        return operator+=(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator-(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return { value - std::forward<decltype(other)>(other) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator-(meta::PlainIs<CheckedValue> auto&& other) const noexcept
      -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return operator-(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator-=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        value -= std::forward<decltype(other)>(other);
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator-=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        return operator-=(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator*(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return { value * std::forward<decltype(other)>(other) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator*(meta::PlainIs<CheckedValue> auto&& other) const noexcept
      -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return operator*(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator*=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        value *= std::forward<decltype(other)>(other);
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator*=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        return operator*=(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator/(meta::PlainIs<ValueType> auto&& other) const noexcept -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return { value / std::forward<decltype(other)>(other) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto CheckedValue<T, Tag, check_fn>::operator/(meta::PlainIs<CheckedValue> auto&& other) const noexcept
      -> CheckedValue
        requires(meta::IsArithmetic<T>)
    {
        return operator/(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator/=(meta::PlainIs<ValueType> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        value /= std::forward<decltype(other)>(other);
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPlain T, meta::IsPlain Tag, auto check_fn>
    STORMKIT_FORCE_INLINE
    constexpr auto CheckedValue<T, Tag, check_fn>::operator/=(meta::PlainIs<CheckedValue> auto&& other) noexcept -> CheckedValue&
        requires(meta::IsArithmetic<T>)
    {
        return operator/=(std::forward_like<decltype(other)>(other.value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto operator+(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T> {
        return std::forward<T>(second).operator+(std::forward<decltype(first)>(first));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto operator-(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T> {
        return std::forward<T>(second).operator-(std::forward<decltype(first)>(first));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto operator*(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T> {
        return std::forward<T>(second).operator*(std::forward<decltype(first)>(first));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsCheckedValue T>
        requires(meta::IsCheckedValueArithmetic<T>)
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto operator/(meta::IsCheckedValueValueType<T> auto&& first, T&& second) noexcept -> meta::ToPlainType<T> {
        return std::forward<T>(second).operator/(std::forward<decltype(first)>(first));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsFormattable T, meta::IsPlain Tag, auto check_fn, typename FormatContext>
        STORMKIT_FORCE_INLINE
    constexpr auto format_as(const CheckedValue<T, Tag, check_fn>& val, FormatContext& ctx) -> decltype(ctx.out()) {
        auto&& out = ctx.out();
        return std::format_to(out, "{}", val.value);
    }

#ifndef STORMKIT_OS_WINDOWS
    #undef STORMKIT_CORE_API
    #define STORMKIT_CORE_API
#endif

#define INSTANCIATE(t)                                                                    \
    template struct STORMKIT_CORE_API CheckedValue<t, PositiveTag, math::is_positive<t>>; \
    template struct STORMKIT_CORE_API CheckedValue<t, NegativeTag, math::is_positive<t>>

    INSTANCIATE(u8);
    INSTANCIATE(i8);
    INSTANCIATE(u16);
    INSTANCIATE(i16);
    INSTANCIATE(u32);
    INSTANCIATE(i32);
    INSTANCIATE(u64);
    INSTANCIATE(i64);
    INSTANCIATE(u128);
    INSTANCIATE(i128);
    INSTANCIATE(f32);
    INSTANCIATE(f64);

#undef INSTANCIATE
}} // namespace stormkit::core
