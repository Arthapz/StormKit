// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#ifdef STORMKIT_COMPILER_MSVC
    #include <__msvc_int128.hpp>
#endif

export module stormkit.core:typesafe.integer;

import std;

import :meta;

export {
    namespace stormkit { inline namespace core {
        namespace cpp {
            using UChar     = unsigned char;
            using UShort    = unsigned short;
            using UInt      = unsigned int;
            using ULong     = unsigned long;
            using ULongLong = unsigned long long;

            using SChar    = signed char;
            using Short    = signed short;
            using Int      = signed int;
            using Long     = signed long;
            using LongLong = signed long long;

            using LongDouble = long double;
        } // namespace cpp

#define STORMKIT_ALLOW_PRIMITIVE_TYPE_IMPLICIT_CONVERSION
#ifdef STORMKIT_ALLOW_PRIMITIVE_TYPE_IMPLICIT_CONVERSION
        using u8  = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;
    #ifdef __SIZEOF_INT128__
        using u128 = unsigned __int128;
    #elif defined(STORMKIT_COMPILER_MSVC)
        using u128 = std::_Unsigned128;
    #endif

        using i8  = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;
    #ifdef __SIZEOF_INT128__
        using i128 = __int128;
    #elif defined(STORMKIT_COMPILER_MSVC)
        using i128 = std::_Signed128;
    #else
    #endif

        using usize   = std::size_t;
        using isize   = std::ptrdiff_t;
        using ioffset = std::ptrdiff_t;
        using uptr    = std::uintptr_t;
        using iptr    = std::intptr_t;
#else
        enum class u8 : std::uint8_t {
        };
        enum class u16 : std::uint16_t {
        };
        enum class u32 : std::uint32_t {
        };
        enum class u64 : std::uint64_t {
        };

    #ifdef __SIZEOF_INT128__
        enum class u128 : unsigned __int128 {
        };
    #elif defined(STORMKIT_COMPILER_MSVC)
        enum class u128 : std::_Unsigned128 {
        };
    #endif

        enum class i8 : std::int8_t {
        };
        enum class i16 : std::int16_t {
        };
        enum class i32 : std::int32_t {
        };
        enum class i64 : std::int64_t {
        };
    #ifdef __SIZEOF_INT128__
        enum class i128 : __int128 {
        };
    #elif defined(STORMKIT_COMPILER_MSVC)
        enum class i128 : std::_Signed128 {
        };
    #else
    #endif

        enum class usize : std::size_t {
        };
        enum class isize : std::ptrdiff_t {
        };
        enum class ioffset : std::ptrdiff_t {
        };
        enum class uptr : std::uintptr_t {
        };
        enum class iptr : std::intptr_t {
        };
#endif
        [[nodiscard]]
        constexpr auto operator""_i8(unsigned long long) noexcept -> i8;
        [[nodiscard]]
        constexpr auto operator""_i16(unsigned long long) noexcept -> i16;
        [[nodiscard]]
        constexpr auto operator""_i32(unsigned long long) noexcept -> i32;
        [[nodiscard]]
        constexpr auto operator""_i64(unsigned long long) noexcept -> i64;
        [[nodiscard]]
        constexpr auto operator""_i128(unsigned long long) noexcept -> i128;

        [[nodiscard]]
        constexpr auto operator""_u8(unsigned long long) noexcept -> u8;
        [[nodiscard]]
        constexpr auto operator""_u16(unsigned long long) noexcept -> u16;
        [[nodiscard]]
        constexpr auto operator""_u32(unsigned long long) noexcept -> u32;
        [[nodiscard]]
        constexpr auto operator""_u64(unsigned long long) noexcept -> u64;
        [[nodiscard]]
        constexpr auto operator""_u128(unsigned long long) noexcept -> u128;

        [[nodiscard]]
        constexpr auto operator""_usize(unsigned long long) noexcept -> usize;
        [[nodiscard]]
        constexpr auto operator""_isize(unsigned long long) noexcept -> isize;
        [[nodiscard]]
        constexpr auto operator""_ioffset(unsigned long long) noexcept -> ioffset;
        [[nodiscard]]
        constexpr auto operator""_uptr(unsigned long long) noexcept -> uptr;
        [[nodiscard]]
        constexpr auto operator""_iptr(unsigned long long) noexcept -> iptr;
    }} // namespace stormkit::core
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_i8(unsigned long long value) noexcept -> i8 {
        return static_cast<i8>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_i16(unsigned long long value) noexcept -> i16 {
        return static_cast<i16>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_i32(unsigned long long value) noexcept -> i32 {
        return static_cast<i32>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_i64(unsigned long long value) noexcept -> i64 {
        return static_cast<i64>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_i128(unsigned long long value) noexcept -> i128 {
        return static_cast<i128>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_u8(unsigned long long value) noexcept -> u8 {
        return static_cast<u8>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_u16(unsigned long long value) noexcept -> u16 {
        return static_cast<u16>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_u32(unsigned long long value) noexcept -> u32 {
        return static_cast<u32>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_u64(unsigned long long value) noexcept -> u64 {
        return static_cast<u64>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_u128(unsigned long long value) noexcept -> u128 {
        return static_cast<u128>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_usize(unsigned long long value) noexcept -> usize {
        return static_cast<usize>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_isize(unsigned long long value) noexcept -> isize {
        return static_cast<isize>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_ioffset(unsigned long long value) noexcept -> ioffset {
        return static_cast<ioffset>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_iptr(unsigned long long value) noexcept -> iptr {
        return static_cast<iptr>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST STORMKIT_INTRINSIC
    constexpr auto operator""_uptr(unsigned long long value) noexcept -> uptr {
        return static_cast<uptr>(value);
    }
}} // namespace stormkit::core
