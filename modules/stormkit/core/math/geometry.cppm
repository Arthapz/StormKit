// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:math.geometry;

import std;

import :meta;
import :typesafe;

import :math.arithmetic;
import :math.extent;
import :math.linear.vector;

export namespace stormkit { inline namespace core { namespace math {
    template<core::meta::IsArithmetic T>
    struct rect {
        T           x;
        T           y;
        Positive<T> width;
        Positive<T> height;

        constexpr auto position() const noexcept -> vec2<T>;
        constexpr auto extent() const noexcept -> extent2<T>;

        template<core::meta::IsArithmetic U>
        constexpr auto to() const noexcept -> rect<U>;
    };

    using recti = rect<i32>;
    using rectu = rect<u32>;
    using rectf = rect<f32>;

    template<core::meta::IsArithmetic T>
    rect(T, T, T, T) -> rect<T>;

    template<typename T, typename FormatContext>
    constexpr auto format_as(const rect<T>& point, FormatContext& ctx) -> decltype(ctx.out());

    template<core::meta::IsArithmetic T>
    struct bounding_rect {
        T left;
        T top;
        T right;
        T bottom;

        constexpr auto topleft() const noexcept -> vec2<T>;
        constexpr auto bottomright() const noexcept -> vec2<T>;

        template<core::meta::IsArithmetic U>
        constexpr auto to() const noexcept -> rect<U>;
    };

    template<typename T, typename FormatContext>
    constexpr auto format_as(const bounding_rect<T>& point, FormatContext& ctx) -> decltype(ctx.out());

    template<typename T>
    constexpr auto to_bounding_rect(const rect<T>& _rect) noexcept -> bounding_rect<T>;

    template<typename T>
    constexpr auto to_rect(const bounding_rect<T>& _rect) noexcept -> rect<T>;

    template<typename T>
    constexpr auto AABB(const rect<T>& rect1, const rect<T>& rect2) noexcept -> bool;

    template<typename T>
    constexpr auto AABB(const vec2<T>& pos, const bounding_rect<T>& rect) noexcept -> bool;

    template<typename T>
    constexpr auto AABB(const vec2<T>& pos, const rect<T>& rect) noexcept -> bool;
}}} // namespace stormkit::core::math

namespace stormkit { inline namespace core { namespace math {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto rect<T>::position() const noexcept -> vec2<T> {
        return { x, y };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto rect<T>::extent() const noexcept -> extent2<T> {
        return { width.value, height.value };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto rect<T>::to() const noexcept -> rect<U> {
        return { as<U>(x), as<U>(y), as<U>(width), as<U>(height) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    constexpr auto format_as(const rect<T>& point, FormatContext& ctx) -> decltype(ctx.out()) {
        return std::format_to(ctx.out(),
                              "{{ rect: .x = {}, .y = {}, .width = {}, .height = {} }}",
                              point.x,
                              point.y,
                              point.width,
                              point.height);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto bounding_rect<T>::topleft() const noexcept -> vec2<T> {
        return { left, top };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto bounding_rect<T>::bottomright() const noexcept -> vec2<T> {
        return { right, bottom };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_FORCE_INLINE
    STORMKIT_PURE
    constexpr auto bounding_rect<T>::to() const noexcept -> rect<U> {
        return { as<U>(left), as<U>(top), as<U>(right), as<U>(bottom) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    constexpr auto format_as(const bounding_rect<T>& point, FormatContext& ctx) -> decltype(ctx.out()) {
        return std::format_to(ctx.out(),
                              "{{ bounding_rect: .left = {}, .top = {}, .right = {}, .bottom = {} }}",
                              point.left,
                              point.top,
                              point.right,
                              point.bottom);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto to_bounding_rect(const rect<T>& _rect) noexcept -> bounding_rect<T> {
        return { _rect.x, _rect.y, _rect.x + _rect.width, _rect.y + _rect.height };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto to_rect(const bounding_rect<T>& _rect) noexcept -> rect<T> {
        return { _rect.left, _rect.top, _rect.right - _rect.left, _rect.bottom - _rect.top };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto AABB(const bounding_rect<T>& rect1, const bounding_rect<T>& rect2) noexcept -> bool {
        return rect1.left < rect2.right and rect1.right > rect2.left and rect1.top < rect2.bottom and rect1.bottom > rect2.top;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto AABB(const rect<T>& rect1, const rect<T>& rect2) noexcept -> bool {
        return AABB(to_bounding_rect(rect1), to_bounding_rect(rect2));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto AABB(const vec2<T>& pos, const bounding_rect<T>& rect) noexcept -> bool {
        return pos.x >= rect.left and pos.x <= rect.right and pos.y >= rect.top and pos.y <= rect.bottom;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto AABB(const vec2<T>& pos, const rect<T>& rect) noexcept -> bool {
        return AABB(pos, to_bounding_rect(rect));
    }
}}} // namespace stormkit::core::math
