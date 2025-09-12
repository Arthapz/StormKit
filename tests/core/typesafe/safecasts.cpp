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
    auto _ = test::TestSuite {
        "Core.typesafe.safecasts",
        {
          { "common.same_value",
            [] static noexcept {
                auto foo = 9;
                auto bar = as<decltype(foo)>(foo);

                EXPECTS(foo == bar);
            } },
          { "common.customization_point.no_namespace",
            [] static noexcept {
                auto foo = Foo { 2 };
                auto bar = as<short>(foo);
                EXPECTS(bar == 2);
                static_assert(meta::Is<decltype(bar), short>);
            } },
          { "common.customization_point.namespace",
            [] static noexcept {
                auto foo = bar::Foo { 2 };
                auto bar = as<short>(foo);
                EXPECTS(bar == 2);
                static_assert(meta::Is<decltype(bar), short>);
            } },
          }
    };
} // namespace
