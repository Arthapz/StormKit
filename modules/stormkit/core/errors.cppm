// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/contract_macro.hpp>

// #include <status-code/error.hpp>
// #include <status-code/status_code.hpp>
// #include <status-code/std_error_code.hpp>

#ifdef STORMKIT_OS_WINDOWS
    #include <stormkit/core/platform/windows.hpp>
#endif

export module stormkit.core:errors;

import std;

import :utils.contract;

import :string.aliases;
import :containers.aliases;
export import :status_code;
import :typesafe.integer;
import :string.format;

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
                    EXPECTS(expected_ptr != nullptr);
                    *expected_ptr = Expected { std::in_place, std::forward<Args>(args)... };
                }

                constexpr auto return_value(ERR error) noexcept {
                    EXPECTS(expected_ptr != nullptr);
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
        using Error = system_error2::system_code;

        namespace error {
#ifdef STORMKIT_OS_WINDOWS
            auto from_win32() noexcept -> Error;
            auto from_ntstatus() noexcept -> Error;
#endif
            auto from_errno() noexcept -> Error;
            auto from_stderrc(std::errc code) noexcept -> Error;
        } // namespace error

        template<typename T>
        using Result = Expected<T, Error>;
    }} // namespace stormkit::core
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;

namespace stormkit { inline namespace core {

    namespace error {
#ifdef STORMKIT_OS_WINDOWS
        STORMKIT_FORCE_INLINE
        inline auto from_win32() noexcept -> Error {
            return system_error2::win32_code { GetLastError() };
        }

        STORMKIT_FORCE_INLINE
        inline auto from_ntstatus(long status) noexcept -> Error {
            return system_error2::nt_code { status };
        }
#endif

        STORMKIT_FORCE_INLINE
        inline auto from_errno() noexcept -> Error {
            return system_error2::posix_code { errno };
        }

        STORMKIT_FORCE_INLINE
        inline auto from_stderrc(std::errc code) noexcept -> Error {
            return Error { system_error2::posix_code { static_cast<i32>(code) } };
        }
    } // namespace error

    //    ////////////////////////////////////////
    //    ////////////////////////////////////////
    //    STORMKIT_FORCE_INLINE
    //    inline auto to_string(const System_error& error) noexcept -> string {
    //        const auto code = static_cast<int>(error.code);
    // #ifdef STORMKIT_OS_WINDOWS
    //        thread_local auto STRERROR_BUFFER = array<char, 512> {};
    //        strerror_s(stdr::data(STRERROR_BUFFER), stdr::size(STRERROR_BUFFER), code);
    //        return string { stdr::data(STRERROR_BUFFER) };
    // #else
    //        return std::strerror(code);
    // #endif
    //    }

    // template<typename FormatContext>
    // constexpr auto format_as(const Error& point, FormatContext& ctx) -> decltype(ctx.out()) {
    //     return std::format_to(ctx.out(), "[]");
    // }
}} // namespace stormkit::core
