module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.stacktrace;

import std;

export namespace stormkit { inline namespace core {
    STORMKIT_CORE_API
    auto print_stacktrace(int ignore_count = 0) noexcept -> void;
}} // namespace stormkit::core
