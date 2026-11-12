// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/macro_utils.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.math.linear.vector:operations;

import :base;

export namespace stormkit::math {
    template<cmeta::arithmetic T, usize N>
    [[nodiscard]]
    constexpr auto add(const vec<T, N>& a, const vec<T, N>& b) noexcept -> vec<T, N>;

    template<cmeta::arithmetic T, usize N>
    [[nodiscard]]
    constexpr auto sub(const vec<T, N>& a, const vec<T, N>& b) noexcept -> vec<T, N>;

    template<cmeta::arithmetic T, usize N>
    [[nodiscard]]
    constexpr auto mul(const vec<T, N>& a, T b) noexcept -> vec<T, N>;

    template<cmeta::arithmetic T, usize N>
    [[nodiscard]]
    constexpr auto div(const vec<T, N>& a, T b) noexcept -> vec<T, N>;

    template<cmeta::arithmetic T, usize N>
    [[nodiscard]]
    constexpr auto normalize(const vec<T, N>& vector) noexcept -> vec<T, N>;

    template<cmeta::arithmetic T, usize N>
    [[nodiscard]]
    constexpr auto dot(const vec<T, N>& a, const vec<T, N>& b) noexcept -> T;

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept -> vec<T, 3>;

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto cross(const vec<T, 4>& a, const vec<T, 4>& b) noexcept -> vec<T, 4>;
} // namespace stormkit::math

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::math {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    constexpr auto add(const vec<T, N>& a, const vec<T, N>& b) noexcept -> vec<T, N> {
        auto out = vec<T, N> {};
        for (auto i : range(N)) out[i] = a[i] + b[i];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    constexpr auto sub(const vec<T, N>& a, const vec<T, N>& b) noexcept -> vec<T, N> {
        auto out = vec<T, N> {};
        for (auto i : range(N)) out[i] = a[i] - b[i];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    constexpr auto mul(const vec<T, N>& a, T b) noexcept -> vec<T, N> {
        auto out = vec<T, N> {};
        for (auto i : range(N)) out[i] = a[i] * b;
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    constexpr auto div(const vec<T, N>& a, T b) noexcept -> vec<T, N> {
        EXPECTS(not is(b, 0));
        auto out = vec<T, N> {};
        for (auto i : range(N)) out[i] = a[i] / b;
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    constexpr auto normalize(const vec<T, N>& vector) noexcept -> vec<T, N> {
        auto out = vec<T, N> {};

        const auto sum = init_by<T>([vector](auto& sum) noexcept {
            for (auto i : range(N)) sum += (vector[i] * vector[i]);
            sum = unchecked_narrow<T>(std::sqrt(sum));
        });

        for (auto i : range(N)) out[i] = vector[i] / sum;

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    constexpr auto dot(const vec<T, N>& a, const vec<T, N>& b) noexcept -> T {
        auto out = T { 0 };
        for (auto i : range(N)) out += (a[i] * b[i]);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept -> vec<T, 3> {
        auto out = vec<T, 3> {};
        out[0]   = a[1] * b[2] - a[2] * b[1];
        out[1]   = a[2] * b[0] - a[0] * b[2];
        out[2]   = a[0] * b[1] - a[1] * b[0];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    constexpr auto cross(const vec<T, 4>& a, const vec<T, 4>& b) noexcept -> vec<T, 4> {
        auto out = vec<T, 4> {};
        out[0]   = a[1] * b[2] - a[2] * b[1];
        out[1]   = a[2] * b[0] - a[0] * b[2];
        out[2]   = a[0] * b[1] - a[1] * b[0];
        out[3]   = T { 0 };
        return out;
    }
} // namespace stormkit::math
