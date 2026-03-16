// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <climits>
#include <cstdlib>

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:string.encodings;

import std;

import :typesafe.safecasts;
import :typesafe.integer;
import :typesafe.byte;

export namespace stormkit { inline namespace core {
    auto ascii_to_utf16(std::string_view) -> std::u16string;
    auto utf16_to_ascii(std::u16string_view) -> std::string;

    auto ascii_to_wide(std::string_view) -> std::wstring;
    auto wide_to_ascii(std::wstring_view) -> std::string;

    auto ascii_to_utf8(std::string_view) -> std::u8string;
    auto utf8_to_ascii(std::u8string_view) -> std::string;

#ifdef STORMKIT_COMPILER_MSVC
    auto to_native_encoding(std::string_view) -> std::u16string;
#else
    auto to_native_encoding(std::string_view) -> std::u8string;
#endif
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;

// TODO make proper support of char8_t / char16_t / char32_t on macOS / iOS / tvOS
namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto ascii_to_utf16(std::string_view input) -> std::u16string {
        auto output = std::u16string {};
#if not defined(STORMKIT_COMPILER_CLANG)
        auto state = std::mbstate_t {};
        output.resize(stdr::size(input));

        auto len      = 0ull;
        auto input_it = stdr::data(input);
        while ((len = std::mbrtoc16(std::bit_cast<char16_t*>(stdr::data(output)), input_it, MB_CUR_MAX, &state)) > 0ull)
            input_it += len;
#else
        output = std::bit_cast<char16_t*>(stdr::data(input));
#endif

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto utf16_to_ascii(std::u16string_view input) -> std::string {
        auto output = std::string {};
#if not defined(STORMKIT_COMPILER_CLANG)
        auto state = std::mbstate_t {};
        output.resize(stdr::size(input));

        for (const auto& c : input) std::c16rtomb(std::bit_cast<char*>(stdr::data(output)), c, &state);
#else
        output = std::bit_cast<char*>(stdr::data(input));
#endif

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto ascii_to_wide(std::string_view input) -> std::wstring {
        [[maybe_unused]]
        auto state  = std::mbstate_t {};
        auto output = std::wstring {};
        output.resize(stdr::size(input));

        [[maybe_unused]]
        auto len = 0ull;
        [[maybe_unused]]
        auto input_it = stdr::data(input);
        [[maybe_unused]]
        auto i = 0;
#if defined(STORMKIT_COMPILER_MSVC)
        while ((len = std::mbrtoc16(std::bit_cast<char16_t*>(stdr::data(output)) + i++, input_it, MB_CUR_MAX, &state)) > 0u)
            input_it += len;
#elif defined(STORMKIT_COMPILER_CLANG)
        output = std::bit_cast<wchar_t*>(stdr::data(input));
#else
        while ((len = std::mbrtoc8(std::bit_cast<char8_t*>(stdr::data(output)) + i++, input_it, MB_CUR_MAX, &state)) > 0ull)
            input_it += len;
#endif

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto wide_to_ascii(std::wstring_view input) -> std::string {
        [[maybe_unused]]
        auto state = std::mbstate_t {};
        [[maybe_unused]]
        auto output = std::string {};
        output.resize(stdr::size(input));

#if defined(STORMKIT_COMPILER_MSVC)
        for (const auto& c : input) std::c16rtomb(stdr::data(output), narrow<char16_t>(c), &state);
#elif defined(STORMKIT_COMPILER_CLANG)
        output = std::bit_cast<char*>(stdr::data(input));
#else
        for (const auto& c : input) std::c8rtomb(stdr::data(output), narrow<char>(c), &state);
#endif

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto ascii_to_utf8(std::string_view input) -> std::u8string {
        [[maybe_unused]]
        auto output = std::u8string {};
        output.resize(stdr::size(input) * narrow<usize>(MB_LEN_MAX));

#if defined(STORMKIT_COMPILER_MSVC)
        stdr::copy(as_bytes(input), std::bit_cast<char*>(stdr::begin(output)));
#elif defined(STORMKIT_COMPILER_CLANG)
        output = std::bit_cast<char8_t*>(stdr::data(input));
#else
        auto state    = std::mbstate_t {};
        auto len      = 0ull;
        auto input_it = stdr::data(input);
        auto i        = 0;
        while ((len = std::mbrtoc8(std::bit_cast<char8_t*>(stdr::data(output)) + i++, input_it, MB_CUR_MAX, &state)) > 0ull)
            input_it += len;
#endif

        output.shrink_to_fit();

        return output;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto utf8_to_ascii(std::u8string_view input) -> std::string {
        [[maybe_unused]]
        auto output = std::string {};
        output.resize(stdr::size(input));

#if defined(STORMKIT_COMPILER_MSVC)
        auto bytes = as_bytes(output);
        stdr::copy(as_bytes(input), stdr::begin(bytes));
#elif defined(STORMKIT_COMPILER_CLANG)
        output = std::bit_cast<char*>(stdr::data(input));
#else
        auto state = std::mbstate_t {};
        for (const auto& c : input) std::c8rtomb(stdr::data(output), c, &state);
#endif

        output.shrink_to_fit();

        return output;
    }

#ifdef STORMKIT_COMPILER_MSVC
    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto to_native_encoding(std::string_view input) -> std::u16string {
        return ascii_to_utf16(input);
    }
#else
    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto to_native_encoding(std::string_view input) -> std::u8string {
        return ascii_to_utf8(input);
    }
#endif
}} // namespace stormkit::core
