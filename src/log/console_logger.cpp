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
        constexpr auto StyleMap = frozen::make_unordered_map<Severity, console_style>({
          { Severity::INFO,    console_style { .fg = console_color::GREEN, .modifiers = style_modifier::INVERSE }   },
          { Severity::WARNING, console_style { .fg = console_color::MAGENTA, .modifiers = style_modifier::INVERSE } },
          { Severity::ERROR,   console_style { .fg = console_color::YELLOW, .modifiers = style_modifier::INVERSE }  },
          { Severity::FATAL,   console_style { .fg = console_color::RED, .modifiers = style_modifier::INVERSE }     },
          { Severity::DEBUG,   console_style { .fg = console_color::CYAN, .modifiers = style_modifier::INVERSE }    },
        });

        constexpr auto format_string_with_module = "{}[{}, {:%S}, {}]{} {}"sv;
        constexpr auto format_string             = "{}[{}, {:%S}]{} {}"sv;
    } // namespace

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
    auto ConsoleLogger::write(Severity severity, const Module& module, std::string_view str) noexcept -> void {
        const auto now      = LogClock::now();
        const auto time     = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time);
        const auto is_error = severity == Severity::ERROR or severity == Severity::FATAL;
        const auto out      = (is_error) ? get_stderr() : get_stdout();

        if (stdr::empty(module.name)) std::println(out, format_string, StyleMap.at(severity), severity, time, ecma48::RESET, str);
        else
            std::println(out, format_string_with_module, StyleMap.at(severity), severity, time, module.name, ecma48::RESET, str);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::flush() noexcept -> void {
        std::fflush(get_stdout());
        std::fflush(get_stderr());
    }
} // namespace stormkit::log
