// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
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
        using common_range_value_type = range_value_type<first_type<Inputs...>>;
    } // namespace meta

    template<typename Container, typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    constexpr auto make_container(T&& first, Ts&&... args) noexcept -> Container;

    template<template<class> typename Container, typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    constexpr auto make_container(Ts&&... args) noexcept -> Container<meta::to_plain_type<T>>;

    template<template<class, usize> typename Container, typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    constexpr auto make_container(T&& first, Ts&&... args) noexcept -> Container<meta::to_plain_type<T>, 1 + sizeof...(Ts)>;

    template<typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    constexpr auto make_dynarray(T&& first, Ts&&... args) noexcept -> dynarray<meta::to_plain_type<T>>;

    template<typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    constexpr auto make_array(T&& first, Ts&&... args) noexcept -> array<meta::to_plain_type<T>, 1 + sizeof...(Ts)>;

    template<template<class> typename Container = dynarray, typename T>
    constexpr auto filled_with(usize size, T value) noexcept -> Container<T>;

    template<auto Value, template<class> typename Container = dynarray>
    constexpr auto filled_with(usize size) noexcept -> Container<decltype(Value)>;

    template<auto N, auto Value, template<class> typename Container = dynarray>
    constexpr auto filled_with() noexcept -> Container<decltype(Value)>;

    template<usize N, typename T>
    constexpr auto filled_with(T value) noexcept -> array<T, N>;

    template<usize N, auto Value>
    constexpr auto filled_with() noexcept -> array<decltype(Value), N>;

    template<stdr::range Out, stdr::input_range... Inputs>
    constexpr auto merge(Out& output, const Inputs&... ranges) noexcept -> void;

    template<template<class...> typename Out = dynarray, stdr::input_range... Inputs>
    constexpr auto concat(const Inputs&... inputs) noexcept -> Out<meta::common_range_value_type<Inputs...>>;

    template<stdr::range Out, stdr::input_range... Inputs>
    constexpr auto move_and_merge(Out& output, Inputs&&... ranges) noexcept -> void;

    template<template<class...> typename Out = dynarray, stdr::input_range... Inputs>
    constexpr auto move_and_concat(Inputs&&... inputs) noexcept -> Out<meta::common_range_value_type<Inputs...>>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Container, typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    constexpr auto make_container(T&& first, Ts&&... args) noexcept -> Container {
        auto out = Container {};
        out.reserve(1 + sizeof...(Ts));
        out.emplace_back(std::forward<T>(first));
        (out.emplace_back(std::forward<Ts>(args)), ...);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class> typename Container, typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto make_container(T&& first, Ts&&... args) noexcept -> Container<meta::to_plain_type<T>> {
        return make_container<Container<meta::to_plain_type<T>>>(std::forward<T>(first), std::forward<Ts>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class, usize> typename Container, typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto make_container(T&& first, Ts&&... args) noexcept -> Container<meta::to_plain_type<T>, 1 + sizeof...(Ts)> {
        return make_container<Container<meta::to_plain_type<T>, 1 + sizeof...(Ts)>>(std::forward<T>(first),
                                                                                    std::forward<Ts>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto make_dynarray(T&& first, Ts&&... args) noexcept -> dynarray<meta::to_plain_type<T>> {
        return make_container<dynarray>(std::forward<T>(first), std::forward<Ts>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, meta::convertible_to<meta::to_plain_type<T>>... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto make_array(T&& first, Ts... args) noexcept -> array<meta::to_plain_type<T>, 1 + sizeof...(Ts)> {
        return array<meta::to_plain_type<T>, 1 + sizeof...(Ts)> { std::forward<T>(first), std::forward<Ts>(args)... };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class> typename Container, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto filled_with(usize size, T value) noexcept -> Container<T> {
        auto out = Container<T> {};
        out.resize(size);
        std::ranges::fill(out, value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<auto Value, template<class> typename Container>
    STORMKIT_FORCE_INLINE
    constexpr auto filled_with(usize size) noexcept -> Container<decltype(Value)> {
        auto out = Container<decltype(Value)> {};
        out.resize(size);
        std::ranges::fill(out, Value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize N, auto Value, template<class> typename Container>
    STORMKIT_FORCE_INLINE
    constexpr auto filled_with() noexcept -> Container<decltype(Value)> {
        auto out = Container<decltype(Value)> {};
        out.resize(N);
        std::ranges::fill(out, Value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize N, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto filled_with(T value) noexcept -> array<T, N> {
        auto out = array<T, N> {};
        std::ranges::fill(out, value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize N, auto Value>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto filled_with() noexcept -> array<decltype(Value), N> {
        auto out = array<decltype(Value), N> {};
        std::ranges::fill(out, Value);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<stdr::range Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    constexpr auto merge(Out& output, const Inputs&... ranges) noexcept -> void {
        static_assert(stdr::output_range<Out, meta::range_value_type<Inputs...[0]>>);
        output.reserve(std::size(output) + (stdr::size(ranges) + ...));
        (stdr::copy(ranges, std::back_inserter(output)), ...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class...> typename Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    constexpr auto concat(const Inputs&... inputs) noexcept -> Out<meta::common_range_value_type<Inputs...>> {
        auto output = Out<meta::common_range_value_type<Inputs...>> {};
        merge(output, inputs...);

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<stdr::range Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    constexpr auto move_and_merge(Out& output, Inputs&&... inputs) noexcept -> void {
        static_assert(stdr::output_range<Out, meta::range_value_type<Inputs...[0]>>);
        output.reserve(std::size(output) + (stdr::size(inputs) + ...));
        (stdr::move(std::forward<Inputs>(inputs), std::back_inserter(output)), ...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<template<class...> typename Out, stdr::input_range... Inputs>
    STORMKIT_FORCE_INLINE
    constexpr auto move_and_concat(Inputs&&... inputs) noexcept -> Out<meta::common_range_value_type<Inputs...>> {
        auto output = Out<meta::common_range_value_type<Inputs...>> {};
        move_and_merge(output, std::forward<Inputs>(inputs)...);

        return output;
    }
}} // namespace stormkit::core
