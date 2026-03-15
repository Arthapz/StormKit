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
    #define TryTransformError(m, t)                                   \
        ({                                                            \
            auto res = (m);                                           \
            if (not res.has_value()) [[unlikely]]                     \
                return std::unexpected { t(std::move(res).error()) }; \
            std::move(res).value();                                   \
        })

    #define TryDiscard(m)                                                 \
        ({                                                                \
            auto res = (m).transform(stormkit::core::monadic::discard()); \
            if (not res.has_value()) [[unlikely]]                         \
                return std::unexpected { std::move(res).error() };        \
        })
    #define TryDiscardOr(m, t)                                            \
        ({                                                                \
            auto res = (m).transform(stormkit::core::monadic::discard()); \
            if (not res.has_value()) [[unlikely]]                         \
                t(std::move(res).error());                                \
        })
    #define TryDiscardTransformError(m, t)                                \
        ({                                                                \
            auto res = (m).transform(stormkit::core::monadic::discard()); \
            if (not res.has_value()) [[unlikely]]                         \
                return std::unexpected { t(std::move(res).error()) };     \
        })

    #define Return return
#else
    #define Try(m)                         co_await m
    #define TryOr(m, t)                    co_await m.transform_error(t)
    #define TryTransformError(m, t)        TryOr(m, t)
    #define TryDiscard(m)                  co_await m.transform(stormkit::core::monadic::discard())
    #define TryDiscardOr(m, t)             co_await m.transform(stormkit::core::monadic::discard()).transform_error(t)
    #define TryDiscardTransformError(m, t) TryDiscardOr(m, t)
    #define Return                         co_return
#endif
#define TryAssert(m, msg)        TryOr(m, stormkit::core::monadic::assert(msg))
#define TryDiscardAssert(m, msg) TryDiscardOr(m, stormkit::core::monadic::assert(msg))
#endif
