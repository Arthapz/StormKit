// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.string.format;

import std;

import stormkit.core.types;

import stormkit.core.meta.tag_invoke;
import stormkit.core.meta.concepts;
import stormkit.core.meta.algorithms;
import stormkit.core.meta.type_manipulation;
import stormkit.core.meta.type_query;
import stormkit.core.typesafe.safecasts;
import stormkit.core.typesafe.ref_ptr;
import stormkit.core.errors;

template<typename T, typename CharT = char>
concept has_as_string = requires(const T& value) {
    { as<stormkit::core::basic_string<CharT>>(value) } -> stormkit::core::meta::is<stormkit::core::basic_string<CharT>>;
};

export {
    namespace stormkit { inline namespace core {
        namespace meta {
            template<meta::enumeration T>
            inline constexpr auto DISABLE_DEFAULT_FORMATTER_FOR_ENUM = false;

            template<typename T>
            inline constexpr auto ENABLE_AS_STRING_AS_FORMATTER = false;
        } // namespace meta

        template<typename CharT>
        struct format_as_fn {
          private:
            template<typename T, typename FormatContext>
            using invoke_result = meta::tag_invoke_result<format_as_fn<CharT>, const T&, FormatContext&>;

            template<typename T, typename FormatContext>
            static constexpr auto IS_TAG_INVOKABLE = meta::tag_invocable<format_as_fn<CharT>, const T&, FormatContext&>;

          public:
            template<typename T, typename FormatContext>
            [[nodiscard]]
            static constexpr auto operator()(const T& value, FormatContext& ctx) noexcept -> invoke_result<T, FormatContext>
                requires(not IS_TAG_INVOKABLE<T, FormatContext>
                         and meta::ENABLE_AS_STRING_AS_FORMATTER<T>
                         and has_as_string<T, CharT>);

            template<typename T, typename FormatContext>
            [[nodiscard]]
            static constexpr auto operator()(const T& value, FormatContext& ctx) noexcept -> invoke_result<T, FormatContext>
                requires(IS_TAG_INVOKABLE<T, FormatContext>);
        };

        namespace meta {
            template<typename T, typename CharT>
            concept has_format_as = requires(const T& value) {
                {
                    format_as_fn<CharT> {}(value, std::declval<std::format_context&>())
                } -> meta::is<decltype(std::declval<std::format_context&>().out())>;
            };

            namespace plain {
                template<typename T, typename CharT>
                concept has_format_as = apply_to<T, meta::has_format_as, CharT>;
            }
        } // namespace meta

        template<typename CharT, typename FormatContext>
        constexpr auto tag_invoke(format_as_fn<CharT>, byte value, FormatContext& ctx) -> decltype(ctx.out());

        template<typename CharT, typename FormatContext, meta::negate<meta::raw_indirection> T>
        constexpr auto tag_invoke(format_as_fn<CharT>, ref_ptr<T> value, FormatContext& ctx) -> decltype(ctx.out());

        template<typename CharT, typename FormatContext>
        constexpr auto tag_invoke(format_as_fn<CharT>, meta::in<system_code> value, FormatContext& ctx) -> decltype(ctx.out());
    }} // namespace stormkit::core

    template<typename T, typename CharT>
        requires(stormkit::core::meta::plain::has_format_as<T, CharT>)
    struct std::formatter<T, CharT> {
        [[nodiscard]]
        constexpr auto parse(auto& ctx) noexcept -> decltype(ctx.begin());

        [[nodiscard]]
        constexpr auto format(const T&, auto& ctx) const noexcept -> decltype(ctx.out());
    };
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    template<typename CharT>
    template<typename T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    constexpr auto format_as_fn<CharT>::operator()(const T& value, FormatContext& ctx) noexcept -> invoke_result<T, FormatContext>
        requires(not IS_TAG_INVOKABLE<T, FormatContext> and meta::ENABLE_AS_STRING_AS_FORMATTER<T> and has_as_string<T, CharT>)
    {
        return std::format_to(ctx.out(), "{}", as<basic_string<CharT>>(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename CharT>
    template<typename T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    constexpr auto format_as_fn<CharT>::operator()(const T& value, FormatContext& ctx) noexcept -> invoke_result<T, FormatContext>
        requires(IS_TAG_INVOKABLE<T, FormatContext>)
    {
        return tag_invoke(format_as_fn<CharT> {}, value, ctx);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename CharT, typename FormatContext>
    constexpr auto tag_invoke(format_as_fn<CharT>, byte value, FormatContext& ctx) -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "{:0x}", value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename CharT, typename FormatContext, meta::negate<meta::raw_indirection> T>
    constexpr auto tag_invoke(format_as_fn<CharT>, ref_ptr<T> value, FormatContext& ctx) -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "{:0x}", value.get());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename CharT, typename FormatContext>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(format_as_fn<CharT>, meta::in<system_code> error, FormatContext& ctx) -> decltype(ctx.out()) {
        return std::format_to(ctx.out(),
                              "{:#x} ({})",
                              static_cast<unsigned long long>(error.value()),
                              std::string_view { error.message() });
    }
}} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<typename T, typename CharT>
    requires(stormkit::core::meta::plain::has_format_as<T, CharT>)
constexpr auto std::formatter<T, CharT>::parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
    return ctx.begin();
}

/////////////////////////////////////
/////////////////////////////////////
template<typename T, typename CharT>
    requires(stormkit::core::meta::plain::has_format_as<T, CharT>)
constexpr auto std::formatter<T, CharT>::format(const T& value, auto& ctx) const noexcept -> decltype(ctx.out()) {
    return stormkit::core::format_as_fn<CharT> {}(value, ctx);
}
