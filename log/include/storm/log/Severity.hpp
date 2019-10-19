// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Flags.hpp>
#include <storm/core/Strings.hpp>
#include <string>

namespace storm::log {
    enum class Severity { Info, Warning, Error, Fatal, Debug };

    inline std::string severityToString(Severity severity) {
        switch (severity) {
            case Severity::Info: return "Information";
            case Severity::Warning: return "Warning";
            case Severity::Error: return "Error";
            case Severity::Fatal: return "Fatal";
            case Severity::Debug: return "Debug";
        }

        return "Unkown severity";
    }
} // namespace storm::log

FLAG_ENUM(storm::log::Severity)

CUSTOM_FORMAT(storm::log::Severity, "{}", storm::log::severityToString(data));
