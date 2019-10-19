// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonDefaultInstanciable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/log/Fwd.hpp>
#include <storm/log/Logger.hpp>

namespace storm::log {
    class STORM_PUBLIC LogHandler: public core::NonDefaultInstanciable {
      public:
        template<class T, typename... Args>
        static void setupLogger(Args &&... param_args);

        static void setupDefaultLogger();

        template<typename... Args>
        static void
            log(Severity severity, Module module, std::string format_string, Args &&... param_args);

        template<typename... Args>
        static inline void log(Severity severity, std::string format_string, Args &&... param_args);

        template<typename... Args>
        static inline void dlog(Args &&... param_args);

        template<typename... Args>
        static inline void ilog(Args &&... param_args);

        template<typename... Args>
        static inline void wlog(Args &&... param_args);

        template<typename... Args>
        static inline void elog(Args &&... param_args);

        template<typename... Args>
        static inline void flog(Args &&... param_args);

        static Logger &logger();

      private:
        static void setupLogger(LoggerOwnedPtr &&ptr);

        static LoggerOwnedPtr m_logger;
    };
} // namespace storm::log

#include "LogHandler.inl"
