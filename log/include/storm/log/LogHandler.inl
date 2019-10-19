// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <fmt/core.h>
#include <queue>
#include <storm/core/Strings.hpp>

namespace storm::log {
    template<class T, typename... Args>
    void LogHandler::setupLogger(Args &&... param_args) {
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        if (m_logger) time_point = m_logger->startTime();

        auto logger = std::make_unique<T>(std::move(time_point), std::forward<Args>(param_args)...);

        setupLogger(std::move(logger));
    }

    template<typename... Args>
    void LogHandler::log(Severity severity,
                         Module module,
                         std::string format_string,
                         Args &&... param_args) {
        auto memory_buffer = fmt::memory_buffer {};
        fmt::format_to(memory_buffer, format_string, std::forward<Args>(param_args)...);

        logger().write(severity, module, std::data(fmt::to_string(memory_buffer)));
    }

    template<typename... Args>
    inline void
        LogHandler::log(Severity severity, std::string format_string, Args &&... param_args) {
        log(severity, ""_module, std::move(format_string), std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    inline void LogHandler::dlog(Args &&... param_args) {
        log(Severity::Debug, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    inline void LogHandler::ilog(Args &&... param_args) {
        log(Severity::Info, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    inline void LogHandler::wlog(Args &&... param_args) {
        log(Severity::Warning, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    inline void LogHandler::elog(Args &&... param_args) {
        log(Severity::Error, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    inline void LogHandler::flog(Args &&... param_args) {
        log(Severity::Fatal, std::forward<Args>(param_args)...);
    }

} // namespace storm::log
