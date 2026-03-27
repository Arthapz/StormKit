// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <cstdio>

module stormkit.log;

import std;
import frozen;

import stormkit.core;

using namespace std::literals;

namespace stdr = std::ranges;

namespace stormkit::log {
    namespace {
        constexpr auto StyleMap = frozen::make_unordered_map<Severity, ConsoleStyle>({
          { Severity::INFO,    ConsoleStyle { .fg = ConsoleColor::GREEN, .modifiers = StyleModifier::INVERSE }   },
          { Severity::WARNING, ConsoleStyle { .fg = ConsoleColor::MAGENTA, .modifiers = StyleModifier::INVERSE } },
          { Severity::ERROR,   ConsoleStyle { .fg = ConsoleColor::YELLOW, .modifiers = StyleModifier::INVERSE }  },
          { Severity::FATAL,   ConsoleStyle { .fg = ConsoleColor::RED, .modifiers = StyleModifier::INVERSE }     },
          { Severity::DEBUG,   ConsoleStyle { .fg = ConsoleColor::CYAN, .modifiers = StyleModifier::INVERSE }    },
        });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::ConsoleLogger(LogClock::time_point start) noexcept : Logger { std::move(start) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::ConsoleLogger(LogClock::time_point start, Severity log_level) noexcept
        : Logger { std::move(start), log_level } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::write(Severity severity, const Module& module, czstring str) noexcept -> void {
        const auto now      = LogClock::now();
        const auto time     = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time);
        const auto is_error = severity == Severity::ERROR or severity == Severity::FATAL;
        const auto out      = (is_error) ? get_stderr() : get_stdout();

        const auto header = [&severity, &module, &time] noexcept {
            const auto severity_str = replace(as_string(severity), "Severity::", "");
            if (std::empty(module.name)) return std::format("[{}, {:%S}]", severity_str, time);
            else
                return std::format("[{}, {:%S}, {}]", severity_str, time, module.name);
        }();

        const auto prefixed_string = [&header, str] noexcept {
            const auto header_length = stdr::size(header) + 1;

            auto prefix = string {};
            prefix.resize(header_length + 1, ' ');
            prefix.front() = '\n';
            return replace(str, "\n", prefix);
        }();

        const auto styled_header = std::format("{} ", StyleMap.at(severity) | header);
        std::println(out, "{}{}", styled_header, prefixed_string);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::flush() noexcept -> void {
        std::fflush(get_stdout());
        std::fflush(get_stderr());
    }
} // namespace stormkit::log
