// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Configure.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Concepts.hpp>

#if __has_include(<format>) && !defined(STORMKIT_OS_MACOS) && !defined(STORMKIT_GEN_DOC)
    #include <format>
namespace storm::core {
    template<class ... Args>
    auto format(std::string_view f, const Args& ... args) -> std::string {
        return std::format(f, args...);
    }

    template<class OutputIt, class ... Args>
    auto format_to(OutputIt it, std::string_view f, const Args& ... args) -> OutputIt {
        return std::format_to(it, f, args...);
    }

} // namespace storm::core

#define CUSTOM_FORMAT(TYPE, OUTPUT_STR, ...)                          \
        template<class CharT>                                                    \
        struct std::formatter<TYPE, CharT> : std::formatter<std::string, CharT> {   \
            template<typename FormatContext>                          \
            auto format(const TYPE &data, FormatContext &ctx) {       \
                return std::formatter<std::string, CharT>::format(std::format(OUTPUT_STR, __VA_ARGS__), ctx); \
            }                                                         \
        };

template <storm::core::Enumeration Enum, class CharT>
struct std::formatter<Enum, CharT> : std::formatter<std::underlying_type_t<Enum>, CharT> {
    template <typename FormatContext>
    auto format(Enum e, FormatContext &ctx) {
        using underlying_type = std::underlying_type_t<Enum>;
        return std::formatter<underlying_type, CharT>::format(static_cast<underlying_type>(e), ctx);
    }
};
#elif __has_include(<experimental/format>)  && !defined(STORMKIT_GEN_DOC)
    #include <experimental/span>
namespace storm::core {
    template<class ... Args>
    auto format(Args&& ... args) -> decltype(std::format(std::experimental::forward<Args>(args)...)) {
        return std::experimental::format(std::forward<Args>(args)...);
    }
    template<class ... Args>
    auto format_to(Args&& ... args) -> decltype(std::experimental::format_to(std::forward<Args>(args)...)) {
        return std::experimental::format_to(std::forward<Args>(args)...);
    }
} // namespace storm::core
    #define CUSTOM_FORMAT(TYPE, OUTPUT_STR, ...)                          \
    namespace std::experimental {                                                   \
        template<>                                                    \
        struct formatter<TYPE> {                                      \
            template<typename ParseContext>                           \
            constexpr auto parse(ParseContext &ctx) {                 \
                return ctx.begin();                                   \
            }                                                         \
            template<typename FormatContext>                          \
            auto format(const TYPE &data, FormatContext &ctx) {       \
                return format_to(ctx.out(), OUTPUT_STR, __VA_ARGS__); \
            }                                                         \
        };                                                            \
    }
#elif __has_include(<fmt/format.h>)
    #include <fmt/format.h>
namespace storm::core {
    template<class ... Args>
    auto format(std::string_view f, const Args& ... args) -> std::string {
        return fmt::format(f, args...);
    }

    template<class OutputIt, class ... Args>
    auto format_to(OutputIt it, std::string_view f, const Args& ... args) -> OutputIt {
        return fmt::format_to(it, f, args...);
    }
} // namespace storm::core

    #define CUSTOM_FORMAT(TYPE, OUTPUT_STR, ...)                          \
    namespace fmt {                                                   \
        template<>                                                    \
        struct formatter<TYPE> {                                      \
            template<typename ParseContext>                           \
            constexpr auto parse(ParseContext &ctx) {                 \
                return ctx.begin();                                   \
            }                                                         \
            template<typename FormatContext>                          \
            auto format(const TYPE &data, FormatContext &ctx) {       \
                return format_to(ctx.out(), OUTPUT_STR, __VA_ARGS__); \
            }                                                         \
        };                                                            \
    }

#endif
