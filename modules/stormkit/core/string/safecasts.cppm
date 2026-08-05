// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.string.safecasts;

import std;

import stormkit.core.types;
import stormkit.core.errors;
import stormkit.core.typesafe.safecasts;
import stormkit.core.contract;
import stormkit.core.meta.concepts;
import stormkit.core.meta.type_manipulation;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core {
    namespace meta {
        template<typename T>
        concept Has_as_string_view = requires(const T& value) {
            { as<string_view>(value) } -> SameAs<string_view>;
        };
    } // namespace meta

    template<meta::Has_as_string_view From>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<string>, From&& value, const std::source_location&) noexcept -> string;

    template<meta::arg::PlainTypeTo<meta::IsIntegral> From>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<string>,
                              From&& value,
                              i32    base                 = 10,
                              const std::source_location& = std::source_location::current()) noexcept -> System_result<string>;

    template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> From>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<string>,
                              From&&            value,
                              std::chars_format fmt       = std::chars_format::general,
                              const std::source_location& = std::source_location::current()) noexcept -> System_result<string>;

    template<meta::arg::PlainTypeTo<meta::IsIntegral> To>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>,
                              std::string_view value,
                              i32              base       = 10,
                              const std::source_location& = std::source_location::current()) noexcept -> System_result<To>;

    template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> To>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>,
                              std::string_view  value,
                              std::chars_format fmt       = std::chars_format::general,
                              const std::source_location& = std::source_location::current()) noexcept -> System_result<To>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::Has_as_string_view From>
        STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<string>, From&& value, const std::source_location&) noexcept -> string {
        return string { as<string_view>(std::forward<From>(value)) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsIntegral> From>
    constexpr auto tag_invoke(as_fn<string>, From&& value, i32 base, const std::source_location&) noexcept
      -> System_result<string> {
        auto out = System_result<string> { std::in_place };
        out->resize(16);
        auto&& [ptr, errc] = std::to_chars(stdr::data(*out), stdr::data(*out) + stdr::size(*out), value, base);
        if (errc != std::errc {}) [[unlikely]]
            out = std::unexpected<System_code> { std::in_place, error::from_stderrc(std::move(errc)) };
        else {
            const auto size = std::distance(stdr::data(*out), ptr);
            out->resize(as<usize>(size));
        }

        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> From>
    constexpr auto tag_invoke(as_fn<string>, From&& value, std::chars_format fmt, const std::source_location&) noexcept
      -> System_result<string> {
        auto out = System_result<string> { std::in_place };
        out->resize(16, '\0');

        auto&& [ptr, errc] = std::to_chars(stdr::data(*out), stdr::data(*out) + stdr::size(*out), value, fmt);
        if (errc != std::errc {}) [[unlikely]]
            out = std::unexpected<System_code> { std::in_place, error::from_stderrc(std::move(errc)) };
        else {
            const auto size = std::distance(stdr::data(*out), ptr);
            out->resize(size);
        }

        return out;
    }

    template<meta::arg::PlainTypeTo<meta::IsIntegral> To>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>, std::string_view value, i32 base, const std::source_location&) noexcept
      -> System_result<To> {
        auto out         = System_result<To> { std::in_place };
        auto&& [_, errc] = std::from_chars(stdr::data(value), stdr::data(value) + stdr::size(value), out.value(), base);
        if (errc != std::errc {}) [[unlikely]]
            out = std::unexpected<System_code> { std::in_place, error::from_stderrc(std::move(errc)) };

        return out;
    }

    template<meta::arg::PlainTypeTo<meta::IsFloatingPoint> To>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>, std::string_view value, std::chars_format fmt, const std::source_location&) noexcept
      -> System_result<To> {
        auto out         = System_result<To> { std::in_place };
        auto&& [_, errc] = std::from_chars(stdr::data(value), stdr::data(value) + stdr::size(value), out.value(), fmt);
        if (errc != std::errc {}) [[unlikely]]
            out = std::unexpected<System_code> { std::in_place, error::from_stderrc(std::move(errc)) };

        return out;
    }
}} // namespace stormkit::core
