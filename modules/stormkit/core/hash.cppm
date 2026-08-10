// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.hash;

import std;

import stormkit.core.types;
import stormkit.core.meta.concepts;
import stormkit.core.meta.tag_invoke;
import stormkit.core.hash.crc;

export namespace stormkit { inline namespace core {
    namespace meta {
        template<class T>
        concept has_std_hash = requires(T a) { std::hash<T> {}(a); };

        template<typename T>
        concept hash_type = meta::is_any_of<T, hash32, hash64>;
    } // namespace meta

    template<meta::hash_type Ret, typename T>
    struct hash_fn final {
      private:
        template<typename... Ts>
        using invoke_result = meta::tag_invoke_result<hash_fn<Ret, T>, T, source_location_arg>;

        template<typename... Ts>
        static constexpr auto IS_TAG_INVOKABLE = meta::tag_invocable<hash_fn<Ret, T>, T, source_location_arg>;

      public:
        using param_type = meta::in<T>;

        [[nodiscard]]
        static constexpr auto operator()(const T& value) noexcept
            requires(not IS_TAG_INVOKABLE<T>)
        = delete ("No hasher defined for this type");

        [[nodiscard]]
        static constexpr auto operator()(param_type value) noexcept -> invoke_result<T>
            requires(meta::trivially_copyable<T> and not IS_TAG_INVOKABLE<T>);

        [[nodiscard]]
        static constexpr auto operator()(param_type value) noexcept -> invoke_result<T>
            requires(IS_TAG_INVOKABLE<T>);
    };

    template<meta::hash_type Ret, typename T>
    inline constexpr auto hash_cpo = hash_fn {};

    template<meta::hash_type Ret = hash64, typename... Ts>
        requires(sizeof...(Ts) >= 1)
    constexpr auto hash_of(Ts&&... values) noexcept -> Ret;

    template<meta::hash_type Ret = hash64, typename... Ts>
        requires(sizeof...(Ts) >= 1)
    constexpr auto hash_combine(Ret hash, Ts&&... args) noexcept -> Ret;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::hash_type Ret = hash64, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto operator()(param_type value) noexcept -> invoke_result<T>
        requires(meta::trivially_copyable<T> and not IS_TAG_INVOKABLE<T>)
    {
        if constexpr (meta::is<Ret, hash32>) return hash::crc32(as<Bytes>(arg));
        else
            return hash::crc64(as<Bytes>(arg));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::hash_type Ret = hash64, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto operator()(param_type value) noexcept -> invoke_result<T>
        requires(IS_TAG_INVOKABLE<T>)
    {
        return tag_invoke(hash_fn<Ret, T>, value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::hash_type Ret = hash64, typename... Ts>
        requires(sizeof...(Ts) >= 1)
    constexpr auto hash_of(const Ts&... values) noexcept -> Ret {
        auto out = Ret {};

        (out = hash_combine(out, value), ...);

        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::hash_type Ret, typename... Ts>
        requires(sizeof...(Ts) >= 1)
    constexpr auto hash_combine(Ret with, const Ts&... values) noexcept -> void {
        static constexpr auto combine = []<typename T>(Ret with, const T& value) static noexcept {
            if constexpr (stdr::input_range<T>)
                for (const auto& elem : value) stormkit::hash_combine<Ret>(with, elem);
            else
                with ^= hash_cpo<Ret, T>(value) + 0x9e3779b9 + (with << 6) + (with >> 2);
            return with;
        };

        (with = COMBINE(with, values), ...);

        return with;
    }

}} // namespace stormkit::core
