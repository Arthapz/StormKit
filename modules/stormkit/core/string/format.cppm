// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/format_macro.hpp>

export module stormkit.core:string.format;

import std;

import :meta;
import :typesafe.integer;
import :typesafe.safecasts;
import :utils.time;
import :string.operations;

export {
    namespace stormkit { inline namespace core {
        template<typename T, typename FormatContext>
        auto format_as(const T&, FormatContext& ctx) noexcept -> decltype(ctx.out()) = delete;

        namespace meta {
            template<meta::IsEnumeration T>
            inline constexpr auto DISABLE_DEFAULT_FORMATTER_FOR_ENUM = false;

            template<typename T>
            concept IsDefaultFormattedEnumeration = IsEnumeration<T> and not DISABLE_DEFAULT_FORMATTER_FOR_ENUM<T>;

            template<typename T>
            concept HasFormatAs = requires(const T& val) {
                { format_as(val, std::declval<std::format_context&>()) } -> Is<std::format_context::iterator>;
            };
        } // namespace meta

        inline constexpr struct FormatFN {
            template<meta::HasFormatAs T, typename FormatContext>
            static constexpr auto operator()(const T& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
                return format_as(value, ctx);
            }
        } format_fn = {};

        template<typename FormatContext>
        auto format_as(std::byte, FormatContext& ctx) noexcept -> decltype(ctx.out());

        template<typename FormatContext>
        auto format_as(stormkit::fsecond, FormatContext& ctx) noexcept -> decltype(ctx.out());
    }} // namespace stormkit::core

    template<stormkit::core::meta::HasFormatAs T, typename CharT>
    struct std::formatter<T, CharT> {
        template<class ParseContext>
        [[nodiscard]]
        constexpr auto parse(ParseContext& ctx) noexcept -> decltype(ctx.begin());

        template<class FormatContext>
        [[nodiscard]]
        auto format(const T&, FormatContext& ctx) const noexcept -> decltype(ctx.out());
    };

    template<stormkit::meta::IsDefaultFormattedEnumeration T, typename CharT>
    struct std::formatter<T, CharT> {
        template<class ParseContext>
        [[nodiscard]]
        constexpr auto parse(ParseContext& ctx) noexcept -> decltype(ctx.begin());

        template<class FormatContext>
        [[nodiscard]]
        auto format(const T&, FormatContext& ctx) const -> decltype(ctx.out());
    };

    template<stormkit::meta::IsPointer T, typename CharT>
    struct std::formatter<T, CharT>: public formatter<std::uintptr_t, CharT> {
        template<class FormatContext>
        [[nodiscard]]
        auto format(const T&, FormatContext& ctx) const -> decltype(ctx.out());
    };

    template<typename CharT>
    struct std::formatter<std::error_code, CharT>: public formatter<basic_string_view<CharT>, CharT> {
        template<class FormatContext>
        [[nodiscard]]
        auto format(const std::error_code&, FormatContext& ctx) const -> decltype(ctx.out());
    };

    template<typename CharT>
    struct std::formatter<std::errc, CharT>: public formatter<basic_string_view<CharT>, CharT> {
        template<class FormatContext>
        [[nodiscard]]
        auto format(const std::errc& code, FormatContext& ctx) const -> decltype(ctx.out());
    };
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(std::byte value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        auto&& out = ctx.out();
        return std::format_to(out, "{}", narrow<i16>(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(fsecond value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        auto&& out = ctx.out();
        return std::format_to(out, "{}", value.count());
    }
}} // namespace stormkit::core

using namespace stormkit;

/////////////////////////////////////
/////////////////////////////////////
template<meta::HasFormatAs T, typename CharT>
template<class ParseContext>
constexpr auto std::formatter<T, CharT>::parse(ParseContext& ctx) noexcept -> decltype(ctx.begin()) {
    return ctx.begin();
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::HasFormatAs T, typename CharT>
template<class FormatContext>
auto std::formatter<T, CharT>::format(const T& value, FormatContext& ctx) const noexcept -> decltype(ctx.out()) {
    return core::format_fn(value, ctx);
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsDefaultFormattedEnumeration T, typename CharT>
template<class ParseContext>
constexpr auto std::formatter<T, CharT>::parse(ParseContext& ctx) noexcept -> decltype(ctx.begin()) {
    return ctx.begin();
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsDefaultFormattedEnumeration T, typename CharT>
template<class FormatContext>
auto std::formatter<T, CharT>::format(const T& value, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto&& out = ctx.out();
    if constexpr (requires {
                      { as_string(value) } -> meta::Is<string_view>;
                  }) {
        const auto strvalue = as_string(value);
        return format_to(out, "{}", strvalue);
    } else
        return format_to(out, "{}", as<Underlying>(value));
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsPointer T, typename CharT>
template<class FormatContext>
auto std::formatter<T, CharT>::format(const T& value, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto&& out = ctx.out();
    return format_to(out, "{:#0x}", std::bit_cast<std::uintptr_t>(std::to_address(value)));
}

/////////////////////////////////////
/////////////////////////////////////
template<typename CharT>
template<class FormatContext>
auto std::formatter<std::error_code, CharT>::format(const std::error_code& code, FormatContext& ctx) const
  -> decltype(ctx.out()) {
    auto&&     out     = ctx.out();
    const auto message = code.message();
    return format_to(out, "{}", message);
}

/////////////////////////////////////
/////////////////////////////////////
template<typename CharT>
template<class FormatContext>
auto std::formatter<std::errc, CharT>::format(const std::errc& code, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto&&     out     = ctx.out();
    const auto message = make_error_code(code).message();
    return format_to(out, "{}", message);
}
