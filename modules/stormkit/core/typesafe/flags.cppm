// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#define SUBSTITUTION 0

export module stormkit.core:typesafe.flags;

import std;

import :meta;

import :string.constexpr_string;
import :string.aliases;

import :math.combinatoric;

import :typesafe.integer;

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

#if SUBSTITUTION
        template<meta::IsEnumeration T, usize N, T DEFAULT_VALUE, usize BUF_LEN = 50>
        consteval auto generate_substitutions_as_string_for(string_view                                prefix,
                                                            const array<std::pair<T, string_view>, N>& mapping,
                                                            char separator = '|') noexcept -> decltype(auto);

        template<meta::IsEnumeration T, usize N, usize BUF_LEN = 50>
        consteval auto generate_substitutions_as_string_for(string_view                                prefix,
                                                            const array<std::pair<T, string_view>, N>& mapping,
                                                            char separator = '|') noexcept -> decltype(auto);
#endif
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

#if SUBSTITUTION
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration T, usize N, T DEFAULT_VALUE, usize BUF_LEN>
    consteval auto generate_substitutions_as_string_for(string_view                                prefix,
                                                        const array<std::pair<T, string_view>, N>& mapping,
                                                        char separator) noexcept -> decltype(auto) {
        constexpr auto OUT_SIZE = [] {
            auto res = 0uz;

            constexpr auto n_fact = math::factoriel(N);

            if constexpr (static_cast<int>(DEFAULT_VALUE) == 0)
                for (auto R = 0uz; R < (N - 1); ++R) res += n_fact / (math::factoriel(N - R) * math::factoriel(R));
            else
                for (auto R = 0uz; R < N; ++R) res += n_fact / (math::factoriel(N - R) * math::factoriel(R));

            return res + 1;
        }();

        auto out   = array<std::pair<T, meta::ConstexprString<BUF_LEN>>, OUT_SIZE> {};
        auto queue = dynarray<std::tuple<T, string, bool>> {};
        for (const auto& [k, v] : mapping) queue.emplace_back(k, string { v }, true);

        auto i = 0uz;
        while (not stdr::empty(queue)) {
            const auto [key, str, single_value] = queue.back();
            if (not stdr::any_of(out, [&key](auto& pair) noexcept { return pair.first == key; })) {
                auto& [k, v] = out[i];
                k            = key;

                auto out_string = string { prefix };
                if (single_value) out_string += str;
                else {
                    out_string += "(";
                    out_string += str;
                    out_string += ")";
                }

                stdr::copy(out_string, stdr::begin(v));
                v.update_size();

                i += 1;
            }
            if (key != DEFAULT_VALUE) {
                for (const auto& [k, v] : mapping) {
                    const auto has_key = stdr::any_of(queue, ([_k = k | key](auto&& tuple) noexcept {
                                                          const auto& [_key, _, _] = tuple;
                                                          return _key == _k;
                                                      }));

                    if (not has_key) {
                        auto str2 = str;
                        if (k != DEFAULT_VALUE) {
                            str2 += " ";
                            str2 += separator;
                            str2 += " ";
                            str2 += v;
                        }
                        queue.emplace(stdr::begin(queue), key | k, str2, false);
                    }
                }
            }
            queue.pop_back();
        }
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration T, usize N, usize BUF_LEN>
    consteval auto generate_substitutions_as_string_for(string_view                                prefix,
                                                        const array<std::pair<T, string_view>, N>& mapping,
                                                        char separator) noexcept -> decltype(auto) {
        constexpr auto OUT_SIZE = [] {
            auto res = 0uz;

            constexpr auto n_fact = math::factoriel(N);
            for (auto R = 0uz; R < N; ++R) res += n_fact / (math::factoriel(N - R) * math::factoriel(R));

            return res;
        }();

        auto out   = array<std::pair<T, meta::ConstexprString<BUF_LEN>>, OUT_SIZE> {};
        auto queue = dynarray<std::tuple<T, string, bool>> {};
        for (const auto& [k, v] : mapping) queue.emplace_back(k, string { v }, true);

        auto i = 0uz;
        while (not stdr::empty(queue)) {
            const auto [key, str, single_value] = queue.back();
            if (not stdr::any_of(out, [&key](auto& pair) noexcept { return pair.first == key; })) {
                auto& [k, v] = out[i];
                k            = key;

                auto out_string = string { prefix };
                if (single_value) out_string += str;
                else {
                    out_string += "(";
                    out_string += str;
                    out_string += ")";
                }

                stdr::copy(out_string, stdr::begin(v));
                v.update_size();

                i += 1;
            }
            for (const auto& [k, v] : mapping) {
                const auto has_key = stdr::any_of(queue, [_k = k | key](auto tuple) noexcept {
                    const auto& [_key, _, _] = tuple;
                    return _key == _k;
                });

                if (not has_key) queue.emplace(stdr::begin(queue), key | k, str + " " + separator + " " + v, false);
            }
            queue.pop_back();
        }
        return out;
    }
#endif

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
