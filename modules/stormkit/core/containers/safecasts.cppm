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
import stormkit.core.meta.tag_invoke;
import stormkit.core.meta.type_query;

namespace stdr = std::ranges;

template<typename T, stormkit::usize EXTENT>
consteval auto get_byte_extent_value_of() -> stormkit::usize {
    if constexpr (EXTENT == std::dynamic_extent) return EXTENT;
    else if constexpr (stormkit::meta::is<stormkit::meta::remove_const_of<T>, void>)
        return EXTENT;
    else
        return EXTENT * sizeof(T);
}

template<auto>
struct require_constant;

template<class R>
constexpr auto get_constexpr_size() -> stormkit::usize {
    if constexpr (std::is_bounded_array_v<R>) return std::extent_v<R>;
    else if constexpr (stormkit::meta::std_array<R>)
        return std::tuple_size_v<R>;
    else if constexpr (stormkit::meta::std_span<R>)
        return R::extent;
    else if constexpr (stdr::sized_range<R> && requires { typename require_constant<R::size()>; })
        return R::size();
    else if constexpr (stdr::contiguous_range<R>)
        return std::dynamic_extent;
    else
        return 1;
}

export namespace stormkit { inline namespace core {
    struct bytes_view {};

    template<meta::std_span From>
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<bytes_view>, From value, source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::forward_const_to<meta::value_type<From>, byte>,
                    get_byte_extent_value_of<meta::value_type<From>, get_constexpr_size<From>()>()>;

    template<stdr::contiguous_range From>
        requires(not meta::std_span<From>)
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<bytes_view>, From& value, source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::forward_const_to<From, byte>, get_byte_extent_value_of<From, get_constexpr_size<From>()>()>;

    template<typename From>
        requires(not stdr::contiguous_range<From>)
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<bytes_view>, From& value, source_location_arg = std::source_location::current()) noexcept
      -> array_view<meta::forward_const_to<From, byte>, get_byte_extent_value_of<From, get_constexpr_size<From>()>()>;

    template<typename To, usize EXTENT>
        requires(EXTENT == std::dynamic_extent or EXTENT == sizeof(To))
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>,
                              array_view<const byte, EXTENT> value,
                              source_location_arg = std::source_location::current()) noexcept -> To;

    template<stdr::contiguous_range To, usize EXTENT>
        requires(not meta::std_span<To>)
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>,
                              array_view<const byte, EXTENT> value,
                              usize                          count,
                              source_location_arg = std::source_location::current()) noexcept -> To;

    template<meta::std_array To, usize EXTENT>
        requires(EXTENT != std::dynamic_extent and (EXTENT / sizeof(meta::value_type<To>)) == get_constexpr_size<To>()
                 or EXTENT == std::dynamic_extent)
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>,
                              array_view<const byte, EXTENT> value,
                              source_location_arg = std::source_location::current()) noexcept -> dynarray<To>;

    template<meta::explicitly_convertible_to<byte> T, usize EXTENT>
    [[nodiscard]]
    constexpr auto tag_invoke(into_fn<bytes_view>,
                              array<T, EXTENT> value,
                              source_location_arg = std::source_location::current()) noexcept -> array<byte, EXTENT>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::std_span From>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<bytes_view>, From value, source_location_arg) noexcept
      -> array_view<meta::forward_const_to<meta::value_type<From>, byte>,
                    get_byte_extent_value_of<meta::value_type<From>, get_constexpr_size<From>()>()> {
        return { std::bit_cast<meta::forward_const_to<From, byte>*>(stdr::data(value)),
                 get_byte_extent_value_of<From, get_constexpr_size<From>()>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::contiguous_range From>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<bytes_view>, From& value, source_location_arg) noexcept
      -> array_view<meta::forward_const_to<From, byte>, get_byte_extent_value_of<From, get_constexpr_size<From>()>()> {
        return { std::bit_cast<meta::forward_const_to<From, byte>*>(stdr::data(value)),
                 get_byte_extent_value_of<From, get_constexpr_size<From>()>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename From>
        requires(not stdr::contiguous_range<From>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<bytes_view>, From& value, source_location_arg) noexcept
      -> array_view<meta::forward_const_to<From, byte>, get_byte_extent_value_of<From, 1>> {
        return { std::bit_cast<meta::forward_const_to<From, byte>*>(&value), get_byte_extent_value_of<From, 1>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, usize EXTENT>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, array_view<const byte, EXTENT> value, source_location_arg) noexcept -> To {
        if constexpr (EXTENT == std::dynamic_extent) expects(stdr::size(value) == sizeof(To), "Invalid cast of bytes to type T!");

        return std::bit_cast<To>(*stdr::data(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::contiguous_range To, usize EXTENT>
        requires(not meta::std_span<To>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, array_view<const byte, EXTENT> value, usize count, source_location_arg) noexcept -> To {
        using To_value_type           = meta::value_type<To>;
        static constexpr auto TO_SIZE = sizeof(To_value_type);

        if constexpr (EXTENT != std::dynamic_extent) {
            expects((EXTENT / TO_SIZE) == count);
            return To { std::from_range, std::bit_cast<array<To_value_type, EXTENT / TO_SIZE>>(*stdr::data(value)) };
        } else {
            expects((stdr::size(value) / TO_SIZE) == count);

            auto out = To {};
            out.reserve(count);

            for (auto it : range(count)) out.emplace_back(std::bit_cast<To>(*(stdr::data(value) + (it * TO_SIZE))));

            return out;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::std_array To, usize EXTENT>
        requires(EXTENT != std::dynamic_extent and (EXTENT / sizeof(meta::value_type<To>)) == get_constexpr_size<To>()
                 or EXTENT == std::dynamic_extent)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, array_view<const byte, EXTENT> value, source_location_arg) noexcept -> To {
        using To_value_type           = meta::value_type<To>;
        static constexpr auto TO_SIZE = sizeof(To_value_type);
        static constexpr auto COUNT   = get_constexpr_size<To>();

        if constexpr (EXTENT != std::dynamic_extent) expects((EXTENT / TO_SIZE) == COUNT);
        else
            expects((stdr::data(value) / TO_SIZE) == COUNT);
        return std::bit_cast<To>(*stdr::data(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::explicitly_convertible_to<byte> T, usize EXTENT>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto tag_invoke(into_fn<bytes_view>, array<T, EXTENT> values, source_location_arg) noexcept -> array<byte, EXTENT> {
        auto out = array<byte, EXTENT> {};
        stdr::transform(values, stdr::begin(out), [](auto&& value) static noexcept { return static_cast<byte>(value); });
        return out;
    }
}} // namespace stormkit::core
