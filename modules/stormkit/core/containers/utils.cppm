// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:containers.utils;

import std;

import :meta;
import :typesafe.byte;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core {

    // template<template<class> typename Container = std::inplace_vector, typename T>
    template<template<class> typename Container = dyn_array, typename T>
    constexpr auto filled_with(usize size, T value) noexcept -> Container<T>;

    template<usize N, typename T>
    constexpr auto filled_with(T value) noexcept -> array<T, N>;

    template<stdr::range Out, stdr::input_range... Inputs>
    constexpr auto merge(Out& output, const Inputs&... ranges) noexcept -> void;

    template<template<class...> typename Out = dyn_array, stdr::input_range... Inputs>
    constexpr auto concat(const Inputs&... inputs) noexcept -> Out<meta::CanonicalType<meta::RangeType<Inputs...[0]>>>;

    template<stdr::range Out, stdr::input_range... Inputs>
    constexpr auto move_and_merge(Out& output, Inputs&&... ranges) noexcept -> void;

    template<template<class...> typename Out = dyn_array, stdr::input_range... Inputs>
    constexpr auto move_and_concat(Inputs&&... inputs) noexcept -> Out<meta::CanonicalType<meta::RangeType<Inputs...[0]>>>;

    using std::to_array;

    template<stdr::input_range T>
    constexpr auto to_dyn_array(T&& range) noexcept -> dyn_array<stdr::range_value_t<T>>;

    template<typename... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_array(Args&&... args) noexcept -> array<meta::CanonicalType<Args...[0]>, sizeof...(Args)>;

    template<typename T, typename... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_array_of(Args&&... args) noexcept -> array<T, sizeof...(Args)>;

    template<typename... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_dyn_array(Args&&... args) noexcept -> dyn_array<meta::CanonicalType<Args...[0]>>;

    template<typename T, typename... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_dyn_array_of(Args&&... args) noexcept -> dyn_array<T>;

    template<meta::IsStringLike T>
    constexpr auto as_view(T& range) noexcept -> string_view;

    template<typename T>
        requires(not stdr::range<T>)
    constexpr auto as_view(T& value) noexcept -> array_view<T>;

    template<stdr::contiguous_range T>
    constexpr auto as_view(T& range) noexcept -> array_view<meta::ForwardConst<T, stdr::range_value_t<T>>>;

    template<stdr::contiguous_range T>
    constexpr auto as_view(T& range, Force) noexcept -> array_view<meta::ForwardConst<T, stdr::range_value_t<T>>>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class> typename Container, typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto filled_with(usize size, T value) noexcept -> Container<T> {
        auto out = Container<T> {};
        out.resize(size);
        std::ranges::fill(out, value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize N, typename T>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto filled_with(T value) noexcept -> array<T, N> {
        auto out = array<T, N> {};
        std::ranges::fill(out, value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<stdr::range Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    constexpr auto merge(Out& output, const Inputs&... ranges) noexcept -> void {
        static_assert(stdr::output_range<Out, meta::RangeType<Inputs...[0]>>);
        output.reserve(std::size(output) + (stdr::size(ranges) + ...));
        (stdr::copy(ranges, std::back_inserter(output)), ...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class...> typename Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto concat(const Inputs&... inputs) noexcept -> Out<meta::CanonicalType<meta::RangeType<Inputs...[0]>>> {
        auto output = Out<meta::CanonicalType<meta::RangeType<Inputs...[0]>>> {};
        merge(output, inputs...);

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<stdr::range Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    constexpr auto move_and_merge(Out& output, Inputs&&... inputs) noexcept -> void {
        static_assert(stdr::output_range<Out, meta::RangeType<Inputs...[0]>>);
        output.reserve(std::size(output) + (stdr::size(inputs) + ...));
        (stdr::move(std::forward<Inputs>(inputs), std::back_inserter(output)), ...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class...> typename Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto move_and_concat(Inputs&&... inputs) noexcept -> Out<meta::CanonicalType<meta::RangeType<Inputs...[0]>>> {
        auto output = Out<meta::CanonicalType<meta::RangeType<Inputs...[0]>>> {};
        move_and_merge(output, std::forward<Inputs>(inputs)...);

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range T>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto to_dyn_array(T&& range) noexcept -> dyn_array<stdr::range_value_t<T>> {
        return std::forward<T>(range) | stdr::to<dyn_array<stdr::range_value_t<T>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto into_array(Args&&... args) noexcept -> array<meta::CanonicalType<Args...[0]>, sizeof...(Args)> {
        static_assert((not meta::IsLValueReference<Args> and ...),
                      "lvalue reference can't be passed to into_ functions as it take "
                      "ownership");
        return array { std::move(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto into_array_of(Args&&... args) noexcept -> array<T, sizeof...(Args)> {
        return array<T, sizeof...(Args)> { T { std::move(args) }... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto into_dyn_array(Args&&... args) noexcept -> dyn_array<meta::CanonicalType<Args...[0]>> {
        static_assert((not meta::IsLValueReference<Args> and ...),
                      "lvalue reference can't be passed to into_ functions as it take "
                      "ownership");
        return dyn_array { std::move(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto into_dyn_array_of(Args&&... args) noexcept -> dyn_array<T> {
        return dyn_array<T> { T { std::move(args) }... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsStringLike T>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto as_view(T& range) noexcept -> string_view {
        return string_view { range };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not stdr::range<T>)
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto as_view(T& value) noexcept -> array_view<T> {
        return { &value, 1 };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::contiguous_range T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto as_view(T& range) noexcept -> array_view<meta::ForwardConst<T, stdr::range_value_t<T>>> {
        return { range };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::contiguous_range T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto as_view(T& range, Force) noexcept -> array_view<meta::ForwardConst<T, stdr::range_value_t<T>>> {
        return { range };
    }
}} // namespace stormkit::core
