// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.ranges.numeric_range;

import std;

import stormkit.core.types;

import stormkit.core.meta;
import stormkit.core.typesafe.safecasts;
import stormkit.core.coroutines;

export namespace stormkit { inline namespace core {
    template<meta::arithmetic T>
    struct NumericsRange {
        using range_type = T;
        T begin;
        T end;
        T step = T { 1 };
    };

    namespace meta {
        template<class T>
        concept IsNumericsRange = requires(const T& t) {
            t.begin;
            t.end;
            t.step;
            typename T::range_type;
        };

        template<class T>
        concept IsNumericsRangePure = IsNumericsRange<to_plain_type<T>>;
    } // namespace meta

    template<meta::arithmetic T>
    [[nodiscard]]
    constexpr auto range(const T& end) noexcept -> decltype(auto);

    template<meta::arithmetic T, meta::arithmetic U>
    [[nodiscard]]
    constexpr auto range(const T& begin, const U& end) noexcept -> decltype(auto);

    template<meta::arithmetic T, meta::arithmetic U, meta::arithmetic V = U>
    [[nodiscard]]
    constexpr auto range(const T& begin, const U& end, const V& step) noexcept -> decltype(auto);

    [[nodiscard]]
    constexpr auto range(meta::IsNumericsRangePure auto&& range) noexcept -> decltype(auto);

    template<meta::arithmetic... Ts>
    [[nodiscard]]
    constexpr auto multi_range(const Ts&... args) noexcept -> decltype(auto);

    template<meta::IsNumericsRangePure... Ts>
    [[nodiscard]]
    constexpr auto multi_range(Ts&&... args) noexcept -> decltype(auto);
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace stormkit { inline namespace core {
#define FOR(a, b)  for (auto a = b.begin; a < b.end; a += b.step)
#define YIELD(...) co_yield { __VA_ARGS__ };

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRange T, meta::IsNumericsRange U>
    auto range_implementation(T a, U b) noexcept -> std::generator<std::tuple<decltype(a.begin), decltype(b.begin)>> {
        FOR(i, a)
        FOR(j, b)
        YIELD(i, j)
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRange T, meta::IsNumericsRange U, meta::IsNumericsRange V>
    auto range_implementation(T a, U b, V c) noexcept
      -> std::generator<std::tuple<decltype(a.begin), decltype(b.begin), decltype(c.begin)>> {
        FOR(i, a)
        FOR(j, b)
        FOR(k, c)
        YIELD(i, j, k)
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRange T, meta::IsNumericsRange U, meta::IsNumericsRange V, meta::IsNumericsRange W>
    auto range_implementation(T a, U b, V c, W d) noexcept
      -> std::generator<std::tuple<decltype(a.begin), decltype(b.begin), decltype(c.begin), decltype(d.begin)>> {
        FOR(i, a)
        FOR(j, b)
        FOR(k, c)
        FOR(l, d)
        YIELD(i, j, k, l)
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRange T,
             meta::IsNumericsRange U,
             meta::IsNumericsRange V,
             meta::IsNumericsRange W,
             meta::IsNumericsRange X>
    auto range_implementation(T a, U b, V c, W d, X e) noexcept -> std::generator<
      std::tuple<decltype(a.begin), decltype(b.begin), decltype(c.begin), decltype(d.begin), decltype(e.begin)>> {
        FOR(i, a)
        FOR(j, b)
        FOR(k, c)
        FOR(l, d)
        FOR(m, e)
        YIELD(i, j, k, l, m)
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRange T,
             meta::IsNumericsRange U,
             meta::IsNumericsRange V,
             meta::IsNumericsRange W,
             meta::IsNumericsRange X,
             meta::IsNumericsRange Y>
    auto range_implementation(T a, U b, V c, W d, X e, Y f) noexcept -> std::generator<
      std::
        tuple<decltype(a.begin), decltype(b.begin), decltype(c.begin), decltype(d.begin), decltype(e.begin), decltype(f.begin)>> {
        FOR(i, a)
        FOR(j, b)
        FOR(k, c)
        FOR(l, d)
        FOR(m, e)
        FOR(n, f)
        YIELD(i, j, k, l, m, n)
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRange T,
             meta::IsNumericsRange U,
             meta::IsNumericsRange V,
             meta::IsNumericsRange W,
             meta::IsNumericsRange X,
             meta::IsNumericsRange Y,
             meta::IsNumericsRange Z>
    auto range_implementation(T a, U b, V c, W d, X e, Y f, Z g) noexcept
      -> std::generator<std::tuple<decltype(a.begin),
                                   decltype(b.begin),
                                   decltype(c.begin),
                                   decltype(d.begin),
                                   decltype(e.begin),
                                   decltype(f.begin),
                                   decltype(g.begin)>> {
        FOR(i, a)
        FOR(j, b)
        FOR(k, c)
        FOR(l, d)
        FOR(m, e)
        FOR(n, f)
        FOR(o, g)
        YIELD(i, j, k, l, m, n, g)
    }

#undef FOR
#undef YIELD

    template<typename T>
    struct Range {
        using type = typename T::range_type;

        struct Sentinel {
            type val;
        };

        struct Iterator {
            using value_type      = T;
            using difference_type = isize;

            constexpr Iterator(Type val, Type step) noexcept : m_val { val }, m_step { step } {}

            constexpr Iterator(const Iterator&) noexcept = default;
            constexpr Iterator(Iterator&&) noexcept      = default;
            constexpr ~Iterator() noexcept               = default;

            constexpr auto operator=(const Iterator&) noexcept -> Iterator& = default;
            constexpr auto operator=(Iterator&&) noexcept -> Iterator&      = default;

            constexpr auto operator=(const Sentinel& end) noexcept -> Iterator& {
                m_val = end.val;

                return *this;
            }

            constexpr auto operator+(std::size_t index) noexcept -> decltype(auto) {
                auto cpy = auto { *this };
                for (auto i = 0u; i < index; ++i) ++cpy;
                return cpy;
            }

            constexpr auto operator++() noexcept -> decltype(auto) {
                m_val += m_step;
                return *this;
            }

            constexpr auto operator++(int) noexcept -> decltype(auto) {
                auto old = auto { *this };
                m_val += m_step;
                return old;
            }

            constexpr auto operator--() noexcept -> decltype(auto) {
                m_val -= m_step;
                return *this;
            }

            constexpr auto operator--(int) noexcept -> decltype(auto) {
                auto old = auto { *this };
                m_val -= m_step;
                return old;
            }

            constexpr auto operator==(const Iterator& other) const noexcept { return m_val == other.m_val; }

            constexpr auto operator==(const Sentinel& end) const noexcept -> bool {
                if (m_step > 0) return m_val >= end.val;

                return m_val <= end.val;
            }

            constexpr auto operator!=(const Iterator& other) const noexcept { return m_val != other.m_val; }

            constexpr auto operator*() const noexcept -> const Type& { return m_val; }

          private:
            Type m_val;
            Type m_step;
        };

        constexpr explicit Range(meta::same_as<T> auto&& range) : m_range { std::forward<decltype(range)>(range) } {}

        constexpr auto begin() const noexcept -> Iterator { return { m_range.begin, m_range.step }; }

        constexpr auto cbegin() const noexcept -> Iterator { return begin(); }

        constexpr auto end() const noexcept -> Sentinel { return { m_range.end }; }

        constexpr auto cend() const noexcept -> Sentinel { return end(); }

      private:
        T m_range;
    };

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto range(meta::IsNumericsRangePure auto&& range) noexcept -> decltype(auto) {
        return Range<meta::to_plain_type<decltype(range)>> { std::forward<decltype(range)>(range) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arithmetic T, meta::arithmetic U, meta::arithmetic V>
    STORMKIT_FORCE_INLINE
    constexpr auto range(const T& begin, const U& end, const V& step) noexcept -> decltype(auto) {
        using type = meta::safe_narrow_type<meta::safe_narrow_type<T, U>, V>;
        return range(NumericsRange<type> { .begin = as<type>(begin), .end = as<type>(end), .step = as<type>(step) });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arithmetic T, meta::arithmetic U>
    STORMKIT_FORCE_INLINE
    constexpr auto range(const T& begin, const U& end) noexcept -> decltype(auto) {
        return stdv::iota(begin, end);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto range(const T& end) noexcept -> decltype(auto) {
        return range(T { 0 }, end);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arithmetic... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto multi_range(const Ts&... args) noexcept -> decltype(auto) {
        return range_implementation(NumericsRange<Ts> { .begin = 0, .end = args }...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNumericsRangePure... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto multi_range(Ts&&... args) noexcept -> decltype(auto) {
        return range_implementation(std::forward<Ts>(args)...);
    }
}} // namespace stormkit::core
