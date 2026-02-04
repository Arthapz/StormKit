// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:typesafe;

export import :typesafe.strong_type;
export import :typesafe.boolean;
export import :typesafe.byte;
export import :typesafe.character;
export import :typesafe.ref;
export import :typesafe.integer;
export import :typesafe.floating_point;
export import :typesafe.flags;
export import :typesafe.safecasts;
export import :typesafe.checked_value;

export namespace stormkit { inline namespace core {
    template<typename T>
    [[nodiscard]]
    constexpr auto clone(const T& value) noexcept -> T;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

export namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto clone(const T& value) noexcept -> T {
        return auto(value);
    }
}} // namespace stormkit::core
