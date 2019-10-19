// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#include <filesystem>
#include <fstream>
#include <storm/core/NonCopyable.hpp>
#include <storm/log/Logger.hpp>
#include <unordered_map>

#ifdef STORM_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

namespace storm::log {
    class STORM_PUBLIC FileLogger final: public storm::core::NonCopyable, public Logger {
      public:
        explicit FileLogger(LogClock::time_point start,
                            std::filesystem::path path,
                            Severity log_level = Logger::DEFAULT_SEVERITY);
        ~FileLogger() override;

        FileLogger(FileLogger &&);
        FileLogger &operator=(FileLogger &&);

        void write(Severity severity, Module module, const char *string) override;
        void flush() override;

      private:
        std::unordered_map<std::string, std::ofstream> m_streams;

        std::filesystem::path m_base_path;
    };
} // namespace storm::log

#ifdef STORM_COMPILER_MSVC
    #pragma warning(pop)
#endif
