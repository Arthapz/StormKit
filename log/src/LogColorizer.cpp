// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/log/LogColorizer.hpp>

#include <storm/core/Platform.hpp>

using namespace storm;
using namespace storm::log;

#if defined(STORM_OS_WINDOWS)
    #include <Windows.h>

[[maybe_unused]] static constexpr const core::UInt8 KBLA = 0;
[[maybe_unused]] static constexpr const core::UInt8 KRED = 12;
[[maybe_unused]] static constexpr const core::UInt8 KGRN = 2;
[[maybe_unused]] static constexpr const core::UInt8 KYEL = 14;
[[maybe_unused]] static constexpr const core::UInt8 KBLU = 9;
[[maybe_unused]] static constexpr const core::UInt8 KMAG = 13;
[[maybe_unused]] static constexpr const core::UInt8 KCYN = 11;
[[maybe_unused]] static constexpr const core::UInt8 KWHT = 15;
[[maybe_unused]] static constexpr const core::UInt8 KGRS = 8;

namespace storm::log {
    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBegin(Severity severity, bool to_stderr) noexcept {
        HANDLE handle = GetStdHandle((to_stderr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

        core::UInt8 background = KBLA;
        core::UInt8 text       = KWHT;
        switch (severity) {
            case Severity::Info:
                background = KGRN;
                text       = KBLA;
                break;
            case Severity::Warning:
                background = KMAG;
                text       = KBLA;
                break;
            case Severity::Error:
                background = KYEL;
                text       = KBLA;
                break;
            case Severity::Fatal:
                background = KRED;
                text       = KBLA;
                break;
            case Severity::Debug:
                background = KCYN;
                text       = KBLA;
                break;
        }

        SetConsoleTextAttribute(handle, (background << 4) + text);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyEnd(bool to_stderr) noexcept {
        HANDLE handle = GetStdHandle((to_stderr) ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, (KBLA << 4) + KWHT);
    }
} // namespace storm::log
#elif defined(STORM_OS_IOS)
namespace storm::log {
    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBegin([[maybe_unused]] Severity severity, [[maybe_unused]] bool to_stderr) {}

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBEnd([[maybe_unused]] bool to_stderr) {}
} // namespace storm::log
#elif defined(STORM_POSIX)
namespace storm::log {
    [[maybe_unused]] static constexpr const char *const KNRM = "\x1B[0m";
    [[maybe_unused]] static constexpr const char *const KRED = "\x1B[31m";
    [[maybe_unused]] static constexpr const char *const KGRN = "\x1B[32m";
    [[maybe_unused]] static constexpr const char *const KYEL = "\x1B[33m";
    [[maybe_unused]] static constexpr const char *const KBLU = "\x1B[34m";
    [[maybe_unused]] static constexpr const char *const KMAG = "\x1B[35m";
    [[maybe_unused]] static constexpr const char *const KCYN = "\x1B[36m";
    [[maybe_unused]] static constexpr const char *const KWHT = "\x1B[37m";
    [[maybe_unused]] static constexpr const char *const KGRS = "\033[1m";
    [[maybe_unused]] static constexpr const char *const KINV = "\e[7m";

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyBegin(Severity severity, bool to_stderr) noexcept {
        auto &output = (to_stderr) ? std::cerr : std::cout;
        switch (severity) {
            case Severity::Info: output << KGRS << KINV << KGRN; break;
            case Severity::Warning: output << KGRS << KINV << KMAG; break;
            case Severity::Error: output << KGRS << KINV << KYEL; break;
            case Severity::Fatal: output << KGRS << KINV << KRED; break;
            case Severity::Debug: output << KGRS << KINV << KCYN; break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void colorifyEnd(bool to_stderr) noexcept {
        auto &output = (to_stderr) ? std::cerr : std::cout;
        output << KNRM;
    }
} // namespace storm::log
#endif