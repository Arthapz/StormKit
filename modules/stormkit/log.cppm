// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/flags_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

#include <stormkit/log/api.hpp>

export module stormkit.log;

import std;
import frozen;

import stormkit.core;

namespace stdr = std::ranges;

export {
    namespace stormkit::log {
        struct Module;
        enum class Severity : u8 {
            INFO    = 1,
            WARNING = 2,
            ERROR   = 4,
            FATAL   = 8,
            DEBUG   = 16,
        };

        [[nodiscard]]
        constexpr auto as_string(Severity severity) noexcept -> string_view;
        [[nodiscard]]
        constexpr auto to_string(Severity severity) noexcept -> string;

        STORMKIT_LOG_API
        auto parse_args(array_view<const string_view> args) noexcept -> void;

        class STORMKIT_LOG_API Logger {
          public:
            using LogClock = std::chrono::high_resolution_clock;

            explicit Logger(LogClock::time_point start) noexcept;
            Logger(LogClock::time_point start, Severity log_level) noexcept;
            virtual ~Logger() noexcept;

            virtual auto write(Severity severity, const Module& module, std::string_view string) noexcept -> void = 0;
            virtual auto flush() noexcept -> void                                                                 = 0;

            auto set_log_level(Severity log_level) noexcept -> void;

            [[nodiscard]]
            auto start_time() const noexcept -> const LogClock::time_point&;
            [[nodiscard]]
            auto log_level() const noexcept -> const Severity&;

            [[nodiscard]]
            auto mutex() noexcept -> std::mutex&;

            template<class T, typename... Args>
            [[nodiscard]]
            static auto create_logger_instance(Args&&... param_args) noexcept -> T;

            template<class T, typename... Args>
            [[nodiscard]]
            static auto allocate_logger_instance(Args&&... param_args) noexcept -> Heap<T>;

            template<class... Args>
            static auto log(Severity                    severity,
                            const Module&               module,
                            std::format_string<Args...> format_string,
                            Args&&... args) noexcept -> void;

            template<class... Args>
            static auto log(Severity severity, std::format_string<Args...> format_string, Args&&... args) noexcept -> void;

            template<class... Args>
            static auto log_runtime(Severity severity, const Module& module, string_view format_string, Args&&... args) noexcept
              -> void;

            template<class... Args>
            static auto log_runtime(Severity severity, string_view format_string, Args&&... args) noexcept -> void;

            template<class... Args>
            static auto dlog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto ilog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto wlog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto elog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto flog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void;

            template<class... Args>
            static auto dlog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto ilog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto wlog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto elog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto flog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void;

            template<class... Args>
            static auto dlog_runtime(std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto ilog_runtime(std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto wlog_runtime(std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto elog_runtime(std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto flog_runtime(std::string_view format_string, Args&&... args) noexcept -> void;

            template<class... Args>
            static auto dlog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto ilog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto wlog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto elog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void;
            template<class... Args>
            static auto flog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void;

            [[nodiscard]]
            static auto has_logger() noexcept -> bool;
            [[nodiscard]]
            static auto instance() noexcept -> Logger&;

          protected:
            LogClock::time_point m_start_time;
            Severity             m_log_level;

            std::mutex m_mutex;
        };

        struct Module {
            template<class... Args>
            auto dlog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto ilog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto wlog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto elog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto flog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void;

            template<class... Args>
            auto dlog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto ilog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto wlog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto elog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void;
            template<class... Args>
            auto flog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void;

            auto flush() const noexcept -> void;

            string_view name = "";
        };

        template<meta::ConstexprString str>
        [[nodiscard]]
        constexpr auto operator""_module() noexcept -> stormkit::log::Module;

        class STORMKIT_LOG_API FileLogger final: public Logger {
          public:
            FileLogger(LogClock::time_point start, std::filesystem::path path) noexcept;
            FileLogger(LogClock::time_point start, std::filesystem::path path, Severity log_level) noexcept;
            ~FileLogger() noexcept override;

            FileLogger(const FileLogger&) noexcept                    = delete;
            auto operator=(const FileLogger&) noexcept -> FileLogger& = delete;

            FileLogger(FileLogger&&) noexcept                    = delete;
            auto operator=(FileLogger&&) noexcept -> FileLogger& = delete;

            auto write(Severity severity, const Module& module, std::string_view string) noexcept -> void override;
            auto flush() noexcept -> void override;

          private:
            string_hash_map<std::ofstream> m_streams;

            std::filesystem::path m_base_path;
        };

        class STORMKIT_LOG_API ConsoleLogger final: public Logger {
          public:
            explicit ConsoleLogger(LogClock::time_point start) noexcept;
            ConsoleLogger(LogClock::time_point start, Severity log_level) noexcept;

            ConsoleLogger(const ConsoleLogger&) noexcept                    = delete;
            auto operator=(const ConsoleLogger&) noexcept -> ConsoleLogger& = delete;

            ConsoleLogger(ConsoleLogger&&) noexcept                    = delete;
            auto operator=(ConsoleLogger&&) noexcept -> ConsoleLogger& = delete;

            ~ConsoleLogger() noexcept override;

            auto write(Severity severity, const Module& module, std::string_view string) noexcept -> void override;
            auto flush() noexcept -> void override;
        };
    } // namespace stormkit::log
    FLAG_ENUM(stormkit::log::Severity)
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

using namespace std::literals;

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto as_string(Severity severity) noexcept -> string_view {
        switch (severity) {
            case Severity::INFO: return "INFO";
            case Severity::WARNING: return "WARNING";
            case Severity::ERROR: return "ERROR";
            case Severity::FATAL: return "FATAL";
            case Severity::DEBUG: return "DEBUG";
            default: break;
        }

        std::unreachable();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_string(Severity severity) noexcept -> string {
        return string { as_string(severity) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Logger::set_log_level(Severity log_level) noexcept -> void {
        m_log_level = log_level;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Logger::start_time() const noexcept -> const LogClock::time_point& {
        return m_start_time;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Logger::log_level() const noexcept -> const Severity& {
        return m_log_level;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Logger::mutex() noexcept -> std::mutex& {
        return m_mutex;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    inline auto Logger::create_logger_instance(Args&&... param_args) noexcept -> T {
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        return T { std::move(time_point), std::forward<Args>(param_args)... };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    inline auto Logger::allocate_logger_instance(Args&&... param_args) noexcept -> Heap<T> {
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        return allocate(std::move(time_point), std::forward<Args>(param_args)...)
          .transform_error(core::monadic::assert("Failed to allocate logger instance"));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    inline auto Logger::log(Severity                    severity,
                            const Module&               m,
                            std::format_string<Args...> format_string,
                            Args&&... param_args) noexcept -> void {
        EXPECTS(has_logger());

        const auto log_level = instance().log_level();
        if (not check_flag_bit(log_level, severity)) return;

        auto size = std::formatted_size(format_string, std::forward<Args>(param_args)...);

        if (size <= 64) {
            thread_local auto memory_buffer = array<char, 64> {};
            const auto        end_it        = std::format_to(stdr::begin(memory_buffer),
                                                             std::move(format_string),
                                                             std::forward<Args>(param_args)...);

            const auto _ = std::unique_lock(instance().mutex());
            instance().write(severity, m, string_view { stdr::begin(memory_buffer), end_it });
        } else {
            auto memory_buffer = dyn_array<char> {};
            memory_buffer.resize(size);
            const auto end_it = std::format_to(stdr::begin(memory_buffer),
                                               std::move(format_string),
                                               std::forward<Args>(param_args)...);

            const auto _ = std::unique_lock(instance().mutex());
            instance().write(severity, m, string_view { stdr::begin(memory_buffer), end_it });
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::log(Severity severity, std::format_string<Args...> format_string, Args&&... param_args) noexcept -> void {
        log(severity, Module {}, std::move(format_string), std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    inline auto Logger::log_runtime(Severity         severity,
                                    const Module&    m,
                                    std::string_view format_string,
                                    Args&&... param_args) noexcept -> void {
        EXPECTS(has_logger());

        struct counter {
            usize n { 0 };
            using difference_type = long;

            auto operator*() const noexcept { return std::ignore; }

            auto operator++() noexcept -> counter& {
                ++n;
                return *this;
            }

            auto operator++(int) noexcept -> counter { return counter { n++ }; }
        };

        const auto log_level = instance().log_level();
        if (not check_flag_bit(log_level, severity)) return;

        auto args = std::format_args { std::forward<Args>(param_args)... };

        auto c = counter {};
        c      = std::vformat_to(c, format_string, args);

        if (c.n <= 64) {
            thread_local auto memory_buffer = array<char, 64> {};
            const auto        end_it        = std::format_to(stdr::begin(memory_buffer),
                                                             std::move(format_string),
                                                             std::forward<Args>(param_args)...);

            const auto _ = std::unique_lock(instance().mutex());
            instance().write(severity, m, string_view { stdr::begin(memory_buffer), end_it });
        } else {
            auto memory_buffer = dyn_array<char> {};
            memory_buffer.resize(c.n);
            const auto end_it = std::vformat_to(stdr::begin(memory_buffer), std::move(format_string), args);

            const auto _ = std::unique_lock(instance().mutex());
            instance().write(severity, m, string_view { stdr::begin(memory_buffer), end_it });
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::log_runtime(Severity severity, string_view format_string, Args&&... param_args) noexcept -> void {
        log(severity, Module {}, std::move(format_string), std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline ConsoleLogger::~ConsoleLogger() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline FileLogger::~FileLogger() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::dlog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::DEBUG, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::ilog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::INFO, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::wlog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::WARNING, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::elog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::ERROR, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::flog(std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::FATAL, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::dlog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::DEBUG, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::ilog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::INFO, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::wlog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::WARNING, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::elog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::ERROR, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::flog(const Module& module, std::format_string<Args...> format_string, Args&&... args) noexcept -> void {
        log(Severity::FATAL, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::dlog_runtime(std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::DEBUG, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::ilog_runtime(std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::INFO, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::wlog_runtime(std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::WARNING, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::elog_runtime(std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::ERROR, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::flog_runtime(std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::FATAL, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::dlog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::DEBUG, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::ilog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::INFO, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::wlog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::WARNING, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::elog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::ERROR, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::flog_runtime(const Module& module, std::string_view format_string, Args&&... args) noexcept -> void {
        log_runtime(Severity::FATAL, module, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::dlog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void {
        Logger::dlog(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::ilog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void {
        Logger::ilog(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::wlog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void {
        Logger::wlog(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::elog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void {
        Logger::elog(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::flog(std::format_string<Args...> format_string, Args&&... args) const noexcept -> void {
        Logger::flog(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::dlog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void {
        Logger::dlog_runtime(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::ilog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void {
        Logger::ilog_runtime(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::wlog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void {
        Logger::wlog_runtime(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::elog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void {
        Logger::elog_runtime(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::flog_runtime(std::string_view format_string, Args&&... args) const noexcept -> void {
        Logger::flog_runtime(*this, std::move(format_string), std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Module::flush() const noexcept -> void {
        Logger::instance().flush();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::ConstexprString str>
    STORMKIT_FORCE_INLINE
    constexpr auto operator""_module() noexcept -> stormkit::log::Module {
        return Module { str.view() };
    }
} // namespace stormkit::log
