// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

#include <storm/log/Logger.hpp>

namespace storm::log {
    class STORM_PUBLIC ConsoleLogger final: public Logger {
      public:
        explicit ConsoleLogger(LogClock::time_point start,
                               Severity log_level = Logger::DEFAULT_SEVERITY);

        ~ConsoleLogger() override;

        ConsoleLogger(ConsoleLogger &&);
        ConsoleLogger &operator=(ConsoleLogger &&);

        ConsoleLogger(const ConsoleLogger &);
        ConsoleLogger &operator=(const ConsoleLogger &);

        void write(Severity severity, Module module, const char *string) override;
        void flush() override;
    };
} // namespace storm::log

#ifdef STORM_COMPILER_MSVC
    #pragma warning(pop)
#endif
