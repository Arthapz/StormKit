// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.core;
import stormkit.test;

#include <stormkit/test/test_macro.hpp>

using namespace stormkit::core;

struct Foo {
    int b;
};

template<stormkit::meta::IsIntegral T>
constexpr auto as_impl(const Foo& value, const std::source_location&) noexcept -> T {
    return static_cast<T>(value.b);
}

namespace bar {
    struct Foo {
        int b;
    };

    template<stormkit::meta::IsIntegral T>
    constexpr auto as_impl(const Foo& value, const std::source_location&) noexcept -> T {
        return static_cast<T>(value.b);
    }
} // namespace bar

namespace {
    [[maybe_unused]]
    const auto i8_1 = i8 { 1 };
    [[maybe_unused]]
    const auto i8_2 = i8 { 2 };

    [[maybe_unused]]
    const auto u8_1 = u8 { 1 };
    [[maybe_unused]]
    const auto u8_2 = u8 { 2 };

    [[maybe_unused]]
    const auto i16_1 = i16 { 1 };
    [[maybe_unused]]
    const auto i16_2 = i16 { 2 };

    [[maybe_unused]]
    const auto u16_1 = u16 { 1 };
    [[maybe_unused]]
    const auto u16_2 = u16 { 2 };

    [[maybe_unused]]
    const auto i32_1 = i32 { 1 };
    [[maybe_unused]]
    const auto i32_2 = i32 { 2 };

    [[maybe_unused]]
    const auto u32_1 = u32 { 1 };
    [[maybe_unused]]
    const auto u32_2 = u32 { 2 };

    [[maybe_unused]]
    const auto i64_1 = i64 { 1 };
    [[maybe_unused]]
    const auto i64_2 = i64 { 2 };

    [[maybe_unused]]
    const auto u64_1 = u64 { 1 };
    [[maybe_unused]]
    const auto u64_2 = u64 { 2 };

    [[maybe_unused]]
    const auto i128_1 = i128 { 1 };
    [[maybe_unused]]
    const auto i128_2 = i128 { 2 };

    [[maybe_unused]]
    const auto u128_1 = u128 { 1 };
    [[maybe_unused]]
    const auto u128_2 = u128 { 2 };

    auto _ = test::TestSuite {
        "Core.typesafe.safecasts",
        {
          { "arithmetic.integrals",
            [] static noexcept {

            } },
          { "arithmetic.is.integral",
            [] static noexcept {
                EXPECTS(is_equal(7, 7));
                EXPECTS(is_equal(19, 19.f));
            } },
          { "arithmetic.as.integral",
            [] static noexcept {
                static_assert(meta::IsSignNarrowing<i32, u32>);
                static_assert(meta::IsSignNarrowing<u32, i32>);
                static_assert(not meta::IsSignNarrowing<i8, u32>);
                static_assert(meta::IsSignNarrowing<u32, i8>);
                static_assert(meta::IsNarrowing<i16, i32>);
                static_assert(not meta::IsNarrowing<i32, i16>);
                static_assert(meta::IsNarrowing<i32, u32>);
                static_assert(meta::IsNarrowing<u32, i32>);

                EXPECTS(as<i8>(127) == i8 { 127 });
                EXPECTS(as<i8>(-80) != i8 { -81 });

                EXPECTS(as<i32>(8u) == 8);
                EXPECTS(as<i32>(1820) != 7);

                EXPECTS(as<u32>(14) == 14u);

                EXPECTS(is_equal(as<f32>(19), 19));
                EXPECTS(as<i8>(1_i16) == 1);
                EXPECTS(as<i8>(1_i32) == 1);
                EXPECTS(as<i8>(1_i64) == 1);
                EXPECTS(as<i8>(1_i128) == 1);
                EXPECTS(as<i8>(1_u8) == 1);
                EXPECTS(as<i8>(1_u16) == 1);
                EXPECTS(as<i8>(1_u32) == 1);
                EXPECTS(as<i8>(1_u64) == 1);
                EXPECTS(as<i8>(1_u128) == 1);

                EXPECTS(as<u8>(1_i8) == 1);
                EXPECTS(as<u8>(1_i16) == 1);
                EXPECTS(as<u8>(1_i32) == 1);
                EXPECTS(as<u8>(1_i64) == 1);
                EXPECTS(as<u8>(1_i128) == 1);
                EXPECTS(as<u8>(1_u16) == 1);
                EXPECTS(as<u8>(1_u32) == 1);
                EXPECTS(as<u8>(1_u64) == 1);
                EXPECTS(as<u8>(1_u128) == 1);

                EXPECTS(as<i16>(1_i8) == 1);
                EXPECTS(as<i16>(1_i32) == 1);
                EXPECTS(as<i16>(1_i64) == 1);
                EXPECTS(as<i16>(1_i128) == 1);
                EXPECTS(as<i16>(1_u8) == 1);
                EXPECTS(as<i16>(1_u16) == 1);
                EXPECTS(as<i16>(1_u32) == 1);
                EXPECTS(as<i16>(1_u64) == 1);
                EXPECTS(as<i16>(1_u128) == 1);

                EXPECTS(as<u16>(1_i8) == 1);
                EXPECTS(as<u16>(1_i16) == 1);
                EXPECTS(as<u16>(1_i32) == 1);
                EXPECTS(as<u16>(1_i64) == 1);
                EXPECTS(as<u16>(1_i128) == 1);
                EXPECTS(as<u16>(1_u8) == 1);
                EXPECTS(as<u16>(1_u32) == 1);
                EXPECTS(as<u16>(1_u64) == 1);
                EXPECTS(as<u16>(1_u128) == 1);

                EXPECTS(as<i32>(1_i8) == 1);
                EXPECTS(as<i32>(1_i16) == 1);
                EXPECTS(as<i32>(1_i64) == 1);
                EXPECTS(as<i32>(1_i128) == 1);
                EXPECTS(as<i32>(1_u8) == 1);
                EXPECTS(as<i32>(1_u16) == 1);
                EXPECTS(as<i32>(1_u32) == 1);
                EXPECTS(as<i32>(1_u64) == 1);
                EXPECTS(as<i32>(1_u128) == 1);

                EXPECTS(as<u32>(1_i8) == 1);
                EXPECTS(as<u32>(1_i16) == 1);
                EXPECTS(as<u32>(1_i32) == 1);
                EXPECTS(as<u32>(1_i64) == 1);
                EXPECTS(as<u32>(1_i128) == 1);
                EXPECTS(as<u32>(1_u8) == 1);
                EXPECTS(as<u32>(1_u16) == 1);
                EXPECTS(as<u32>(1_u64) == 1);
                EXPECTS(as<u32>(1_u128) == 1);

                EXPECTS(as<i64>(1_i8) == 1);
                EXPECTS(as<i64>(1_i16) == 1);
                EXPECTS(as<i64>(1_i32) == 1);
                EXPECTS(as<i64>(1_i128) == 1);
                EXPECTS(as<i64>(1_u8) == 1);
                EXPECTS(as<i64>(1_u16) == 1);
                EXPECTS(as<i64>(1_u32) == 1);
                EXPECTS(as<i64>(1_u64) == 1);
                EXPECTS(as<i64>(1_u128) == 1);

                EXPECTS(as<u64>(1_i8) == 1);
                EXPECTS(as<u64>(1_i16) == 1);
                EXPECTS(as<u64>(1_i32) == 1);
                EXPECTS(as<u64>(1_i64) == 1);
                EXPECTS(as<u64>(1_i128) == 1);
                EXPECTS(as<u64>(1_u8) == 1);
                EXPECTS(as<u64>(1_u16) == 1);
                EXPECTS(as<u64>(1_u32) == 1);
                EXPECTS(as<u64>(1_u128) == 1);

                EXPECTS(as<i128>(1_i8) == 1);
                EXPECTS(as<i128>(1_i16) == 1);
                EXPECTS(as<i128>(1_i32) == 1);
                EXPECTS(as<i128>(1_i64) == 1);
                EXPECTS(as<i128>(1_u8) == 1);
                EXPECTS(as<i128>(1_u16) == 1);
                EXPECTS(as<i128>(1_u32) == 1);
                EXPECTS(as<i128>(1_u64) == 1);
                EXPECTS(as<i128>(1_u128) == 1);

                EXPECTS(as<u128>(1_i8) == 1);
                EXPECTS(as<u128>(1_i16) == 1);
                EXPECTS(as<u128>(1_i32) == 1);
                EXPECTS(as<u128>(1_i64) == 1);
                EXPECTS(as<u128>(1_i128) == 1);
                EXPECTS(as<u128>(1_u8) == 1);
                EXPECTS(as<u128>(1_u16) == 1);
                EXPECTS(as<u128>(1_u32) == 1);
                EXPECTS(as<u128>(1_u64) == 1);
            } },
          { "arithmetic.narrow.integral",
            [] static noexcept {
                EXPECTS(narrow<i8>(1_i16) == 1);
                EXPECTS(narrow<i8>(1_i32) == 1);
                EXPECTS(narrow<i8>(1_i64) == 1);
                EXPECTS(narrow<i8>(1_i128) == 1);
                EXPECTS(narrow<i8>(1_u8) == 1);
                EXPECTS(narrow<i8>(1_u16) == 1);
                EXPECTS(narrow<i8>(1_u32) == 1);
                EXPECTS(narrow<i8>(1_u64) == 1);
                EXPECTS(narrow<i8>(1_u128) == 1);

                EXPECTS(narrow<u8>(1_i8) == 1);
                EXPECTS(narrow<u8>(1_i16) == 1);
                EXPECTS(narrow<u8>(1_i32) == 1);
                EXPECTS(narrow<u8>(1_i64) == 1);
                EXPECTS(narrow<u8>(1_i128) == 1);
                EXPECTS(narrow<u8>(1_u16) == 1);
                EXPECTS(narrow<u8>(1_u32) == 1);
                EXPECTS(narrow<u8>(1_u64) == 1);
                EXPECTS(narrow<u8>(1_u128) == 1);

                EXPECTS(narrow<i16>(1_i8) == 1);
                EXPECTS(narrow<i16>(1_i32) == 1);
                EXPECTS(narrow<i16>(1_i64) == 1);
                EXPECTS(narrow<i16>(1_i128) == 1);
                EXPECTS(narrow<i16>(1_u8) == 1);
                EXPECTS(narrow<i16>(1_u16) == 1);
                EXPECTS(narrow<i16>(1_u32) == 1);
                EXPECTS(narrow<i16>(1_u64) == 1);
                EXPECTS(narrow<i16>(1_u128) == 1);

                EXPECTS(narrow<u16>(1_i8) == 1);
                EXPECTS(narrow<u16>(1_i16) == 1);
                EXPECTS(narrow<u16>(1_i32) == 1);
                EXPECTS(narrow<u16>(1_i64) == 1);
                EXPECTS(narrow<u16>(1_i128) == 1);
                EXPECTS(narrow<u16>(1_u8) == 1);
                EXPECTS(narrow<u16>(1_u32) == 1);
                EXPECTS(narrow<u16>(1_u64) == 1);
                EXPECTS(narrow<u16>(1_u128) == 1);

                EXPECTS(narrow<i32>(1_i8) == 1);
                EXPECTS(narrow<i32>(1_i16) == 1);
                EXPECTS(narrow<i32>(1_i64) == 1);
                EXPECTS(narrow<i32>(1_i128) == 1);
                EXPECTS(narrow<i32>(1_u8) == 1);
                EXPECTS(narrow<i32>(1_u16) == 1);
                EXPECTS(narrow<i32>(1_u32) == 1);
                EXPECTS(narrow<i32>(1_u64) == 1);
                EXPECTS(narrow<i32>(1_u128) == 1);

                EXPECTS(narrow<u32>(1_i8) == 1);
                EXPECTS(narrow<u32>(1_i16) == 1);
                EXPECTS(narrow<u32>(1_i32) == 1);
                EXPECTS(narrow<u32>(1_i64) == 1);
                EXPECTS(narrow<u32>(1_i128) == 1);
                EXPECTS(narrow<u32>(1_u8) == 1);
                EXPECTS(narrow<u32>(1_u16) == 1);
                EXPECTS(narrow<u32>(1_u64) == 1);
                EXPECTS(narrow<u32>(1_u128) == 1);

                EXPECTS(narrow<i64>(1_i8) == 1);
                EXPECTS(narrow<i64>(1_i16) == 1);
                EXPECTS(narrow<i64>(1_i32) == 1);
                EXPECTS(narrow<i64>(1_i128) == 1);
                EXPECTS(narrow<i64>(1_u8) == 1);
                EXPECTS(narrow<i64>(1_u16) == 1);
                EXPECTS(narrow<i64>(1_u32) == 1);
                EXPECTS(narrow<i64>(1_u64) == 1);
                EXPECTS(narrow<i64>(1_u128) == 1);

                EXPECTS(narrow<u64>(1_i8) == 1);
                EXPECTS(narrow<u64>(1_i16) == 1);
                EXPECTS(narrow<u64>(1_i32) == 1);
                EXPECTS(narrow<u64>(1_i64) == 1);
                EXPECTS(narrow<u64>(1_i128) == 1);
                EXPECTS(narrow<u64>(1_u8) == 1);
                EXPECTS(narrow<u64>(1_u16) == 1);
                EXPECTS(narrow<u64>(1_u32) == 1);
                EXPECTS(narrow<u64>(1_u128) == 1);

                EXPECTS(narrow<i128>(1_i8) == 1);
                EXPECTS(narrow<i128>(1_i16) == 1);
                EXPECTS(narrow<i128>(1_i32) == 1);
                EXPECTS(narrow<i128>(1_i64) == 1);
                EXPECTS(narrow<i128>(1_u8) == 1);
                EXPECTS(narrow<i128>(1_u16) == 1);
                EXPECTS(narrow<i128>(1_u32) == 1);
                EXPECTS(narrow<i128>(1_u64) == 1);
                EXPECTS(narrow<i128>(1_u128) == 1);

                EXPECTS(narrow<u128>(1_i8) == 1);
                EXPECTS(narrow<u128>(1_i16) == 1);
                EXPECTS(narrow<u128>(1_i32) == 1);
                EXPECTS(narrow<u128>(1_i64) == 1);
                EXPECTS(narrow<u128>(1_i128) == 1);
                EXPECTS(narrow<u128>(1_u8) == 1);
                EXPECTS(narrow<u128>(1_u16) == 1);
                EXPECTS(narrow<u128>(1_u32) == 1);
                EXPECTS(narrow<u128>(1_u64) == 1);
            } },
          }
    };
} // namespace
