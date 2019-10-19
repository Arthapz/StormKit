#include <iostream>

#include <storm/core/Platform.hpp>
#include <storm/core/Ranges.hpp>
#include <storm/core/Strings.hpp>

#include <storm/log/FileLogger.hpp>

using namespace storm;
using namespace storm::log;

////////////////////////////////////////
////////////////////////////////////////
FileLogger::FileLogger(LogClock::time_point start, std::filesystem::path path, Severity log_level)
    : Logger { std::move(start), log_level }, m_base_path { std::move(path) } {
    if (!std::filesystem::exists(m_base_path)) std::filesystem::create_directory(m_base_path);

    ASSERT(std::filesystem::is_directory(m_base_path), "path need to be a directory");

    auto filepath                = m_base_path / "log.txt";
    m_streams[filepath.string()] = std::ofstream { filepath.string() };
}

////////////////////////////////////////
////////////////////////////////////////
FileLogger::~FileLogger() = default;

////////////////////////////////////////
////////////////////////////////////////
FileLogger::FileLogger(FileLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FileLogger &FileLogger::operator=(FileLogger &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void FileLogger::flush() {
    for (auto &[path, stream] : m_streams) stream.flush();
}

////////////////////////////////////////
////////////////////////////////////////
void FileLogger::write(Severity severity, Module module, const char *string) {
    const auto now  = LogClock::now();
    const auto time = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time).count();

    auto filepath = m_base_path / "log.txt";
    if (std::strcmp(module.get(), "") != 0) {
        filepath = m_base_path / (module.get() + std::string { "-log.txt" });

        if (m_streams.find(filepath.string()) == core::ranges::end(m_streams))
            m_streams[filepath.string()] = std::ofstream { filepath.string() };
    }

    static constexpr const auto LOG_LINE        = "[{0}, {1}s] {2}\n";
    static constexpr const auto LOG_LINE_MODULE = "[{0}, {1}s, {2}] {3}\n";

    auto final_string = std::string {};
    if (std::strcmp(module.get(), "") == 0)
        final_string = fmt::format(LOG_LINE, severity, time, string);
    else
        final_string = fmt::format(LOG_LINE_MODULE, severity, time, module.get(), string);

    m_streams[filepath.string()] << final_string << std::flush;
}
