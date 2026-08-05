// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#ifndef STORMKIT_ASSERT
    #define STORMKIT_ASSERT 1
#endif

module stormkit.core.contract;

import std;

import stormkit.core.console;
import stormkit.core.string;
import stormkit.core.typesafe.flags;

namespace stormkit { inline namespace core {
    static auto mutex = std::mutex {};

    // extern constexpr auto as_string(Assert_type value) noexcept -> string_view;

    /////////////////////////////////////
    /////////////////////////////////////
    // STORMKIT_FORCE_INLINE STORMKIT_CONST
    // static constexpr auto as_string(Assert_type value) noexcept -> string_view {
    //     using enum Assert_type;
    //     switch (value) {
    //         case ASSERTION: return "Contract check";
    //         case PRE_CONDITION: return "Pre condition check";
    //         case POST_CONDITION: return "Post condition check";
    //         default: break;
    //     };

    //    std::unreachable();
    // }

    /////////////////////////////////////
    /////////////////////////////////////
    auto assert_base(bool cond, Assert_type type, string_view message, const std::source_location& location) noexcept -> void {
        if constexpr (STORMKIT_ASSERT == 1) {
            constexpr auto ASSERTION_PREFIX = ConsoleStyle {
                .fg        = ConsoleColor::BRIGHT_RED,
                .modifiers = StyleModifier::BOLD | StyleModifier::INVERSE
            } | "[Assertion]"sv;
            if (not cond) [[unlikely]] {
                auto lock = std::unique_lock { mutex };
                std::println(get_stderr(),
                             "{} {} failed in \n"
                             "    > file:     {}:{}:{}\n"
                             "      function: {}\n"
                             "      reason:   {}",
                             ASSERTION_PREFIX,
                             as_string(type),
                             ConsoleStyle { .fg = ConsoleColor::GREEN } | location.file_name(),
                             ConsoleStyle { .fg = ConsoleColor::BLUE } | location.line(),
                             ConsoleStyle { .fg = ConsoleColor::BLUE } | location.column(),
                             ConsoleStyle { .fg = ConsoleColor::YELLOW } | location.function_name(),
                             ConsoleStyle { .fg = ConsoleColor::RED, .modifiers = StyleModifier::BOLD } | message);
                std::fflush(get_stderr());
#if defined(__cpp_lib_debugging) and __cpp_lib_debugging >= 202311L
                if (std::is_debugger_present) { std::breakpoint(); }
#endif

                std::terminate();
            }
        }
    }
}} // namespace stormkit::core
