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
        constexpr auto as_string(Severity severity) noexcept -> std::string_view;
        [[nodiscard]]
        constexpr auto to_string(Severity severity) noexcept -> std::string;

        STORMKIT_LOG_API
        auto parse_args(std::span<const std::string_view> args) noexcept -> void;

        class STORMKIT_LOG_API Logger {
          public:
            using LogClock = std::chrono::high_resolution_clock;

            explicit Logger(LogClock::time_point start) noexcept;
            Logger(LogClock::time_point start, Severity log_level) noexcept;
            virtual ~Logger() noexcept;

            virtual auto write(Severity severity, const Module& module, CZString string) noexcept -> void = 0;
            virtual auto flush() noexcept -> void                                                         = 0;

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
            static auto log(Severity         severity,
                            const Module&    module,
                            std::string_view format_string,
                            Args&&... param_args) noexcept -> void;

            template<class... Args>
            static auto log(Severity severity, std::string_view format_string, Args&&... param_args) noexcept -> void;

            template<class... Args>
            static auto dlog(Args&&... param_args) noexcept -> void;

            template<class... Args>
            static auto ilog(Args&&... param_args) noexcept -> void;

            template<class... Args>
            static auto wlog(Args&&... param_args) noexcept -> void;

            template<class... Args>
            static auto elog(Args&&... param_args) noexcept -> void;

            template<class... Args>
            static auto flog(Args&&... param_args) noexcept -> void;

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
            auto dlog(Args&&... args) const noexcept -> void;

            template<class... Args>
            auto ilog(Args&&... args) const noexcept -> void;

            template<class... Args>
            auto wlog(Args&&... args) const noexcept -> void;

            template<class... Args>
            auto elog(Args&&... args) const noexcept -> void;

            template<class... Args>
            auto flog(Args&&... args) const noexcept -> void;

            auto flush() const noexcept -> void;

            std::string_view name = "";
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

            auto write(Severity severity, const Module& module, CZString string) noexcept -> void override;
            auto flush() noexcept -> void override;

          private:
            StringHashMap<std::ofstream> m_streams;

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

            auto write(Severity severity, const Module& module, CZString string) noexcept -> void override;
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
    constexpr auto as_string(Severity severity) noexcept -> std::string_view {
        switch (severity) {
            case Severity::INFO: return "Severity::INFO";
            case Severity::WARNING: return "Severity::WARNING";
            case Severity::ERROR: return "Severity::ERROR";
            case Severity::FATAL: return "Severity::FATAL";
            case Severity::DEBUG: return "Severity::DEBUG";
            default: break;
        }

        std::unreachable();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_string(Severity severity) noexcept -> std::string {
        return std::string { as_string(severity) };
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
    inline auto Logger::log(Severity severity, const Module& m, std::string_view format_string, Args&&... param_args) noexcept
      -> void {
        EXPECTS(has_logger());

        const auto log_level = instance().log_level();
        if (not check_flag_bit(log_level, severity)) return;

        auto memory_buffer = std::string {};
        memory_buffer.reserve(std::size(format_string));
        std::vformat_to(std::back_inserter(memory_buffer), format_string, std::make_format_args(param_args...));

        auto _ = std::unique_lock(instance().mutex());
        instance().write(severity, m, std::data(memory_buffer));
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
    inline auto Logger::log(Severity severity, std::string_view format_string, Args&&... param_args) noexcept -> void {
        log(severity, Module {}, format_string, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::dlog(Args&&... param_args) noexcept -> void {
        log(Severity::DEBUG, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::ilog(Args&&... param_args) noexcept -> void {
        log(Severity::INFO, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::wlog(Args&&... param_args) noexcept -> void {
        log(Severity::WARNING, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::elog(Args&&... param_args) noexcept -> void {
        log(Severity::ERROR, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Logger::flog(Args&&... param_args) noexcept -> void {
        log(Severity::FATAL, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::dlog(Args&&... args) const noexcept -> void {
        Logger::dlog(*this, std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::ilog(Args&&... args) const noexcept -> void {
        Logger::ilog(*this, std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::wlog(Args&&... args) const noexcept -> void {
        Logger::wlog(*this, std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::elog(Args&&... args) const noexcept -> void {
        Logger::elog(*this, std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class... Args>
    STORMKIT_FORCE_INLINE
    inline auto Module::flog(Args&&... args) const noexcept -> void {
        Logger::flog(*this, std::forward<Args>(args)...);
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
