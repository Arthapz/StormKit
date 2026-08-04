// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#define SUBSTITUTION 0

export module stormkit.core.typesafe.flags;

import std;

import stormkit.core.meta;
import stormkit.core.types;

export {
    namespace stormkit { inline namespace core {
        namespace meta {
            template<IsEnumeration T>
            inline constexpr auto FLAG_TRAIT = false;

            template<class T>
            concept IsFlag = FLAG_TRAIT<T>;
        } // namespace meta

        /// \brief Check if a flag bit is enabled
        /// \requires `Enum` to be an enumeration promoted static_cast a flag with `FLAG_ENUM`
        /// macro
        /// \returns true if the flag big is set and false if not
        template<meta::IsFlag T>
        [[nodiscard]]
        constexpr auto check_flag_bit(const T& value, const T& flag) noexcept -> bool;

        /// \exclude
        template<meta::IsFlag T>
        [[nodiscard]]
        constexpr auto next_value(const T& value) noexcept -> T;
    }} // namespace stormkit::core

    template<stormkit::meta::IsFlag T>
    [[nodiscard]]
    constexpr auto operator|(const T& lhs, const T& rhs) noexcept -> decltype(auto);

    template<stormkit::meta::IsFlag T>
    [[nodiscard]]
    constexpr auto operator&(const T& lhs, const T& rhs) noexcept -> decltype(auto);

    template<stormkit::meta::IsFlag T>
    [[nodiscard]]
    constexpr auto operator^(const T& lhs, const T& rhs) noexcept -> decltype(auto);

    template<stormkit::meta::IsFlag T>
    [[nodiscard]]
    constexpr auto operator~(const T& lhs) noexcept -> decltype(auto);

    template<stormkit::meta::IsFlag T>
    constexpr auto operator|=(T& lhs, const T& rhs) noexcept -> decltype(auto);

    template<stormkit::meta::IsFlag T>
    constexpr auto operator&=(T& lhs, const T& rhs) noexcept -> decltype(auto);

    template<stormkit::meta::IsFlag T>
    constexpr auto operator^=(T& lhs, const T& rhs) noexcept -> decltype(auto);
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsFlag T>
    STORMKIT_FORCE_INLINE
    constexpr auto check_flag_bit(const T& value, const T& flag) noexcept -> bool {
        return (value & flag) == flag;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsFlag T>
    STORMKIT_FORCE_INLINE
    constexpr auto next_value(const T& value) noexcept -> T {
        using Underlying = meta::UnderlyingType<T>;
        return static_cast<T>(static_cast<Underlying>(value) << 1);
    }
}} // namespace stormkit::core

using namespace stormkit;

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator|(const T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(static_cast<Underlying>(lhs) | static_cast<Underlying>(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator&(const T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(static_cast<Underlying>(lhs) & static_cast<Underlying>(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator^(const T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(static_cast<Underlying>(lhs) ^ static_cast<Underlying>(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator~(const T& lhs) noexcept -> decltype(auto) {
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(~static_cast<Underlying>(lhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator|=(T& lhs, const T& rhs) noexcept -> decltype(auto) {
    lhs = lhs | rhs;
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator&=(T& lhs, const T& rhs) noexcept -> decltype(auto) {
    lhs = lhs & rhs;
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator^=(T& lhs, const T& rhs) noexcept -> decltype(auto) {
    lhs = lhs ^ rhs;
    return lhs;
}
