// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>

module stormkit.log;

import std;

import stormkit.core;

namespace stdr = std::ranges;

namespace stormkit::log {
    namespace {
        constexpr auto DEFAULT_LOG_MASK = Severity::INFO | Severity::ERROR | Severity::FATAL | Severity::WARNING;

        constinit Logger* logger = nullptr;

        constinit auto debug_enabled = false;
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto parse_args(array_view<const string_view> args) noexcept -> void {
        debug_enabled = stdr::find_if(args, [](auto&& v) { return v == "--debug" or v == "-d"; }) != stdr::cend(args);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::Logger(LogClock::time_point start_time) noexcept : Logger { std::move(start_time), DEFAULT_LOG_MASK } {
        EXPECTS(not logger);

        if (debug_enabled) m_severity_mask |= Severity::DEBUG;

        logger = this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::Logger(LogClock::time_point start_time, Severity log_level) noexcept
        : m_start_time { std::move(start_time) }, m_severity_mask { log_level } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::~Logger() noexcept {
        logger = nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Logger::has_logger() noexcept -> bool {
        if (logger) [[likely]]
            return true;
        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Logger::instance() noexcept -> Logger& {
        EXPECTS(logger);

        return *logger;
    }
} // namespace stormkit::log
