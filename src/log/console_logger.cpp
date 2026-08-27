// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <cstdio>

module stormkit.log;

import std;

import stormkit.core;

using namespace std::literals;

namespace stdr = std::ranges;

namespace stormkit::log {
    namespace {
        constexpr auto StyleMap = make_static_hash_map<severity, console_style>({
          { severity::INFO,    console_style { .fg = console_color::GREEN, .modifiers = style_modifier::INVERSE }   },
          { severity::WARNING, console_style { .fg = console_color::MAGENTA, .modifiers = style_modifier::INVERSE } },
          { severity::ERROR,   console_style { .fg = console_color::YELLOW, .modifiers = style_modifier::INVERSE }  },
          { severity::FATAL,   console_style { .fg = console_color::RED, .modifiers = style_modifier::INVERSE }     },
          { severity::DEBUG,   console_style { .fg = console_color::CYAN, .modifiers = style_modifier::INVERSE }    },
        });

        constexpr auto format_string_with_module = "{}[{}, {:%S}, {}]{} {}"sv;
        constexpr auto format_string             = "{}[{}, {:%S}]{} {}"sv;
    } // namespace

    ////////////////////////////////////////
    ////////////////////////////////////////
    console_logger::console_logger(clock_type::time_point start) noexcept : logger { std::move(start) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    console_logger::console_logger(clock_type::time_point start, severity log_level) noexcept
        : logger { std::move(start), log_level } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto console_logger::write(severity severity, const module& module, std::string_view str) noexcept -> void {
        const auto now      = clock_type::now();
        const auto time     = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time);
        const auto is_error = severity == severity::ERROR or severity == severity::FATAL;
        const auto out      = (is_error) ? get_stderr() : get_stdout();

        if (stdr::empty(module.name)) std::println(out, format_string, StyleMap.at(severity), severity, time, ecma48::RESET, str);
        else
            std::println(out, format_string_with_module, StyleMap.at(severity), severity, time, module.name, ecma48::RESET, str);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto console_logger::flush() noexcept -> void {
        std::fflush(get_stdout());
        std::fflush(get_stderr());
    }
} // namespace stormkit::log
