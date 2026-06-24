// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#define _CRT_SECURE_NO_WARNINGS

#include <stormkit/core/platform_macro.hpp>

#include <errno.h>

#include <cstring>

#include <string>

#include <system_error>

export module stormkit.core:errors;

import std;

import :string.aliases;
import :containers.aliases;

export {
    namespace stormkit { inline namespace core {
#if (defined(__clang__) or defined(__GNUC__))
        template<typename Val, typename Err>
        using Expected = std::expected<Val, Err>;
#else
        template<typename VAL, typename ERR>
        class Expected: public std::expected<VAL, ERR> {
          public:
            using std::expected<VAL, ERR>::expected;

            struct promise_type {
                constexpr auto initial_suspend() const noexcept -> std::suspend_never { return {}; }

                constexpr auto final_suspend() const noexcept -> std::suspend_never { return {}; }

                constexpr auto get_return_object() noexcept -> Expected { return Expected { this }; }

                template<typename... Args>
                constexpr auto return_value(Args&&... args) noexcept {
                    assert(expected_ptr != nullptr);
                    *expected_ptr = Expected { std::in_place, std::forward<Args>(args)... };
                }

                constexpr auto return_value(ERR error) noexcept {
                    assert(expected_ptr != nullptr);
                    *expected_ptr = Expected {
                        std::unexpected<ERR> { std::in_place, std::move(error) }
                    };
                }

                [[noreturn]]
                auto unhandled_exception() const noexcept {
                    std::abort();
                }

                Expected* expected_ptr = nullptr;
            };

            constexpr Expected(promise_type* promise) noexcept : Expected {} { promise->expected_ptr = this; }

            constexpr auto await_ready() const noexcept -> bool { return this->has_value(); }

            constexpr auto await_resume() noexcept -> VAL { return std::move(*this).value(); }

            template<typename Promise>
            constexpr auto await_suspend(std::coroutine_handle<Promise> handle) noexcept -> bool {
                handle.promise().return_value(this->error());
                handle.destroy();
                return true;
            }
        };
#endif

        template<typename T>
        struct Error {
            T code;
        };

        template<>
        struct Error<std::errc> {
            static auto from_errno() noexcept -> Error<std::errc>;
            std::errc   code;
        };

        using SystemError = Error<std::errc>;

        template<typename T>
        struct DecoratedError {
            Error<T> error;
            string   message = "<MISSING DESCRIPTION>";
        };

        template<>
        struct DecoratedError<std::errc> {
            explicit DecoratedError(Error<std::errc> _error) noexcept;

            Error<std::errc> error;
            string           message;
        };

        auto to_string(const SystemError& error) noexcept -> string;
    }} // namespace stormkit::core

    namespace std {
        template<class T, class CharT>
        struct formatter<stormkit::core::Error<T>, CharT>: formatter<T, CharT> {
            template<class FormatContext>
            auto format(const stormkit::core::Error<T>&, FormatContext& ctx) const -> decltype(ctx.out());
        };

        template<class T, class CharT>
        struct formatter<stormkit::core::DecoratedError<T>, CharT> {
            template<class ParseContext>
            constexpr auto parse(ParseContext& ctx) noexcept -> decltype(ctx.begin());

            template<class FormatContext>
            auto format(const stormkit::core::DecoratedError<T>&, FormatContext& ctx) const -> decltype(ctx.out());
        };
    } // namespace std
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Error<std::errc>::from_errno() noexcept -> Error<std::errc> {
        return SystemError { std::errc { errno } };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DecoratedError<std::errc>::DecoratedError(Error<std::errc> _error) noexcept
        : error { _error }, message { to_string(error) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto to_string(const SystemError& error) noexcept -> string {
        const auto code = static_cast<int>(error.code);
#ifdef STORMKIT_OS_WINDOWS
        thread_local auto STRERROR_BUFFER = array<char, 512> {};
        strerror_s(stdr::data(STRERROR_BUFFER), stdr::size(STRERROR_BUFFER), code);
        return string { stdr::data(STRERROR_BUFFER) };
#else
        return std::strerror(code);
#endif
    }
}} // namespace stormkit::core

namespace std {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, class CharT>
    template<class FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto formatter<stormkit::core::Error<T>, CharT>::format(const stormkit::core::Error<T>& error, FormatContext& ctx)
      const -> decltype(ctx.out()) {
        return formatter<T>::format(error.code, ctx);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, class CharT>
    template<class ParseContext>
    STORMKIT_FORCE_INLINE
    constexpr auto formatter<stormkit::core::DecoratedError<T>, CharT>::parse(ParseContext& ctx) noexcept
      -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, class CharT>
    template<class FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto formatter<stormkit::core::DecoratedError<T>, CharT>::format(const stormkit::core::DecoratedError<T>& error,
                                                                            FormatContext& ctx) const -> decltype(ctx.out()) {
        auto&& out = ctx.out();
        return format_to("message: {}, code: {}", error.message, error.error.code);
    }
} // namespace std
