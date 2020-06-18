// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Hash.hpp
/// \author Arthapz

#pragma once

#include <functional>
#include <string>
#include <type_traits>

#include <gsl/string_span>

#include <storm/core/Span.hpp>

#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

namespace storm::core {
    /// \brief storm::core::enum_hash<T> is a callable object which compute the
    /// hash of an enum \tparam T the enum type \requires Type 'T' shall be an
    /// enum or an enum class
    template<typename T>
    struct enum_hash {
        /// \group Public Special Member Functions
        /// \brief Compute the hash of an enum
        /// \returns the hash of the enum
        constexpr Hash64 operator()(T val) const noexcept {
            using underlying_type = std::underlying_type_t<T>;

            auto val_c = static_cast<underlying_type>(val);

            return std::hash<underlying_type> {}(val_c);
        }

      private:
        using sfinae = std::enable_if_t<std::is_enum_v<T>>;
    };

    /// \brief storm::core::hash_combine<T> is a function which compute the hash
    /// of data \tparam T the data type \param hash the hash which will be
    /// combined \param v the data which will be hashed and combined
    template<class T>
    constexpr inline void hash_combine(core::Hash64 &hash, const T &v) noexcept {
        const auto hasher = std::hash<T> {};
        hash ^= hasher(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }

    /// \exclude
    template<typename Result, core::ArraySize size>
    constexpr inline Result ComponentHash(gsl::czstring_span<size> str) noexcept {
        return size == 0
                   ? 0xcbf29ce484222325UL
                   : (static_cast<core::Hash64>(str[0]) ^ ComponentHash<Result, std::size(str) - 1>(str[1])) *
                         0x100000001b3UL;
    }

    template<typename Result>
    constexpr inline Result ComponentHash(const char *str, core::ArraySize size) noexcept {
        return size == 0 ? 0xcbf29ce484222325UL
                         : (static_cast<core::ArraySize>(str[0]) ^
                            ComponentHash<Result>(str + 1, size - 1)) *
                               0x100000001b3UL;
    }

    template<typename Result>
    constexpr inline Result ComponentHash(std::string_view str) noexcept {
        return ComponentHash<Result>(std::data(str), std::size(str));
    }

    constexpr inline core::Hash64 operator"" _hash(const char *str, core::ArraySize size) noexcept {
        return ComponentHash<core::Hash64>(str, size);
    }
} // namespace storm::core

/// \exclude
namespace std {
    /// \exclude
    template<typename T>
    struct hash<std::vector<T>> {
        storm::core::Hash64 operator()(const std::vector<T> &in) const noexcept {
            auto size = std::size(in);
            auto seed = storm::core::Hash64 { 0 };
            for (auto i = std::size_t { 0u }; i < size; i++) storm::core::hash_combine(seed, in[i]);

            return seed;
        }
    };
} // namespace std

/// \brief HASH_FUNC is a macro helper which declare std::hash<> and
/// std::equal_to to a type \param x the type
#define HASH_FUNC(x)                                                                               \
    namespace std {                                                                                \
        template<>                                                                                 \
        struct STORM_PUBLIC hash<x> {                                                              \
            [[nodiscard]] storm::core::Hash64 operator()(const x &) const noexcept;                \
        };                                                                                         \
        template<>                                                                                 \
        struct equal_to<x> {                                                                       \
            [[nodiscard]] inline bool operator()(const x &first, const x &second) const noexcept { \
                static auto hasher = hash<x> {};                                                   \
                return hasher(first) == hasher(second);                                            \
            }                                                                                      \
        };                                                                                         \
    }
