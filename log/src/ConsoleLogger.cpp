#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>

#include <storm/log/ConsoleLogger.hpp>
#include <storm/log/LogColorizer.hpp>

#include <fmt/core.h>

#include <iostream>

using namespace storm;
using namespace storm::log;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(LogClock::time_point start, Severity log_level)
    : Logger { std::move(start), log_level } {
}

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::~ConsoleLogger() = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(ConsoleLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger &ConsoleLogger::operator=(ConsoleLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(const ConsoleLogger &) = default;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger &ConsoleLogger::operator=(const ConsoleLogger &) = default;

////////////////////////////////////////
////////////////////////////////////////
void ConsoleLogger::flush() {
    std::cout.flush();
}

////////////////////////////////////////
////////////////////////////////////////
void ConsoleLogger::write(Severity severity, Module module, const char *string) {
    const auto now  = LogClock::now();
    const auto time = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time).count();

    static constexpr const auto LOG_LINE        = "[{1}s]";
    static constexpr const auto LOG_LINE_MODULE = "[{0}, {1}s, {2}]";

    auto str = std::string {};

    if (std::strcmp(module.get(), "") == 0) str = fmt::format(LOG_LINE, severity, time);
    else
        str = fmt::format(LOG_LINE_MODULE, severity, time, module.get());

    const auto to_stderr = severity == Severity::Error || severity == Severity::Fatal;
    auto &output         = (to_stderr) ? std::cerr : std::cout;

    colorifyBegin(severity, to_stderr);
    output << str;
    colorifyEnd(to_stderr);
    output << " " << string << '\n' << std::flush;
}
