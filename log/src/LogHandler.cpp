// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/log/ConsoleLogger.hpp>
#include <storm/log/LogHandler.hpp>

using namespace storm::log;

namespace storm::log {
    LoggerOwnedPtr LogHandler::m_logger = nullptr;
}
void LogHandler::setupLogger(LoggerOwnedPtr &&ptr) {
    m_logger = std::move(ptr);
}

void LogHandler::setupDefaultLogger() {
    setupLogger<ConsoleLogger>();
}

Logger &LogHandler::logger() {
    return *m_logger;
}
