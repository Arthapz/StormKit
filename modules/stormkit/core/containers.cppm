// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core.containers;

import std;

import stormkit.core.meta.type_manipulation;

export import stormkit.core.containers.ringbuffer;
export import stormkit.core.containers.tree;
export import stormkit.core.containers.dag;
export import stormkit.core.containers.utils;
export import stormkit.core.containers.raii_capsule;
export import stormkit.core.containers.shmbuffer;
export import stormkit.core.containers.hash_map;
export import stormkit.core.containers.safecasts;

export namespace std {
    template<class... Ts, class CharT>
    struct formatter<std::variant<Ts...>, CharT> {
        template<class ParseContext>
        constexpr auto parse(ParseContext& ctx) noexcept -> decltype(ctx.begin());

        template<class FormatContext>
        auto format(const std::variant<Ts...>&, FormatContext& ctx) const -> decltype(ctx.out());
    };
} // namespace std

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace std {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Ts, class CharT>
    template<class ParseContext>
    constexpr auto formatter<std::variant<Ts...>, CharT>::parse(ParseContext& ctx) noexcept -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Ts, class CharT>
    template<class FormatContext>
    inline auto formatter<std::variant<Ts...>, CharT>::format(const std::variant<Ts...>& variant, FormatContext& ctx) const
      -> decltype(ctx.out()) {
        return std::visit(
          [&ctx](auto&& value) mutable noexcept {
              using T = stormkit::meta::CanonicalT<decltype(value)>;

              return std::formatter<T> {}.format(value, ctx);
          },
          variant);
    }
} // namespace std
