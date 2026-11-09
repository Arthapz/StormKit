// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <cstdio>

#include <stormkit/core/platform_macro.hpp>

#ifndef STORMKIT_OS_WINDOWS
    #include <unistd.h>
#endif

export module stormkit.core.console.io;

import std;

export namespace stormkit { inline namespace core {
    [[nodiscard]]
    auto get_stderr() noexcept -> std::FILE*;
    [[nodiscard]]
    auto get_stdout() noexcept -> std::FILE*;
    [[nodiscard]]
    auto get_stdin() noexcept -> std::FILE*;

    [[nodiscard]]
    auto get_stderr_fd() noexcept -> int;
    [[nodiscard]]
    auto get_stdout_fd() noexcept -> int;
    [[nodiscard]]
    auto get_stdin_fd() noexcept -> int;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_INTRINSIC
    inline auto get_stderr() noexcept -> std::FILE* {
        return std::bit_cast<std::FILE*>(stderr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_INTRINSIC
    inline auto get_stdout() noexcept -> std::FILE* {
        return std::bit_cast<std::FILE*>(stdout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_INTRINSIC
    inline auto get_stdin() noexcept -> std::FILE* {
        return std::bit_cast<std::FILE*>(stdin);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto get_stderr_fd() noexcept -> int {
#ifdef STORMKIT_OS_WINDOWS
        return ::_fileno(get_stderr());
#else
        return STDERR_FILENO;
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto get_stdout_fd() noexcept -> int {
#ifdef STORMKIT_OS_WINDOWS
        return ::_fileno(get_stdout());
#else
        return STDOUT_FILENO;
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto get_stdin_fd() noexcept -> int {
#ifdef STORMKIT_OS_WINDOWS
        return ::_fileno(get_stdin());
#else
        return STDIN_FILENO;
#endif
    }
}} // namespace stormkit::core
