// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <version>

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.containers.utils;

import std;

import stormkit.core.meta;
import stormkit.core.types;
import stormkit.core.typesafe;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core {
    namespace meta {
        template<stdr::input_range... Inputs>
        using CommonRangeType = stdr::range_value_t<FirstT<Inputs...>>;
    } // namespace meta

    // template<template<class> typename Container = std::inplace_vector, typename T>
    template<template<class> typename Container = dynarray, typename T>
    constexpr auto filled_with(usize size, T value) noexcept -> Container<T>;

    template<usize N, typename T>
    constexpr auto filled_with(T value) noexcept -> array<T, N>;

    template<stdr::range Out, stdr::input_range... Inputs>
    constexpr auto merge(Out& output, const Inputs&... ranges) noexcept -> void;

    template<template<class...> typename Out = dynarray, stdr::input_range... Inputs>
    constexpr auto concat(const Inputs&... inputs) noexcept -> Out<meta::CommonRangeType<Inputs...>>;

    template<stdr::range Out, stdr::input_range... Inputs>
    constexpr auto move_and_merge(Out& output, Inputs&&... ranges) noexcept -> void;

    template<template<class...> typename Out = dynarray, stdr::input_range... Inputs>
    constexpr auto move_and_concat(Inputs&&... inputs) noexcept -> Out<meta::CommonRangeType<Inputs...>>;

    using std::to_array;

    template<stdr::input_range T>
    constexpr auto to_dynarray(T&& range) noexcept -> dynarray<stdr::range_value_t<T>>;

    template<typename... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_array(Args&&... args) noexcept -> array<meta::FirstT<Args...>, sizeof...(Args)>;

    template<typename T, meta::Is<T>... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_array_of(Args&&... args) noexcept -> array<T, sizeof...(Args)>;

    template<typename... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_dynarray(Args&&... args) noexcept -> dynarray<meta::FirstT<Args...>>;

    template<typename T, meta::Is<T>... Args>
        requires(sizeof...(Args) > 0)
    constexpr auto into_dynarray_of(Args&&... args) noexcept -> dynarray<T>;

    template<meta::IsStringLike T>
    constexpr auto as_view(T& range) noexcept -> string_view;

    template<typename T>
        requires(not stdr::range<T>)
    constexpr auto as_view(T& value) noexcept -> array_view<T>;

    template<stdr::contiguous_range T>
    constexpr auto as_view(T& range) noexcept -> array_view<meta::ForwardConst<T, stdr::range_value_t<T>>>;
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
    constexpr auto concat(const Inputs&... inputs) noexcept -> Out<meta::CommonRangeType<Inputs...>> {
        auto output = Out<meta::CommonRangeType<Inputs...>> {};
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
    constexpr auto move_and_concat(Inputs&&... inputs) noexcept -> Out<meta::CommonRangeType<Inputs...>> {
        auto output = Out<meta::CommonRangeType<Inputs...>> {};
        move_and_merge(output, std::forward<Inputs>(inputs)...);

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range T>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto to_dynarray(T&& range) noexcept -> dynarray<stdr::range_value_t<T>> {
        return std::forward<T>(range) | stdr::to<dynarray<stdr::range_value_t<T>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto into_array(Args&&... args) noexcept -> array<meta::FirstT<Args...>, sizeof...(Args)> {
        static_assert((not meta::IsLValueReference<Args> and ...),
                      "lvalue reference can't be passed to into_ functions as it take "
                      "ownership");
        return array { std::move(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::Is<T>... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto into_array_of(Args&&... args) noexcept -> array<T, sizeof...(Args)> {
        return array<T, sizeof...(Args)> { std::forward<Args>(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto into_dynarray(Args&&... args) noexcept -> dynarray<meta::FirstT<Args...>> {
        static_assert((not meta::IsLValueReference<Args> and ...),
                      "lvalue reference can't be passed to into_ functions as it take "
                      "ownership");
        return dynarray { std::move(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::Is<T>... Args>
        requires(sizeof...(Args) > 0)
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto into_dynarray_of(Args&&... args) noexcept -> dynarray<T> {
        return dynarray<T> { std::forward<Args>(args)... };
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
}} // namespace stormkit::core
