// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:hash.base;

import std;

import :meta;

export namespace stormkit { inline namespace core {
    using hash32 = std::uint32_t;
    using hash64 = std::uint64_t;

    namespace meta {
        template<typename T>
        concept HashType = meta::IsOneOf<T, hash32, hash64>;
    }

    template<meta::HashType Ret = hash32, typename T>
    constexpr auto hasher(const T& value) noexcept -> Ret = delete;

    namespace meta {
        template<typename T>
        concept HasHasher = requires(T&& value) {
            { hasher(std::forward<T>(value)) } -> meta::HashType;
        };
    } // namespace meta

    template<meta::HashType Ret = hash32, typename... Args>
        requires(sizeof...(Args) >= 2)
    constexpr auto hash(Args&&... values) noexcept -> Ret;

    template<meta::HashType Ret = hash32, typename T>
    constexpr auto hash(T&& value) noexcept -> Ret;

    template<meta::HashType Ret = hash32, meta::IsArithmetic T>
        requires(sizeof(T) <= sizeof(Ret))
    constexpr auto hasher(T value) noexcept -> Ret;

    template<meta::HashType Ret = hash32, meta::IsEnumeration T>
        requires(sizeof(T) <= sizeof(Ret))
    constexpr auto hasher(T value) noexcept -> Ret;

    template<meta::HashType Ret = hash32, typename T>
    constexpr auto hash_combine(Ret& hash, T&& value) noexcept -> void;

    template<meta::HashType Ret = hash32, typename... Args>
        requires(sizeof...(Args) >= 2)
    constexpr auto hash_combine(Ret& hash, Args&&... args) noexcept -> void;

    namespace literals {
        constexpr auto operator""_hash32(unsigned long long int) -> hash32;
        constexpr auto operator""_hash32(long double) -> hash32;
        constexpr auto operator""_hash64(unsigned long long int) -> hash64;
        constexpr auto operator""_hash64(long double) -> hash64;
    } // namespace literals
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, typename... Args>
        requires(sizeof...(Args) >= 2)
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto hash(Args&&... values) noexcept -> Ret {
        auto out = Ret { 0 };
        stormkit::hash_combine(out, std::forward<Args>(values)...);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto hash(T&& value) noexcept -> Ret {
        static_assert(meta::HasHasher<T> or meta::HasStdHashSpecialization<T>, "No hasher or std::hash specialization!");

        if constexpr (meta::HasHasher<T>) return hasher<Ret>(std::forward<T>(value));
        else {
            const auto _hasher = std::hash<meta::CanonicalType<T>> {};
            return static_cast<Ret>(_hasher(std::forward<T>(value)));
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, meta::IsArithmetic T>
        requires(sizeof(T) <= sizeof(Ret))
        STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto hasher(T value) noexcept -> Ret {
        return static_cast<Ret>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, meta::IsEnumeration T>
        requires(sizeof(T) <= sizeof(Ret))
        STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto hasher(T value) noexcept -> Ret {
        return static_cast<Ret>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto hash_combine(Ret& out, T&& value) noexcept -> void {
        if constexpr (std::ranges::range<T>)
            for (auto&& elem : value) stormkit::hash_combine<Ret>(out, elem);
        else { out ^= hash<Ret>(std::forward<T>(value)) + 0x9e3779b9 + (out << 6) + (out >> 2); }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, typename... Args>
        requires(sizeof...(Args) >= 2)
    STORMKIT_FORCE_INLINE
    constexpr auto hash_combine(Ret& out, Args&&... args) noexcept -> void {
#if defined(__cpp_expansion_statements) and __cpp_expansion_statements >= 202500L
        template for (constexpr auto elem : { std::forward<Args>(args)... })
          stormkit::hash_combine<Ret>(out, std::forward<decltype(elem)>(elem));
#else
        (stormkit::hash_combine<Ret>(out, std::forward<Args>(args)), ...);
#endif
    }

    namespace literals {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        constexpr auto operator""_hash32(unsigned long long int value) -> hash32 {
            return hash<hash32>(value);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        constexpr auto operator""_hash32(long double value) -> hash32 {
            return hash<hash32>(value);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        constexpr auto operator""_hash64(unsigned long long int value) -> hash64 {
            return hash<hash64>(value);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        constexpr auto operator""_hash64(long double value) -> hash64 {
            return hash<hash64>(value);
        }
    } // namespace literals
}} // namespace stormkit::core
