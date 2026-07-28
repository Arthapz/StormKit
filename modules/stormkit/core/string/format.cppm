// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/format_macro.hpp>

export module stormkit.core:string.format;

import std;

import :meta;
import :typesafe.integer;
import :typesafe.byte;
import :typesafe.safecasts;
import :utils.time;
import :string.operations;

export {
    namespace stormkit { inline namespace core {

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
            static constexpr auto operator()(const meta::HasFormatAs auto& value, auto& ctx) noexcept -> decltype(ctx.out());
        } format_fn = {};

        auto           format_as(const auto&, auto& ctx) noexcept -> decltype(ctx.out()) = delete;
        constexpr auto format_as(fsecond, auto& ctx) noexcept -> decltype(ctx.out());
    }} // namespace stormkit::core

    constexpr auto format_as(byte, auto& ctx) noexcept -> decltype(ctx.out());

    template<stormkit::meta::HasFormatAs T, typename CharT>
    struct std::formatter<T, CharT> {
        [[nodiscard]]
        STORMKIT_FORCE_INLINE
        constexpr auto parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
            return ctx.begin();
        }

        [[nodiscard]]
        constexpr auto format(const T&, auto& ctx) const noexcept -> decltype(ctx.out());
    };

    template<stormkit::meta::IsDefaultFormattedEnumeration T, typename CharT>
    struct std::formatter<T, CharT> {
        [[nodiscard]]
        STORMKIT_FORCE_INLINE
        constexpr auto parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
            return ctx.begin();
        }

        [[nodiscard]]
        constexpr auto format(const T& value, auto& ctx) const noexcept -> decltype(ctx.out()) {
            using namespace stormkit;

            auto&& out = ctx.out();
            if constexpr (requires {
                              { as_string(value) } -> meta::Is<string_view>;
                          }) {
                const auto strvalue = as_string(value);
                return format_to(out, "{}", strvalue);
            } else
                return format_to(out, "{}", as<Underlying>(value));
        }
    };

    template<stormkit::meta::IsPointer T, typename CharT>
    struct std::formatter<T, CharT>: public formatter<std::uintptr_t, CharT> {
        [[nodiscard]]
        STORMKIT_FORCE_INLINE
        constexpr auto format(const T& value, auto& ctx) const noexcept -> decltype(ctx.out()) {
            auto&& out = ctx.out();
            return format_to(out, "{:#0x}", std::bit_cast<std::uintptr_t>(std::to_address(value)));
        }
    };

    template<typename CharT>
    struct std::formatter<std::error_code, CharT>: public formatter<basic_string_view<CharT>, CharT> {
        [[nodiscard]]
        constexpr auto format(const std::error_code&, auto& ctx) const noexcept -> decltype(ctx.out());
    };

    template<typename CharT>
    struct std::formatter<std::errc, CharT>: public formatter<basic_string_view<CharT>, CharT> {
        [[nodiscard]]
        constexpr auto format(const std::errc& code, auto& ctx) const noexcept -> decltype(ctx.out());
    };
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

/////////////////////////////////////
/////////////////////////////////////
STORMKIT_FORCE_INLINE
constexpr auto format_as(byte value, auto& ctx) noexcept -> decltype(ctx.out()) {
    auto&& out = ctx.out();
    return std::format_to(out, "{:#x}", narrow<u8>(value));
}

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto FormatFN::operator()(const meta::HasFormatAs auto& value, auto& ctx) noexcept -> decltype(ctx.out()) {
        return format_as(value, ctx);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto format_as(fsecond value, auto& ctx) noexcept -> decltype(ctx.out()) {
        auto&& out = ctx.out();
        return std::format_to(out, "{}", value.count());
    }
}} // namespace stormkit::core

using namespace stormkit;

/////////////////////////////////////
/////////////////////////////////////
// template<meta::HasFormatAs T, typename CharT>
// constexpr auto std::formatter<T, CharT>::parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
//     return ctx.begin();
// }

/////////////////////////////////////
/////////////////////////////////////
template<meta::HasFormatAs T, typename CharT>
constexpr auto std::formatter<T, CharT>::format(const T& value, auto& ctx) const noexcept -> decltype(ctx.out()) {
    return core::format_fn(value, ctx);
}

/////////////////////////////////////
/////////////////////////////////////
// template<meta::IsDefaultFormattedEnumeration T, typename CharT>
// constexpr auto std::formatter<T, CharT>::parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
//     return ctx.begin();
// }

/////////////////////////////////////
/////////////////////////////////////
// template<meta::IsDefaultFormattedEnumeration T, typename CharT>
// constexpr auto std::formatter<T, CharT>::format(const T& value, auto& ctx) const noexcept -> decltype(ctx.out()) {
//     auto&& out = ctx.out();
//     if constexpr (requires {
//                       { as_string(value) } -> meta::Is<string_view>;
//                   }) {
//         const auto strvalue = as_string(value);
//         return format_to(out, "{}", strvalue);
//     } else
//         return format_to(out, "{}", as<Underlying>(value));
// }

/////////////////////////////////////
/////////////////////////////////////
// template<meta::IsPointer T, typename CharT>
// constexpr auto std::formatter<T, CharT>::format(const T& value, auto& ctx) const noexcept -> decltype(ctx.out()) {
//     auto&& out = ctx.out();
//     return format_to(out, "{:#0x}", std::bit_cast<std::uintptr_t>(std::to_address(value)));
// }

/////////////////////////////////////
/////////////////////////////////////
template<typename CharT>
constexpr auto std::formatter<std::error_code, CharT>::format(const std::error_code& code, auto& ctx) const noexcept
  -> decltype(ctx.out()) {
    auto&&     out     = ctx.out();
    const auto message = code.message();
    return format_to(out, "{}", message);
}

/////////////////////////////////////
/////////////////////////////////////
template<typename CharT>
constexpr auto std::formatter<std::errc, CharT>::format(const std::errc& code, auto& ctx) const noexcept -> decltype(ctx.out()) {
    auto&&     out     = ctx.out();
    const auto message = make_error_code(code).message();
    return format_to(out, "{}", message);
}
