// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Logger::setLogLevel(Severity log_level) { m_log_level = log_level; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const LogClock::time_point &Logger::startTime() const noexcept { return m_start_time; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Severity &Logger::logLevel() const noexcept { return m_log_level; }
} // namespace storm::log
