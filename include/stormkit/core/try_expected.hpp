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
    #define Ret(x)    return x
    #define RetErr(x) return std::unexpected { x };
#else
    #define Try(x)    co_await x
    #define Ret(x)    co_return x
    #define RetErr(x) co_return std::unexpected { x };
#endif

#endif
