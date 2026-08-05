// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.containers.safecasts;

import std;

import stormkit.core.types;
import stormkit.core.errors;
import stormkit.core.typesafe.safecasts;
import stormkit.core.contract;
import stormkit.core.meta.concepts;
import stormkit.core.meta.type_manipulation;

namespace stdr = std::ranges;

template<typename T, stormkit::usize EXTENT>
consteval auto get_byte_extent_value_of() {
    if constexpr (EXTENT == std::dynamic_extent) return EXTENT;
    else if constexpr (stormkit::meta::Is<stormkit::meta::RemoveConst<T>, void>)
        return EXTENT;
    else
        return EXTENT * sizeof(T);
}

template<auto>
struct require_constant;

template<class R>
constexpr auto get_constexpr_size() -> stormkit::usize {
    if constexpr (std::is_bounded_array_v<R>) return std::extent_v<R>;
    else if constexpr (stormkit::meta::IsStdArray<R>)
        return std::tuple_size_v<R>;
    else if constexpr (stormkit::meta::IsStdSpan<R>)
        return R::extent;
    else if constexpr (stdr::sized_range<R> && requires { typename require_constant<R::size()>; })
        return R::size();
    else
        return std::dynamic_extent;
}

export namespace stormkit { inline namespace core {
    struct Bytes {};

    template<typename From>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<Bytes>, From& value, source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::ForwardConst<From, byte>, get_byte_extent_value_of<From, 1>>;

    template<typename From, usize Extent>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<Bytes>,
                              array_view<From> value,
                              source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::ForwardConst<From, byte>, get_byte_extent_value_of<From, Extent>>;

    template<stdr::contiguous_range From>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<Bytes>, From& value, source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::ForwardConst<From, byte>, get_byte_extent_value_of<From, get_constrexpr_size<From>()>>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename From>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<Bytes>, From& value, source_location_arg) noexcept
      -> array_view<meta::ForwardConst<From, byte>, get_byte_extent_value_of<From, 1>> {
        return { std::bit_cast<meta::ForwardConst<From, byte>*>(&value), get_byte_extent_value_of<From, 1> };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename From, usize Extent>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<Bytes>, array_view<From, Extent> value, source_location_arg) noexcept
      -> array_view<meta::ForwardConst<From, byte>, get_byte_extent_value_of<From, Extent>> {
        return { std::bit_cast<meta::ForwardConst<From, byte>*>(&value), get_byte_extent_value_of<From, 1> };
    }

    template<stdr::contiguous_range From>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<Bytes>, From& value, source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::ForwardConst<From, byte>, get_byte_extent_value_of<From, get_constexpr_size<From>()>> {
        return { std::bit_cast<meta::ForwardConst<From, byte>*>(stdr::data(value)),
                 get_byte_extent_value_of<From, get_constexpr_size<From>()> };
    }

    static_cast(tag_invoke(as_fn<Bytes>, array_view<const char*> {}));
}} // namespace stormkit::core
