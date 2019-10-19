#include <fmt/core.h>
#include <iostream>
#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>
#include <storm/log/ConsoleLogger.hpp>

using namespace storm::log;

////////////////////////////////////////
////////////////////////////////////////
ConsoleLogger::ConsoleLogger(LogClock::time_point start, Severity log_level)
	: Logger{std::move(start), log_level} {
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
void ConsoleLogger::write(Severity severity, Module module,
						  const char *string) {
	const auto now = LogClock::now();
	const auto time =
		std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time)
			.count();

	static constexpr const auto LOG_LINE		= "[{0}, {1}s] {2}\n";
	static constexpr const auto LOG_LINE_MODULE = "[{0}, {1}s, {2}] {3}\n";

	auto str = std::string{};

	if (std::strcmp(module.get(), "") == 0)
		str = fmt::format(LOG_LINE, severity, time, string);
	else
		str =
			fmt::format(LOG_LINE_MODULE, severity, time, module.get(), string);

	if (severity == Severity::Error || severity == Severity::Fatal)
		std::cerr << str << std::flush;
	else
		std::cout << str << std::flush;
}
