// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/contract_macro.hpp>

export module stormkit.core:typesafe.byte;

import std;

import :meta;

import :typesafe.integer;

import :containers.aliases;

import :utils.contract;
import :utils.tags;

namespace stdr = std::ranges;
namespace stdp = std::pmr;

template<typename T, stormkit::usize EXTENT>
consteval auto get_byte_extent_value_of() {
    if constexpr (EXTENT == std::dynamic_extent) return EXTENT;
    else if constexpr (stormkit::meta::Is<stormkit::meta::RemoveConst<T>, void>)
        return EXTENT;
    else
        return EXTENT * sizeof(T);
}

export namespace stormkit { inline namespace core {
    using std::byte;

    template<usize N>
    using byte_array    = array<byte, N>;
    using byte_dynarray = dynarray<byte>;
    using byte_view     = array_view<const byte>;
    using byte_view_mut = array_view<byte>;

    namespace pmr {
        using byte_dynarray = dynarray<byte>;
    } // namespace pmr

    template<typename T>
    constexpr auto zero_bytes(T& value) noexcept -> void;

    template<typename T>
    constexpr auto zeroed() noexcept -> T;

    template<typename T>
    [[nodiscard]]
    constexpr auto byte_swap(const T& value) noexcept -> T;

    // array_view
    using std::as_bytes;

    template<typename T>
    [[nodiscard]]
    constexpr auto as_bytes(const T* const ptr, usize size = 1) noexcept -> byte_view;

    [[nodiscard]]
    constexpr auto as_bytes(string_view string) noexcept -> byte_view;

    template<class T>
    [[nodiscard]]
    constexpr auto as_bytes(const T& value) noexcept -> array_view<const byte, sizeof(T)>;

    template<typename T, usize EXTENT>
    [[nodiscard]]
    constexpr auto as_mutable_bytes(array_view<T, EXTENT> container) noexcept
      -> array_view<byte, get_byte_extent_value_of<T, EXTENT>()>;

    template<typename T>
    [[nodiscard]]
    constexpr auto as_mutable_bytes(T* const ptr, usize size = 1) noexcept -> byte_view_mut;

    template<class T>
    [[nodiscard]]
    constexpr auto as_mutable_bytes(T& value) noexcept -> array_view<byte, sizeof(T)>;

    template<class T, usize EXTENT = std::dynamic_extent>
    [[nodiscard]]
    constexpr auto bytes_as(array_view<const byte, EXTENT> bytes) noexcept -> const T&;

    // template<typename T, stdr::range Range>
    //     requires(meta::SameAs<meta::ToPlainType<meta::ContainedType<Range>>, byte>)
    // [[nodiscard]]
    // constexpr auto bytes_as_span(const Range& bytes) noexcept -> array_view<const T>;

    // template<typename T, usize EXTENT = std::dynamic_extent>
    // [[nodiscard]]
    // constexpr auto bytes_as_span(byte_view<EXTENT> bytes) noexcept
    //   -> array_view<const T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)>;

    // template<class T, usize EXTENT>
    // [[nodiscard]]
    // constexpr auto bytes_mut_as(byte_view_mut <EXTENT> bytes) noexcept -> T&;

    // template<typename T, stdr::range Range>
    //     requires(meta::SameAs<meta::ContainedType<Range>, byte> and not meta::IsConst<Range>)
    // [[nodiscard]]
    // constexpr auto bytes_mut_as_span(Range& range) noexcept -> array_view<const T>;

    // template<typename T, usize EXTENT>
    // [[nodiscard]]
    // constexpr auto bytes_mut_as_span(byte_view_mut <EXTENT> bytes) noexcept
    //   -> array_view<T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)>;

    template<typename T, usize N>
    [[nodiscard]]
    constexpr auto into_bytes(const T (&bytes)[N]) noexcept -> byte_array<N>;

    namespace literals {
        [[nodiscard]]
        constexpr auto operator""_b(unsigned long long value) noexcept -> byte;

        [[nodiscard]]
        constexpr auto operator""_kb(unsigned long long x) noexcept -> u64;

        [[nodiscard]]
        constexpr auto operator""_mb(unsigned long long x) noexcept -> u64;

        [[nodiscard]]
        constexpr auto operator""_gb(unsigned long long x) noexcept -> u64;

        [[nodiscard]]
        constexpr auto operator""_kib(unsigned long long x) noexcept -> u64;

        [[nodiscard]]
        constexpr auto operator""_mib(unsigned long long x) noexcept -> u64;

        [[nodiscard]]
        constexpr auto operator""_gib(unsigned long long x) noexcept -> u64;

    } // namespace literals
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto zero_bytes(T& value) noexcept -> void {
        auto bytes = as_mutable_bytes(value);
        stdr::fill(bytes, byte { 0 });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto zeroed() noexcept -> T {
        auto data = T {};
        zero_bytes(data);
        return data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto byte_swap(const T& value) noexcept -> T {
        if constexpr (meta::IsIntegral<T>) return std::byteswap(value);
        else {
            auto repr = std::bit_cast<array<byte, sizeof(value)>>(value);

            stdr::reverse(repr);

            return std::launder(std::bit_cast<T>(repr));
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(const T* const ptr, usize size) noexcept -> byte_view {
        return as_bytes(array_view<const T> { ptr, size });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(string_view value) noexcept -> byte_view {
        return as_bytes(array_view<const char> { stdr::data(value), stdr::size(value) });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(const T& value) noexcept -> array_view<const byte, sizeof(T)> {
        return as_bytes<const T, 1>(array_view<const T, 1> { &value, 1 });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, usize EXTENT>
    constexpr auto as_mutable_bytes(array_view<T, EXTENT> container) noexcept
      -> array_view<byte, get_byte_extent_value_of<T, EXTENT>()> {
        return std::as_writable_bytes<T, EXTENT>(container);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_mutable_bytes(T* const ptr, usize size) noexcept -> byte_view_mut {
        return std::as_writable_bytes(array_view<T> { ptr, size });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_mutable_bytes(T& value) noexcept -> array_view<byte, sizeof(T)> {
        return std::as_writable_bytes<T, 1>(array_view<T, 1> { &value, 1 });
    }

    // /////////////////////////////////////
    // /////////////////////////////////////
    // template<class T, usize EXTENT>
    // STORMKIT_FORCE_INLINE
    // constexpr auto bytes_as(byte_view<EXTENT> bytes) noexcept -> const T& {
    //     if constexpr (EXTENT != std::dynamic_extent) EXPECTS(EXTENT == sizeof(T));
    //     EXPECTS(stdr::size(bytes) == sizeof(T));
    //     return *std::launder(std::bit_cast<const T* const>(stdr::data(bytes)));
    // }

    // /////////////////////////////////////
    // /////////////////////////////////////
    // template<typename T, stdr::range Range>
    //     requires(meta::SameAs<meta::ToPlainType<meta::ContainedType<Range>>, byte>)
    // STORMKIT_FORCE_INLINE
    // constexpr auto bytes_as_span(const Range& bytes) noexcept -> array_view<const T> {
    //     return array_view { std::launder(std::bit_cast<const T* const>(stdr::data(bytes))), stdr::size(bytes) / sizeof(T) };
    // }

    // /////////////////////////////////////
    // /////////////////////////////////////
    // template<typename T, usize EXTENT>
    // STORMKIT_FORCE_INLINE
    // constexpr auto bytes_as_span(byte_view<EXTENT> bytes) noexcept
    //   -> array_view<const T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)> {
    //     if constexpr (EXTENT != std::dynamic_extent)
    //         return array_view<const T, EXTENT / sizeof(T)> { std::bit_cast<const T* const>(stdr::data(bytes)),
    //                                                          EXTENT / sizeof(T) };
    //     else
    //         return array_view { std::launder(std::bit_cast<const T* const>(stdr::data(bytes))), stdr::size(bytes) / sizeof(T)
    //         };
    // }

    // /////////////////////////////////////
    // /////////////////////////////////////
    // template<class T, usize EXTENT>
    // STORMKIT_FORCE_INLINE
    // constexpr auto bytes_mut_as(byte_view_mut <EXTENT> bytes) noexcept -> T& {
    //     if constexpr (EXTENT != std::dynamic_extent) EXPECTS(EXTENT == sizeof(T));
    //     EXPECTS(stdr::size(bytes) == sizeof(T));
    //     return *std::launder(std::bit_cast<T* const>(stdr::data(bytes)));
    // }

    // /////////////////////////////////////
    // /////////////////////////////////////
    // template<typename T, stdr::range Range>
    //     requires(meta::SameAs<meta::ContainedType<Range>, byte> and not meta::IsConst<Range>)
    // STORMKIT_FORCE_INLINE
    // constexpr auto bytes_mut_as_span(Range& bytes) noexcept -> array_view<T> {
    //     return array_view { std::launder(std::bit_cast<T* const>(stdr::data(bytes))), stdr::size(bytes) / sizeof(T) };
    // }

    // /////////////////////////////////////
    // /////////////////////////////////////
    // template<typename T, usize EXTENT>
    // STORMKIT_FORCE_INLINE
    // constexpr auto bytes_mut_as_span(byte_view_mut <EXTENT> bytes) noexcept
    //   -> array_view<T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)> {
    //     if constexpr (EXTENT != std::dynamic_extent)
    //         return array_view<T, EXTENT / sizeof(T)> { std::bit_cast<T* const>(stdr::data(bytes)), EXTENT / sizeof(T) };
    //     else
    //         return array_view { std::launder(std::bit_cast<T* const>(stdr::data(bytes))), stdr::size(bytes) / sizeof(T) };
    // }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, usize N>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto into_bytes(const T (&bytes)[N]) noexcept -> byte_array<N> {
        EXPECTS(static_cast<T>(static_cast<byte>(bytes[0])) == bytes[0]);
        auto out = byte_array<N> {};
        auto i   = 0_usize;
        for (auto&& byte : bytes) out[i++] = static_cast<enum byte>(byte);
        return out;
    }

    namespace literals {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
        constexpr auto operator""_b(unsigned long long value) noexcept -> byte {
            return static_cast<byte>(value);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST
        constexpr auto operator""_kb(unsigned long long x) noexcept -> u64 {
            return x * 1000ULL;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST
        constexpr auto operator""_mb(unsigned long long x) noexcept -> u64 {
            return x * 1000_kb;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST
        constexpr auto operator""_gb(unsigned long long x) noexcept -> u64 {
            return x * 1000_mb;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST
        constexpr auto operator""_kib(unsigned long long x) noexcept -> u64 {
            return x * 1024;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST
        constexpr auto operator""_mib(unsigned long long x) noexcept -> u64 {
            return x * 1024_kib;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_CONST
        constexpr auto operator""_gib(unsigned long long x) noexcept -> u64 {
            return x * 1024_mib;
        }
    } // namespace literals
}} // namespace stormkit::core
