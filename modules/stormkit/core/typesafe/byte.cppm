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

import :utils.contract;
import :utils.tags;

namespace stdr = std::ranges;

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
    using Byte     = std::byte;
    using ByteView = std::span<const std::byte>;
    template<usize N>
    using ByteArray       = std::array<std::byte, N>;
    using ByteDynArray    = std::vector<std::byte>;
    using MutableByteView = std::span<std::byte>;

    template<typename T>
    constexpr auto zero_bytes(T& value) noexcept -> void;

    template<typename T>
    constexpr auto zeroed() noexcept -> T;

    template<typename T>
    [[nodiscard]]
    constexpr auto byte_swap(const T& value) noexcept -> T;

    // std::span
    using std::as_bytes;

    template<typename T>
    [[nodiscard]]
    constexpr auto as_bytes(const T* const ptr, usize size = 1) noexcept -> std::span<const byte>;

    template<stdr::range Range>
    [[nodiscard]]
    constexpr auto as_bytes(const Range& range) noexcept -> std::span<const byte>;

    [[nodiscard]]
    constexpr auto as_bytes(std::string_view string) noexcept -> std::span<const byte>;

    template<class T>
    [[nodiscard]]
    constexpr auto as_bytes(const T& value) noexcept -> std::span<const byte, sizeof(T)>;

    // std::span
    template<typename T, usize EXTENT>
    [[nodiscard]]
    constexpr auto as_bytes_mut(std::span<T, EXTENT> container) noexcept
      -> std::span<byte, get_byte_extent_value_of<T, EXTENT>()>;

    template<stdr::range Range>
    [[nodiscard]]
    constexpr auto as_bytes_mut(Range& range) noexcept -> std::span<byte>;

    template<typename T>
    [[nodiscard]]
    constexpr auto as_bytes_mut(T* const ptr, usize size = 1) noexcept -> std::span<byte>;

    template<class T>
    [[nodiscard]]
    constexpr auto as_bytes_mut(T& value) noexcept -> std::span<byte, sizeof(T)>;

    template<class T, usize EXTENT = std::dynamic_extent>
    [[nodiscard]]
    constexpr auto bytes_as(std::span<const byte, EXTENT> bytes) noexcept -> const T&;

    template<typename T, stdr::range Range>
        requires(meta::SameAs<meta::ToPlainType<meta::ContainedType<Range>>, byte>)
    [[nodiscard]]
    constexpr auto bytes_as_span(const Range& bytes) noexcept -> std::span<const T>;

    template<typename T, usize EXTENT = std::dynamic_extent>
    [[nodiscard]]
    constexpr auto bytes_as_span(std::span<const byte, EXTENT> bytes) noexcept
      -> std::span<const T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)>;

    template<class T, usize EXTENT>
    [[nodiscard]]
    constexpr auto bytes_mut_as(std::span<byte, EXTENT> bytes) noexcept -> T&;

    template<typename T, stdr::range Range>
        requires(meta::SameAs<meta::ContainedType<Range>, byte> and not meta::IsConst<Range>)
    [[nodiscard]]
    constexpr auto bytes_mut_as_span(Range& range) noexcept -> std::span<const T>;

    template<typename T, usize EXTENT>
    [[nodiscard]]
    constexpr auto bytes_mut_as_span(std::span<byte, EXTENT> bytes) noexcept
      -> std::span<T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)>;

    template<typename T, usize N>
    [[nodiscard]]
    constexpr auto into_bytes(const T (&bytes)[N]) noexcept -> ByteArray<N>;

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
        auto bytes = as_bytes_mut(value);
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
            auto repr = std::bit_cast<std::array<byte, sizeof(value)>>(value);

            stdr::reverse(repr);

            return std::bit_cast<T>(repr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(const T* const ptr, usize size) noexcept -> std::span<const byte> {
        return std::as_bytes(std::span<const T> { ptr, size });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::range Range>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(const Range& range) noexcept -> std::span<const byte> {
        return as_bytes(std::span { range });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(std::string_view value) noexcept -> std::span<const byte> {
        return std::as_bytes(std::span<const char> { stdr::data(value), stdr::size(value) });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes(const T& value) noexcept -> std::span<const byte, sizeof(T)> {
        return std::as_bytes<const T, 1>(std::span<const T, 1> { &value, 1 });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, usize EXTENT>
    constexpr auto as_bytes_mut(std::span<T, EXTENT> container) noexcept
      -> std::span<byte, get_byte_extent_value_of<T, EXTENT>()> {
        return std::as_writable_bytes<T, EXTENT>(container);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::range Range>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes_mut(Range& range) noexcept -> std::span<byte> {
        return as_bytes_mut(std::span { range });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes_mut(T* const ptr, usize size) noexcept -> std::span<byte> {
        return std::as_writable_bytes(std::span<T> { ptr, size });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_bytes_mut(T& value) noexcept -> std::span<byte, sizeof(T)> {
        return std::as_writable_bytes<T, 1>(std::span<T, 1> { &value, 1 });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, usize EXTENT>
    STORMKIT_FORCE_INLINE
    constexpr auto bytes_as(std::span<const byte, EXTENT> bytes) noexcept -> const T& {
        if constexpr (EXTENT != std::dynamic_extent) EXPECTS(EXTENT == sizeof(T));
        EXPECTS(stdr::size(bytes) == sizeof(T));
        return *std::bit_cast<const T* const>(stdr::data(bytes));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, stdr::range Range>
        requires(meta::SameAs<meta::ToPlainType<meta::ContainedType<Range>>, byte>)
    STORMKIT_FORCE_INLINE
    constexpr auto bytes_as_span(const Range& bytes) noexcept -> std::span<const T> {
        return std::span { std::bit_cast<const T* const>(stdr::data(bytes)), stdr::size(bytes) / sizeof(T) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, usize EXTENT>
    STORMKIT_FORCE_INLINE
    constexpr auto bytes_as_span(std::span<const byte, EXTENT> bytes) noexcept
      -> std::span<const T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)> {
        if constexpr (EXTENT != std::dynamic_extent)
            return std::span<const T, EXTENT / sizeof(T)> { std::bit_cast<const T* const>(stdr::data(bytes)),
                                                            EXTENT / sizeof(T) };
        else
            return std::span { std::bit_cast<const T* const>(stdr::data(bytes)), stdr::size(bytes) / sizeof(T) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, usize EXTENT>
    STORMKIT_FORCE_INLINE
    constexpr auto bytes_mut_as(std::span<byte, EXTENT> bytes) noexcept -> T& {
        if constexpr (EXTENT != std::dynamic_extent) EXPECTS(EXTENT == sizeof(T));
        EXPECTS(stdr::size(bytes) == sizeof(T));
        return *std::bit_cast<T* const>(stdr::data(bytes));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, stdr::range Range>
        requires(meta::SameAs<meta::ContainedType<Range>, byte> and not meta::IsConst<Range>)
    STORMKIT_FORCE_INLINE
    constexpr auto bytes_mut_as_span(Range& bytes) noexcept -> std::span<T> {
        return std::span { std::bit_cast<T* const>(stdr::data(bytes)), stdr::size(bytes) / sizeof(T) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, usize EXTENT>
    STORMKIT_FORCE_INLINE
    constexpr auto bytes_mut_as_span(std::span<byte, EXTENT> bytes) noexcept
      -> std::span<T, EXTENT == std::dynamic_extent ? EXTENT : EXTENT / sizeof(T)> {
        if constexpr (EXTENT != std::dynamic_extent)
            return std::span<T, EXTENT / sizeof(T)> { std::bit_cast<T* const>(stdr::data(bytes)), EXTENT / sizeof(T) };
        else
            return std::span { std::bit_cast<T* const>(stdr::data(bytes)), stdr::size(bytes) / sizeof(T) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, usize N>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto into_bytes(const T (&bytes)[N]) noexcept -> ByteArray<N> {
        EXPECTS(static_cast<T>(static_cast<byte>(bytes[0])) == bytes[0]);
        auto out = ByteArray<N> {};
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
