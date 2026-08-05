// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.string.constexpr_string;

import std;

import stormkit.core.types;
import stormkit.core.typesafe.safecasts;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core { namespace meta {
    template<usize N>
    struct ConstexprString {
        consteval ConstexprString() noexcept = default;
        consteval ConstexprString(const char (&new_str)[N]) noexcept;

        [[nodiscard]]
        constexpr auto begin(this auto& self) noexcept -> decltype(auto);
        [[nodiscard]]
        constexpr auto end(this auto& self) noexcept -> decltype(auto);
        [[nodiscard]]
        constexpr auto size() const noexcept -> usize;

        [[nodiscard]]
        constexpr auto view() const noexcept -> string_view;

        [[nodiscard]]
        constexpr operator string_view() const noexcept;

        constexpr auto update_size() noexcept -> void;

        static constexpr auto STATIC_SIZE = as<usize>(N - 1u);

        std::array<char, N> data   = {};
        usize               m_size = 0;
    };
}}} // namespace stormkit::core::meta

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core { namespace meta {
    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
    STORMKIT_FORCE_INLINE
    consteval ConstexprString<N>::ConstexprString(const char (&new_str)[N]) noexcept {
        std::copy_n(new_str, STATIC_SIZE, std::data(data));
        update_size();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
        STORMKIT_FORCE_INLINE
    constexpr auto ConstexprString<N>::begin(this auto& self) noexcept -> decltype(auto) {
        return stdr::begin(self.data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
        STORMKIT_FORCE_INLINE
    constexpr auto ConstexprString<N>::end(this auto& self) noexcept -> decltype(auto) {
        return stdr::begin(self.data) + unchecked_narrow<std::ptrdiff_t>(self.size());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
        STORMKIT_FORCE_INLINE
    constexpr auto ConstexprString<N>::size() const noexcept -> usize {
        return m_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
        STORMKIT_FORCE_INLINE
    constexpr auto ConstexprString<N>::view() const noexcept -> string_view {
        return string_view { begin(), end() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
        STORMKIT_FORCE_INLINE
    constexpr ConstexprString<N>::operator string_view() const noexcept {
        return view();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<usize N>
        STORMKIT_FORCE_INLINE
    constexpr auto ConstexprString<N>::update_size() noexcept -> void {
        m_size = std::char_traits<char>::length(stdr::data(data));
    }
}}} // namespace stormkit::core::meta
