module;

#include <stormkit/core/platform_macro.hpp>

#include <status-code/error.hpp>
#include <status-code/status_code.hpp>

#include <format>

export module stormkit.core:status_code;

// clang-format off
export SYSTEM_ERROR2_NAMESPACE_BEGIN
  // clang-format on

  using SYSTEM_ERROR2_NAMESPACE::system_code;
using SYSTEM_ERROR2_NAMESPACE::posix_code;

#ifdef STORMKIT_OS_WINDOWS
using SYSTEM_ERROR2_NAMESPACE::nt_code;
using SYSTEM_ERROR2_NAMESPACE::win32_code;

// using nt_code = SYSTEM_ERROR2_NAMESPACE::status_code<SYSTEM_ERROR2_NAMESPACE::nt_code_domain>;
#endif

constexpr auto format_as(const system_code& error, auto& ctx) noexcept -> decltype(ctx.out());
constexpr auto format_as(const posix_code& error, auto& ctx) noexcept -> decltype(ctx.out());
#ifdef STORMKIT_OS_WINDOWS
constexpr auto format_as(const nt_code& error, auto& ctx) noexcept -> decltype(ctx.out());
constexpr auto format_as(const win32_code& error, auto& ctx) noexcept -> decltype(ctx.out());
#endif
// clang-format off
SYSTEM_ERROR2_NAMESPACE_END
// clang-format on

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

SYSTEM_ERROR2_NAMESPACE_BEGIN

STORMKIT_FORCE_INLINE
constexpr auto format_as(const system_code& error, auto& ctx) noexcept -> decltype(ctx.out()) {
    return std::format_to(ctx.out(),
                          "{:#x} ({})",
                          static_cast<unsigned long long>(error.value()),
                          std::string_view { error.message() });
}

STORMKIT_FORCE_INLINE
constexpr auto format_as(const posix_code& error, auto& ctx) noexcept -> decltype(ctx.out()) {
    return std::format_to(ctx.out(),
                          "{:#x} ({})",
                          static_cast<unsigned long long>(error.value()),
                          std::string_view { error.message() });
}

#ifdef STORMKIT_OS_WINDOWS
STORMKIT_FORCE_INLINE
constexpr auto format_as(const nt_code& error, auto& ctx) noexcept -> decltype(ctx.out()) {
    return std::format_to(ctx.out(),
                          "{:#x} ({})",
                          static_cast<unsigned long long>(error.value()),
                          std::string_view { error.message() });
}

STORMKIT_FORCE_INLINE
constexpr auto format_as(const win32_code& error, auto& ctx) noexcept -> decltype(ctx.out()) {
    return std::format_to(ctx.out(),
                          "{:#x} ({})",
                          static_cast<unsigned long long>(error.value()),
                          std::string_view { error.message() });
}
#endif

SYSTEM_ERROR2_NAMESPACE_END
