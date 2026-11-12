module;

#include <stormkit/core/platform_macro.hpp>

#include <status-code/error.hpp>

#include <status-code/status_code.hpp>

#include <status-code/std_error_code.hpp>

export module stormkit.core.status_code;

import std;

export namespace system_error2 {
    using ::system_error2::errc;
    using ::system_error2::quick_status_code_from_enum;
    using ::system_error2::quick_status_code_from_enum_code;
    using ::system_error2::quick_status_code_from_enum_defaults;

    using ::system_error2::posix_code;
    using ::system_error2::status_code;
    using ::system_error2::std_error_code;
    using ::system_error2::system_code;

#if STORMKIT_OS_IS_WINDOWS
    using ::system_error2::nt_code;
    using ::system_error2::win32_code;

// using nt_code = ::system_error2::status_code<::system_error2::nt_code_domain>;
#endif

} // namespace system_error2
