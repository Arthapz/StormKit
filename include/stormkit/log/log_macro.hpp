// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef STORMKIT_LOG_MACRO_HPP
#define STORMKIT_LOG_MACRO_HPP

#include <stormkit/core/platform_macro.hpp>

#define NAMED_LOGGER(NAME, module_chars)                              \
    namespace {                                                       \
        [[maybe_unused]]                                              \
        constexpr auto NAME = stormkit::log::Module { module_chars }; \
    }

#define LOGGER_FUNC(LOG_MODULE)                                                                                          \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto dlog(std::format_string<Ts...> format_string, Ts&&... args) noexcept -> void { \
        LOG_MODULE.dlog(std::move(format_string), std::forward<Ts>(args)...);                                          \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto ilog(std::format_string<Ts...> format_string, Ts&&... args) noexcept -> void { \
        LOG_MODULE.ilog(std::move(format_string), std::forward<Ts>(args)...);                                          \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto wlog(std::format_string<Ts...> format_string, Ts&&... args) noexcept -> void { \
        LOG_MODULE.wlog(std::move(format_string), std::forward<Ts>(args)...);                                          \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto elog(std::format_string<Ts...> format_string, Ts&&... args) noexcept -> void { \
        LOG_MODULE.elog(std::move(format_string), std::forward<Ts>(args)...);                                          \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto flog(std::format_string<Ts...> format_string, Ts&&... args) noexcept -> void { \
        LOG_MODULE.flog(std::move(format_string), std::forward<Ts>(args)...);                                          \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto dlog_runtime(std::string_view format_string, Ts&&... args) noexcept -> void {    \
        LOG_MODULE.dlog_runtime(std::move(format_string), std::forward<Ts>(args)...);                                  \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto ilog_runtime(std::string_view format_string, Ts&&... args) noexcept -> void {    \
        LOG_MODULE.ilog_runtime(std::move(format_string), std::forward<Ts>(args)...);                                  \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto wlog_runtime(std::string_view format_string, Ts&&... args) noexcept -> void {    \
        LOG_MODULE.wlog_runtime(std::move(format_string), std::forward<Ts>(args)...);                                  \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto elog_runtime(std::string_view format_string, Ts&&... args) noexcept -> void {    \
        LOG_MODULE.elog_runtime(std::move(format_string), std::forward<Ts>(args)...);                                  \
    }                                                                                                                    \
    template<class... Ts>                                                                                              \
    STORMKIT_FORCE_INLINE inline auto flog_runtime(std::string_view format_string, Ts&&... args) noexcept -> void {    \
        LOG_MODULE.flog_runtime(std::move(format_string), std::forward<Ts>(args)...);                                  \
    }

#define LOGGER(module)               \
    NAMED_LOGGER(LOG_MODULE, module) \
    LOGGER_FUNC(LOG_MODULE)

#define IN_MODULE_NAMED_LOGGER(NAME, module_chars) \
    [[maybe_unused]]                               \
    inline constexpr auto NAME = stormkit::log::Module { module_chars };

#define IN_MODULE_LOGGER(module)               \
    IN_MODULE_NAMED_LOGGER(LOG_MODULE, module) \
    LOGGER_FUNC(LOG_MODULE)

#endif
