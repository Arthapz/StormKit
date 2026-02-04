// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:typesafe.flags;

import std;

import :meta;

import :string.constexpr_string;

import :math.combinatoric;

import :typesafe.integer;

export {
    namespace stormkit { inline namespace core {
        namespace details {
            template<stormkit::meta::IsEnumeration T>
            struct EnableBitmaskOperators {
                constexpr EnableBitmaskOperators() = default;
                static constexpr auto enable       = false;
            };

            template<stormkit::meta::IsEnumeration T>
            inline constexpr auto BITMASK_OPERATORS_ENABLED = EnableBitmaskOperators<T>::enable;
        } // namespace details

        namespace meta {
            template<class T>
            concept IsFlag = (IsScopedEnumeration<meta::CanonicalType<T>> and core::details::BITMASK_OPERATORS_ENABLED<T>)
                             or IsPlainEnumeration<meta::CanonicalType<T>>;
        }

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

        template<meta::IsEnumeration T, usize N, T DEFAULT_VALUE, usize BUF_LEN = 50>
        consteval auto generate_substitutions_as_string_for(std::string_view                                     prefix,
                                                            const std::array<std::pair<T, std::string_view>, N>& mapping,
                                                            char separator = '|') noexcept -> decltype(auto);

        template<meta::IsEnumeration T, usize N, usize BUF_LEN = 50>
        consteval auto generate_substitutions_as_string_for(std::string_view                                     prefix,
                                                            const std::array<std::pair<T, std::string_view>, N>& mapping,
                                                            char separator = '|') noexcept -> decltype(auto);
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

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsEnumeration T, usize N, T DEFAULT_VALUE, usize BUF_LEN>
    consteval auto generate_substitutions_as_string_for(std::string_view                                     prefix,
                                                        const std::array<std::pair<T, std::string_view>, N>& mapping,
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

        auto out   = std::array<std::pair<T, meta::ConstexprString<BUF_LEN>>, OUT_SIZE> {};
        auto queue = std::vector<std::tuple<T, std::string, bool>> {};
        for (const auto& [k, v] : mapping) queue.emplace_back(k, std::string { v }, true);

        auto i = 0uz;
        while (not stdr::empty(queue)) {
            const auto [key, string, single_value] = queue.back();
            if (not stdr::any_of(out, [&key](auto& pair) noexcept { return pair.first == key; })) {
                auto& [k, v] = out[i];
                k            = key;

                auto out_string = std::string { prefix };
                if (single_value) out_string += string;
                else {
                    out_string += "(";
                    out_string += string;
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
                        auto str = string;
                        if (k != DEFAULT_VALUE) {
                            str += " ";
                            str += separator;
                            str += " ";
                            str += v;
                        }
                        queue.emplace(stdr::begin(queue), key | k, str, false);
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
    consteval auto generate_substitutions_as_string_for(std::string_view                                     prefix,
                                                        const std::array<std::pair<T, std::string_view>, N>& mapping,
                                                        char separator) noexcept -> decltype(auto) {
        constexpr auto OUT_SIZE = [] {
            auto res = 0uz;

            constexpr auto n_fact = math::factoriel(N);
            for (auto R = 0uz; R < N; ++R) res += n_fact / (math::factoriel(N - R) * math::factoriel(R));

            return res;
        }();

        auto out   = std::array<std::pair<T, meta::ConstexprString<BUF_LEN>>, OUT_SIZE> {};
        auto queue = std::vector<std::tuple<T, std::string, bool>> {};
        for (const auto& [k, v] : mapping) queue.emplace_back(k, std::string { v }, true);

        auto i = 0uz;
        while (not stdr::empty(queue)) {
            const auto [key, string, single_value] = queue.back();
            if (not stdr::any_of(out, [&key](auto& pair) noexcept { return pair.first == key; })) {
                auto& [k, v] = out[i];
                k            = key;

                auto out_string = std::string { prefix };
                if (single_value) out_string += string;
                else {
                    out_string += "(";
                    out_string += string;
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

                if (not has_key) queue.emplace(stdr::begin(queue), key | k, string + " " + separator + " " + v, false);
            }
            queue.pop_back();
        }
        return out;
    }
}} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator|(const T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(static_cast<Underlying>(lhs) | static_cast<Underlying>(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator&(const T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(static_cast<Underlying>(lhs) & static_cast<Underlying>(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator^(const T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(static_cast<Underlying>(lhs) ^ static_cast<Underlying>(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator~(const T& lhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    using Underlying = meta::UnderlyingType<T>;
    return static_cast<T>(~static_cast<Underlying>(lhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator|=(T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    lhs = lhs | rhs;
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator&=(T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    lhs = lhs & rhs;
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::meta::IsFlag T>
STORMKIT_FORCE_INLINE
constexpr auto operator^=(T& lhs, const T& rhs) noexcept -> decltype(auto) {
    using namespace stormkit;
    lhs = lhs ^ rhs;
    return lhs;
}
