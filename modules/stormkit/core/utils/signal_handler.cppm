module;

#include <stormkit/core/api.hpp>

export module stormkit.core:utils.signal_handler;

export namespace stormkit { inline namespace core {
    STORMKIT_CORE_API
    auto setup_signal_handler() noexcept -> void;
}} // namespace stormkit::core
