// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:string.operations;

import std;

import :utils.contract;

import :meta;

import :typesafe.integer;

import :string.aliases;

import :typesafe.safecasts;

namespace stdr = std::ranges;
namespace stdv = std::views;

export namespace stormkit { inline namespace core {
    [[nodiscard]]
    constexpr auto split(string_view str, string_view delim) noexcept -> dyn_array<string_view>;
    [[nodiscard]]
    constexpr auto to_lower(string_view str) noexcept -> string;
    [[nodiscard]]
    constexpr auto to_upper(string_view str) noexcept -> string;
    [[nodiscard]]
    auto to_lower(string_view str, const std::locale& locale) noexcept -> string;
    [[nodiscard]]
    auto to_upper(string_view str, const std::locale& locale) noexcept -> string;

    [[nodiscard]]
    constexpr auto replace(string_view in, string_view pattern, string_view replacement) noexcept -> string;

    template<typename T>
    [[nodiscard]]
    constexpr auto as_string(T) noexcept -> string_view = delete;

    template<typename T>
    [[nodiscard]]
    constexpr auto to_string(T) noexcept -> string = delete;

    template<typename T>
    [[nodiscard]]
    constexpr auto from_string(string_view) noexcept -> T = delete;

    template<typename T>
        requires(as_string(std::declval<T>()))
    [[nodiscard]]
    constexpr auto to_string(T&& value) noexcept -> string;

    template<meta::IsIntegral T>
    [[nodiscard]]
    constexpr auto to_string(T value, i32 base = 10) noexcept -> std::expected<string, std::errc>;

    template<meta::IsFloatingPoint T>
    [[nodiscard]]
    auto to_string(T value, std::chars_format fmt = std::chars_format::general) noexcept -> std::expected<string, std::errc>;

    template<meta::IsIntegral T>
    [[nodiscard]]
    constexpr auto from_string(string_view data, i32 base = 10) noexcept -> std::expected<T, std::errc>;

    template<meta::IsFloatingPoint T>
    [[nodiscard]]
    auto from_string(string_view data, std::chars_format fmt = std::chars_format::general) noexcept
      -> std::expected<T, std::errc>;

    [[nodiscard]]
    constexpr auto as_czstring(string_view value) noexcept -> czstring;

    template<typename T>
        requires(as_string(std::declval<T>()))
    [[nodiscard]]
    constexpr auto as_czstring(T&& value) noexcept -> czstring;

    template<meta::IsCharType T>
    constexpr auto is_text(T c) noexcept -> bool;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_upper(char c) -> char {
        if (c >= 'a' && c <= 'z') return c - 32;
        return c;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_lower(char c) -> char {
        if (c >= 'A' && c <= 'Z') return c + 32;
        return c;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto split(string_view str, string_view delim) noexcept -> dyn_array<string_view> {
        return str
               | stdv::split(delim)
               | stdv::transform([](auto&& subrange) { return string_view { stdr::cbegin(subrange), stdr::cend(subrange) }; })
               | stdr::to<dyn_array<string_view>>();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_lower(string_view str) noexcept -> string {
        auto result = string { str };
        for (auto& c : result) c = to_lower(c);
        return result;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto to_lower(string_view str, const std::locale& locale) noexcept -> string {
        auto  result = string { str };
        auto& facet  = std::use_facet<std::ctype<typename string_view::value_type>>(locale);
        facet.tolower(&result[0], &result[0] + stdr::size(result));

        return result;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_upper(string_view str) noexcept -> string {
        auto result = string { str };
        for (auto& c : result) c = to_upper(c);
        return result;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto to_upper(string_view str, const std::locale& locale) noexcept -> string {
        auto  result = string { str };
        auto& facet  = std::use_facet<std::ctype<typename string_view::value_type>>(locale);
        facet.toupper(&result[0], &result[0] + stdr::size(result));

        return result;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline constexpr auto replace(string_view in, string_view pattern, string_view replacement) noexcept -> string {
        return in
               | stdv::split(pattern)
               | stdv::transform([replacement](auto&& substr) noexcept {
                     auto out = string {};
                     out.reserve(stdr::size(replacement) + stdr::size(substr));
                     out += replacement;
                     out += string_view { stdr::cbegin(substr), stdr::cend(substr) };
                     return out;
                 })
               | stdv::join
               | stdv::drop(stdr::size(replacement))
               | stdr::to<string>();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
        requires(as_string(std::declval<T>()))
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto to_string(T&& value) noexcept -> string {
        return string { as_string(std::forward<T>(value)) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsIntegral T>
    constexpr auto to_string(T value, int base) noexcept -> std::expected<string, std::errc> {
        auto out = std::expected<string, std::errc> { std::in_place };
        out->resize(16);
        auto&& [ptr, errc] = std::to_chars(stdr::data(*out), stdr::data(*out) + stdr::size(*out), value, base);
        if (errc != std::errc {}) [[unlikely]]
            out = std::unexpected<std::errc> { std::in_place, std::move(errc) };
        else {
            const auto size = std::distance(stdr::data(*out), ptr);
            out->resize(as<usize>(size));
        }

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    // TODO add an argument to customize string buffer size
    template<meta::IsFloatingPoint T>
    [[nodiscard]]
    auto to_string(T value, std::chars_format fmt) noexcept -> std::expected<string, std::errc> {
        auto out = std::expected<string, std::errc> { std::in_place };
        out->resize(16, '\0');

        auto&& [ptr, errc] = std::to_chars(stdr::data(*out), stdr::data(*out) + stdr::size(*out), value, fmt);
        if (errc != std::errc {}) [[unlikely]]
            out = std::unexpected<std::errc> { std::in_place, std::move(errc) };
        else {
            const auto size = std::distance(stdr::data(*out), ptr);
            out->resize(size);
        }

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsIntegral T>
    inline constexpr auto from_string(string_view data, i32 base) noexcept -> std::expected<T, std::errc> {
        auto value       = T {};
        auto&& [_, errc] = std::from_chars(stdr::data(data), stdr::data(data) + stdr::size(data), value, base);
        if (errc != std::errc {}) [[unlikely]]
            return std::unexpected<std::errc> { std::in_place, std::move(errc) };

        return std::expected<T, std::errc> { std::in_place, value };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsFloatingPoint T>
    inline auto from_string(string_view data, std::chars_format fmt) noexcept -> std::expected<T, std::errc> {
        auto value       = T {};
        auto&& [_, errc] = std::from_chars(stdr::data(data), stdr::data(data) + stdr::size(data), value, fmt);
        if (errc != std::errc {}) [[unlikely]]
            return std::unexpected<std::errc> { std::in_place, std::move(errc) };

        return std::expected<T, std::errc> { std::in_place, value };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto as_czstring(string_view value) noexcept -> czstring {
        return stdr::data(value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
        requires(as_string(std::declval<T>()))
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto as_czstring(T value) noexcept -> czstring {
        return stdr::data(as_string(std::forward<T>(value)));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsCharType T>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto is_text(T c) noexcept -> bool {
        return (c >= 32 and c <= 126) or (c >= 128);
    }
}} // namespace stormkit::core
