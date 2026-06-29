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
using SYSTEM_ERROR2_NAMESPACE::win32_code;

constexpr auto format_as(const system_code& error, auto& ctx) noexcept -> decltype(ctx.out());
// clang-format off
SYSTEM_ERROR2_NAMESPACE_END
// clang-format on

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

SYSTEM_ERROR2_NAMESPACE_BEGIN

STORMKIT_FORCE_INLINE
constexpr auto format_as(const system_code& error, auto& ctx) noexcept -> decltype(ctx.out()) {
    return std::format_to(ctx.out(), "{}", error.message());
}

SYSTEM_ERROR2_NAMESPACE_END
