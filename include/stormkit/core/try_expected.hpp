// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef STORMKIT_TRY_EXPECTED_HPP
#define STORMKIT_TRY_EXPECTED_HPP

#include <stormkit/core/platform_macro.hpp>

#if (defined(__clang__) or defined(__GNUC__))
    #define Try(try_expression)                                        \
        ({                                                             \
            auto res = (try_expression);                               \
            if (not res.has_value()) [[unlikely]]                      \
                return { std::unexpected { std::move(res.error()) } }; \
            *std::move(res);                                           \
        })
    #define TryOr(try_expression, or_closure)                                                                 \
        ({                                                                                                    \
            auto res = (try_expression);                                                                      \
            if (not res.has_value()) [[unlikely]] { return std::invoke(or_closure, std::move(res.error())); } \
            *std::move(res);                                                                                  \
        })
    #define TryTransform(try_expression, transform_closure)                                        \
        ({                                                                                         \
            auto res = (try_expression);                                                           \
            if (not res.has_value()) [[unlikely]] {                                                \
                return std::unexpected { std::invoke(transform_closure, std::move(res.error())) }; \
            }                                                                                      \
            *std::move(res);                                                                       \
        })
    #define TryAssert(try_expression, msg)                 \
        ({                                                 \
            auto res = (try_expression);                   \
            stormkit::core::ensures(res.has_value(), msg); \
            *std::move(res);                               \
        })

    #define Return return
#else
    #define Try(try_expression)                             co_await (try_expression)
    #define TryOr(try_expression, or_closure)               co_await (try_expression).or_else(or_closure)
    #define TryTransform(try_expression, transform_closure) co_await (try_expression).transform_error(transform_closure)
    #define TryAssert(try_expression, msg) co_await (try_expression).or_else(stormkit::core::monadic::assert(msg))

    #define Return co_return
#endif

#define CustomLoggedTry(try_expression, logger, msg)          \
    TryTransform(try_expression, [&](auto&& error) noexcept { \
        logger("{}\n    > reason: {}", msg, error);           \
        return error;                                         \
    })
#define CustomLoggedTryOr(try_expression, or_closure, logger, msg)      \
    TryOr(try_expression, [&]<typename Error>(Error&& error) noexcept { \
        logger("{}\n    > reason: {}", msg, error);                     \
        return std::invoke(or_closure, std::forward<Error>(error));     \
    })
#define CustomLoggedTryTransform(m, transform_closure, logger, msg)        \
    TryTransform(m, [&]<typename Error>(Error&& error) noexcept {          \
        logger("{}\n    > reason: {}", msg, error);                        \
        return std::invoke(transform_closure, std::forward<Error>(error)); \
    })
#define LoggedTry(try_expression, msg)               CustomLoggedTry(try_expression, elog, msg)
#define LoggedTryOr(try_expression, or_closure, msg) CustomLoggedTryOr(try_expression, or_closure, elog, msg)
#define LoggedTryTransform(try_expression, transform_closure, msg) \
    CustomLoggedTryTransform(try_expression, transform_closure, elog, msg)

#define TryLift(try_expression) TryOr(try_expression, stormkit::core::monadic::identity())

#define CustomLoggedTryLift(try_expression, logger, msg) \
    TryOr(try_expression, [&](auto&& error) noexcept {   \
        logger("{}\n    > reason: {}", msg, error);      \
        return error;                                    \
    })
#define LoggedTryLift(try_expression) CustomLoggedTryLift(try_expression, elog, msg)

#define DiscardTry(try_expression) \
    [[maybe_unused]]               \
    auto _ = Try(try_expression)
#define DiscardTryOr(try_expression, or_closure) \
    [[maybe_unused]]                             \
    auto _ = TryOr(try_expression, or_closure)
#define DiscardTryTransform(try_expression, transform_closure) \
    [[maybe_unused]]                                           \
    auto _ = TryTransform(try_expression, transform_closure)

#define CustomLoggedDiscardTry(try_expression, logger, msg) \
    [[maybe_unused]]                                        \
    auto _ = CustomLoggedTry(try_expression, logger, msg)
#define LoggedDiscardTry(try_expression, msg) CustomLoggedDiscardTry(try_expression, elog, msg)
#define CustomLoggedDiscardTryOr(try_expression, or_closure, logger, msg) \
    [[maybe_unused]]                                                      \
    auto _ = CustomLoggedTryOr(try_expression, or_closure, logger, msg)
#define LoggedDiscardTryOr(try_expression, or_closure, msg) CustomLoggedDiscardTryOr(try_expression, or_closure, elog, msg)
#define CustomLoggedDiscardTryTransform(try_expression, transform_closure, logger, msg) \
    [[maybe_unused]]                                                                    \
    auto _ = CustomLoggedTryTransform(try_expression, transform_closure, logger, msg)
#define LoggedDiscardTryTransform(try_expression, transform_closure, msg) \
    CustomLoggedDiscardTryTransform(try_expression, transform_closure, elog, msg)

#define DiscardTryLift(try_expression) \
    [[maybe_unused]]                   \
    auto _ = TryLift(try_expression)
#define CustomLoggedDiscardTryLift(try_expression, logger, msg) \
    [[maybe_unused]]                                            \
    auto _ = CustomLoggedTryLift(try_expression, logger, msg)
#define LoggedDiscardTryLift(try_expression) \
    [[maybe_unused]]                         \
    auto _ = LoggedTryLift(try_expression, msg)

#define DiscardTryAssert(try_expression, msg) \
    [[maybe_unused]]                          \
    auto _ = TryAssert(try_expression, msg)

#endif
