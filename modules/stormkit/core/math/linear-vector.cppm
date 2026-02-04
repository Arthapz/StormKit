// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:math.linear.vector;

import std;

import :meta;

import :typesafe.integer;
import :typesafe.floating_point;

import :math.linear;

import :hash.base;

import :string.format;

export namespace stormkit { inline namespace core { namespace math {
    template<core::meta::IsArithmetic T>
    struct alignas(std::array<T, 2>) vec2 {
        using value_type  = T;
        using size_type   = usize;
        using extent_type = u8;

        static constexpr auto EXTENT = std::array<extent_type, 1> { 2uz };

        T x;
        T y;

        template<typename Self>
        constexpr auto operator[](this Self& self, usize i) noexcept -> core::meta::ForwardConst<Self, value_type&>;

        template<core::meta::IsArithmetic U>
        constexpr auto to() const noexcept -> vec2<U>;
    };

    using fvec2 = vec2<f32>;
    using ivec2 = vec2<i32>;
    using uvec2 = vec2<u32>;

    template<core::meta::IsArithmetic T>
    struct alignas(std::array<T, 3>) vec3 {
        using value_type  = T;
        using size_type   = usize;
        using extent_type = u8;

        static constexpr auto EXTENT = std::array<extent_type, 1> { 3uz };

        T x;
        T y;
        T z;

        template<typename Self>
        constexpr auto operator[](this Self& self, usize i) noexcept -> core::meta::ForwardConst<Self, value_type&>;

        template<core::meta::IsArithmetic U>
        constexpr auto to() const noexcept -> vec3<U>;
    };

    using fvec3 = vec3<f32>;
    using ivec3 = vec3<i32>;
    using uvec3 = vec3<u32>;

    template<core::meta::IsArithmetic T>
    struct alignas(std::array<T, 4>) vec4 {
        using value_type  = T;
        using size_type   = usize;
        using extent_type = u8;

        static constexpr auto EXTENT = std::array<extent_type, 1> { 4uz };

        T x;
        T y;
        T z;
        T w;

        template<typename Self>
        constexpr auto operator[](this Self& self, usize i) noexcept -> core::meta::ForwardConst<Self, value_type&>;

        template<core::meta::IsArithmetic U>
        constexpr auto to() const noexcept -> vec4<U>;
    };

    using fvec4 = vec4<f32>;
    using ivec4 = vec4<i32>;
    using uvec4 = vec4<u32>;

    namespace meta {
        template<typename T>
        concept IsVec2 = core::meta::IsSpecializationOf<T, vec2>;
        template<typename T>
        concept IsVec3 = core::meta::IsSpecializationOf<T, vec3>;

        template<typename T>
        concept IsVec = IsVec2<T> || IsVec3<T>;

        template<typename T, typename U>
        concept HasOneVecType = not(core::meta::IsMdspanType<T> and core::meta::IsMdspanType<U>)
                                or meta::IsVec<T>
                                or meta::IsVec<U>;
    } // namespace meta

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto add(const T& a, const T& b) noexcept -> T;

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto sub(const T& a, const T& b) noexcept -> T;

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto mul(const T& a, typename T::value_type b) noexcept -> T;

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto div(const T& a, typename T::value_type b) noexcept -> T;

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto dot(const T& a, const T& b) noexcept -> typename T::value_type;

    template<typename T>
    [[nodiscard]]
    constexpr auto cross(const vec3<T>& a, const vec3<T>& b) noexcept -> vec3<T>;

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto normalize(const T& a) noexcept -> T;

    template<meta::IsVec T>
    [[nodiscard]]
    constexpr auto as_mdspan(const T& value) noexcept -> VectorSpan<const typename T::value_type, T::EXTENT[0]>;

    template<meta::IsVec T>
        requires(not core::meta::IsConst<T>)
    [[nodiscard]]
    constexpr auto as_mdspan_mut(T& value) noexcept -> VectorSpan<typename T::value_type, T::EXTENT[0]>;

    template<core::meta::HashType Ret = hash32, core::meta::IsArithmetic T>
    constexpr auto hasher(const vec2<T>& value) noexcept -> Ret;

    template<core::meta::HashType Ret = hash32, core::meta::IsArithmetic T>
    constexpr auto hasher(const vec3<T>& value) noexcept -> Ret;

    template<core::meta::HashType Ret = hash32, core::meta::IsArithmetic T>
    constexpr auto hasher(const vec4<T>& value) noexcept -> Ret;

    template<core::meta::IsArithmetic T, typename FormatContext>
    auto format_as(const vec2<T>& value, FormatContext& ctx) noexcept -> decltype(ctx.out());

    template<core::meta::IsArithmetic T, typename FormatContext>
    auto format_as(const vec3<T>& value, FormatContext& ctx) noexcept -> decltype(ctx.out());

    template<core::meta::IsArithmetic T, typename FormatContext>
    auto format_as(const vec4<T>& value, FormatContext& ctx) noexcept -> decltype(ctx.out());
}}} // namespace stormkit::core::math

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;

namespace stormkit { inline namespace core { namespace math {
    static_assert(sizeof(uvec2) == sizeof(u32) * 2);
    static_assert(sizeof(vec2<u64>) == sizeof(u64) * 2);
    static_assert(sizeof(ivec2) == sizeof(i32) * 2);
    static_assert(sizeof(vec2<i64>) == sizeof(i64) * 2);
    static_assert(sizeof(fvec2) == sizeof(f32) * 2);
    static_assert(sizeof(vec2<f64>) == sizeof(f64) * 2);

    static_assert(sizeof(uvec3) == sizeof(u32) * 3);
    static_assert(sizeof(vec3<u64>) == sizeof(u64) * 3);
    static_assert(sizeof(ivec3) == sizeof(i32) * 3);
    static_assert(sizeof(vec3<i64>) == sizeof(i64) * 3);
    static_assert(sizeof(fvec3) == sizeof(f32) * 3);
    static_assert(sizeof(vec3<f64>) == sizeof(f64) * 3);

    static_assert(sizeof(uvec4) == sizeof(u32) * 4);
    static_assert(sizeof(vec4<u64>) == sizeof(u64) * 4);
    static_assert(sizeof(ivec4) == sizeof(i32) * 4);
    static_assert(sizeof(vec4<i64>) == sizeof(i64) * 4);
    static_assert(sizeof(fvec4) == sizeof(f32) * 4);
    static_assert(sizeof(vec4<f64>) == sizeof(f64) * 4);

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<typename Self>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto vec2<T>::operator[](this Self& self, usize i) noexcept -> core::meta::ForwardConst<Self, value_type&> {
        static constexpr auto* members = { &vec2::x, &vec2::y };

        return std::forward_like<Self>(self->*members[i]);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto vec2<T>::to() const noexcept -> vec2<U> {
        return { core::as<U>(x), core::as<U>(y) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<typename Self>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto vec3<T>::operator[](this Self& self, usize i) noexcept -> core::meta::ForwardConst<Self, value_type&> {
        static constexpr auto* members = { &vec3::x, &vec3::y, &vec3::z };

        return std::forward_like<Self>(self->*members[i]);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto vec3<T>::to() const noexcept -> vec3<U> {
        return { core::as<U>(x), core::as<U>(y), core::as<U>(z) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<typename Self>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto vec4<T>::operator[](this Self& self, usize i) noexcept -> core::meta::ForwardConst<Self, value_type&> {
        static constexpr auto* members = { &vec4::x, &vec4::y, &vec4::z, &vec4::w };

        return std::forward_like<Self>(self->*members[i]);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto vec4<T>::to() const noexcept -> vec4<U> {
        return { core::as<U>(x), core::as<U>(y), core::as<U>(z), core::as<U>(w) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto normalize(const T& a) noexcept -> T {
        auto out = T {};

        normalize(as_mdspan(a), as_mdspan_mut(out));

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto add(const T& a, const T& b) noexcept -> T {
        auto out = T {};

        add(as_mdspan(a), as_mdspan(b), as_mdspan_mut(out));

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto sub(const T& a, const T& b) noexcept -> T {
        auto out = T {};

        sub(as_mdspan(a), as_mdspan(b), as_mdspan_mut(out));

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto mul(const T& a, typename T::value_type b) noexcept -> T {
        auto out = T {};

        mul(as_mdspan(a), b, as_mdspan_mut(out));

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto div(const T& a, typename T::value_type b) noexcept -> T {
        auto out = T {};

        div(as_mdspan(a), b, as_mdspan_mut(out));

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto dot(const T& a, const T& b) noexcept -> typename T::value_type {
        return dot(as_mdspan(a), as_mdspan(b));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto cross(const vec3<T>& a, const vec3<T>& b) noexcept -> vec3<T> {
        auto out = vec3<T> {};

        cross(as_mdspan(a), as_mdspan(b), as_mdspan_mut(out));

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto as_mdspan(const T& value) noexcept -> VectorSpan<const typename T::value_type, T::EXTENT[0]> {
        return VectorSpan<const typename T::value_type, T::EXTENT[0]> { &value.x, T::EXTENT };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsVec T>
        requires(not core::meta::IsConst<T>)
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto as_mdspan_mut(T& value) noexcept -> VectorSpan<typename T::value_type, T::EXTENT[0]> {
        return VectorSpan<typename T::value_type, T::EXTENT[0]> { &value.x, T::EXTENT };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::HashType Ret, core::meta::IsArithmetic T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto hasher(const vec2<T>& value) noexcept -> Ret {
        return hash<Ret>(value.x, value.y);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::HashType Ret, core::meta::IsArithmetic T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto hasher(const vec3<T>& value) noexcept -> Ret {
        return hash<Ret>(value.x, value.y, value.z);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::HashType Ret, core::meta::IsArithmetic T>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto hasher(const vec4<T>& value) noexcept -> Ret {
        return hash<Ret>(value.x, value.y, value.z, value.w);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const vec2<T>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[vec2 x: {}, y: {}]", value.x, value.y);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const vec3<T>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[vec2 x: {}, y: {}, z: {}]", value.x, value.y, value.z);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::meta::IsArithmetic T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const vec4<T>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[vec2 x: {}, y: {}, z: {}, w: {}]", value.x, value.y, value.z, value.w);
    }
}}} // namespace stormkit::core::math
