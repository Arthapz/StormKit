// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef STORMKIT_TRY_EXPECTED_HPP
#define STORMKIT_TRY_EXPECTED_HPP

#if (defined(__clang__) or defined(__GNUC__))
    #define Try(m)                                                 \
        ({                                                         \
            auto res = (m);                                        \
            if (not res.has_value()) [[unlikely]]                  \
                return std::unexpected { std::move(res).error() }; \
            std::move(res).value();                                \
        })
    #define TryOr(m, t)                           \
        ({                                        \
            auto res = (m);                       \
            if (not res.has_value()) [[unlikely]] \
                t(std::move(res).error());        \
            std::move(res).value();               \
        })
    #define TryDiscard(m)                                                 \
        ({                                                                \
            auto res = (m).transform(stormkit::core::monadic::discard()); \
            if (not res.has_value()) [[unlikely]]                         \
                return std::unexpected { std::move(res).error() };        \
        })
    #define TryOrDiscard(m, t)                                            \
        ({                                                                \
            auto res = (m).transform(stormkit::core::monadic::discard()); \
            if (not res.has_value()) [[unlikely]]                         \
                t(std::move(res).error());                                \
        })

    #define Return return
#else
    #define Try(m)             co_await m
    #define TryOr(m, t)        co_await t(co_await m)
    #define TryDiscard(m)      co_await m.transform(stormkit::core::monadic::discard())
    #define TryOrDiscard(m, t) co_await m.transform(stormkit::core::monadic::discard())
    #define Return             co_return
#endif
#define TryAssert(m, msg)        TryOr(m, stormkit::core::monadic::assert(msg))
#define TryAssertDiscard(m, msg) TryOrDiscard(m, stormkit::core::monadic::assert(msg))
#endif
