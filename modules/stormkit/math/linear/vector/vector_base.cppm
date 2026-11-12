// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/macro_utils.hpp>
#include <stormkit/core/platform_macro.hpp>

#define SWIZZLE_INDEX_x 0
#define SWIZZLE_INDEX_y 1
#define SWIZZLE_INDEX_z 2
#define SWIZZLE_INDEX_w 3

#define GEN_SWIZZLE2(a, b)                                                             \
    STORMKIT_FORCE_INLINE constexpr auto a##b() const noexcept -> vec<value_type, 2> { \
        auto out = vec<value_type, 2> {};                                              \
        out[0]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, a)];                  \
        out[1]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, b)];                  \
        return out;                                                                    \
    }
#define GEN_SWIZZLE3(a, b, c)                                                             \
    STORMKIT_FORCE_INLINE constexpr auto a##b##c() const noexcept -> vec<value_type, 3> { \
        auto out = vec<value_type, 3> {};                                                 \
        out[0]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, a)];                     \
        out[1]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, b)];                     \
        out[2]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, c)];                     \
        return out;                                                                       \
    }
#define GEN_SWIZZLE4(a, b, c, d)                                                             \
    STORMKIT_FORCE_INLINE constexpr auto a##b##c##d() const noexcept -> vec<value_type, 4> { \
        auto out = vec<value_type, 4> {};                                                    \
        out[0]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, a)];                        \
        out[1]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, b)];                        \
        out[2]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, c)];                        \
        out[3]   = this->storage[STORMKIT_CONCAT(SWIZZLE_INDEX_, d)];                        \
        return out;                                                                          \
    }

export module stormkit.math.linear.vector:base;

import std;

import stormkit.core.meta;
import stormkit.core.types;
import stormkit.core.typesafe;
import stormkit.core.ranges;
import stormkit.core.contract;
import stormkit.core.functional;
import stormkit.core.containers.utils;
import stormkit.core.containers.accessor;
import stormkit.core.string.format;

namespace stdr  = std::ranges;
namespace cmeta = stormkit::core::meta;

using namespace stormkit::literals;

export namespace stormkit::math {
    template<cmeta::arithmetic T, usize N>
    struct vec;
}

namespace stormkit::math {
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    struct STORMKIT_TRIVIALLY_RELOCATABLE alignas(array<T, N>) vec_base {
        using value_type   = T;
        using storage_type = array<T, N>;
        using size_type    = usize;
        template<typename Self>
        using accessor_view_type = accessor_view<T,
                                                 meta::forward_ref_to<Self, meta::forward_const_to<Self, storage_type>>,
                                                 Bound_accessors...>;

        using reference       = value_type&;
        using const_reference = const value_type&;
        template<bool Const>
        using conditional_reference = cmeta::conditional<Const, const_reference, reference>;

        using pointer       = storage_type::pointer;
        using const_pointer = storage_type::const_pointer;
        template<bool Const>
        using conditional_pointer = cmeta::conditional<Const, const_pointer, pointer>;

        using iterator       = storage_type::iterator;
        using const_iterator = storage_type::const_iterator;
        template<bool Const>
        using conditional_iterator = cmeta::conditional<Const, const_iterator, iterator>;

        using reverse_iterator       = storage_type::reverse_iterator;
        using const_reverse_iterator = storage_type::const_reverse_iterator;
        template<bool Const>
        using conditional_reverse_iterator = cmeta::conditional<Const, const_reverse_iterator, reverse_iterator>;

        storage_type storage;

        template<typename Self>
        [[nodiscard]]
        constexpr auto operator[](this Self& self STORMKIT_LIFETIMEBOUND, size_type i) noexcept
          -> conditional_reference<cmeta::const_qualified<Self>>;

        template<typename Self>
        [[nodiscard]]
        constexpr auto data(this Self& self STORMKIT_LIFETIMEBOUND) noexcept -> conditional_pointer<cmeta::const_qualified<Self>>;

        template<typename Self>
        [[nodiscard]]
        constexpr auto begin(this Self& self STORMKIT_LIFETIMEBOUND) noexcept
          -> conditional_iterator<cmeta::const_qualified<Self>>;
        [[nodiscard]]
        constexpr auto cbegin() const & noexcept STORMKIT_LIFETIMEBOUND -> const_iterator;

        template<typename Self>
        [[nodiscard]]
        constexpr auto end(this Self& self STORMKIT_LIFETIMEBOUND) noexcept -> conditional_iterator<cmeta::const_qualified<Self>>;
        [[nodiscard]]
        constexpr auto cend() const & noexcept STORMKIT_LIFETIMEBOUND -> const_iterator;

        template<typename Self>
        [[nodiscard]]
        constexpr auto rbegin(this Self& self STORMKIT_LIFETIMEBOUND) noexcept
          -> conditional_reverse_iterator<cmeta::const_qualified<Self>>;
        [[nodiscard]]
        constexpr auto crbegin() const & noexcept STORMKIT_LIFETIMEBOUND -> const_reverse_iterator;

        template<typename Self>
        [[nodiscard]]
        constexpr auto rend(this Self& self STORMKIT_LIFETIMEBOUND) noexcept
          -> conditional_reverse_iterator<cmeta::const_qualified<Self>>;
        [[nodiscard]]
        constexpr auto crend() const & noexcept STORMKIT_LIFETIMEBOUND -> const_reverse_iterator;

        constexpr auto fill(T value) noexcept -> void;

        template<typename Self>
        constexpr auto swap(this Self& self, Self& other) noexcept -> void;

        [[nodiscard]]
        static constexpr auto extent() noexcept -> size_type;

        template<typename Self>
        constexpr auto operator->(this Self&& self STORMKIT_LIFETIMEBOUND) noexcept -> accessor_view_type<Self&&>
            requires(sizeof...(Bound_accessors) > 0);
    };

    template<cmeta::arithmetic T, typename Storage_type>
    struct x_accessor {
        element_accessor<T, Storage_type, 0> x;
    };

    template<cmeta::arithmetic T, typename Storage_type>
    struct y_accessor {
        element_accessor<T, Storage_type, 1> y;
    };

    template<cmeta::arithmetic T, typename Storage_type>
    struct z_accessor {
        element_accessor<T, Storage_type, 2> z;
    };

    template<cmeta::arithmetic T, typename Storage_type>
    struct w_accessor {
        element_accessor<T, Storage_type, 3> w;
    };

    template<cmeta::arithmetic T>
    using vec1_base = vec_base<T, 1, x_accessor>;

    template<cmeta::arithmetic T>
    using vec2_base = vec_base<T, 2, x_accessor, y_accessor>;

    template<cmeta::arithmetic T>
    using vec3_base = vec_base<T, 3, x_accessor, y_accessor, z_accessor>;

    template<cmeta::arithmetic T, usize N>
    using vecn_base = vec_base<T, N, x_accessor, y_accessor, z_accessor, w_accessor>;
} // namespace stormkit::math

export namespace stormkit::math {
    template<cmeta::arithmetic T, usize N>
    constexpr auto operator==(const vec<T, N>& lhs, const vec<T, N>& rhs) noexcept -> bool;

    template<cmeta::arithmetic T, usize N>
    constexpr auto operator<=>(const vec<T, N>& lhs, const vec<T, N>& rhs) noexcept -> synth_three_way_result<T>;

    template<usize Index, cmeta::arithmetic T, usize N>
    constexpr auto get(vec<T, N>& vec) noexcept -> T&;
    template<usize Index, cmeta::arithmetic T, usize N>
    constexpr auto get(vec<T, N>&& vec) noexcept -> T;

    template<usize Index, cmeta::arithmetic T, usize N>
    constexpr auto get(const vec<T, N>& vec) noexcept -> const T&;
    template<usize Index, cmeta::arithmetic T, usize N>
    constexpr auto get(const vec<T, N>&& vec) noexcept -> T;

    template<cmeta::arithmetic T, usize N>
    constexpr auto swap(vec<T, N>& lhs, vec<T, N>& rhs) noexcept -> void;

    namespace meta {
        template<typename T>
        concept is_vec = cmeta::specialization_of_nttp_tv<T, vec>;

        template<typename T>
        concept is_vec_2 = is_vec<T> and T::extent() == 2;

        template<typename T>
        concept is_vec_3 = is_vec<T> and T::extent() == 3;

        template<typename T>
        concept is_vec_4 = is_vec<T> and T::extent() == 4;
    } // namespace meta

    template<cmeta::arithmetic T>
    struct vec<T, 1>: vec1_base<T> {
        using value_type = vec1_base<T>::value_type;
        using vec1_base<T>::operator->;
    };

    template<cmeta::arithmetic T>
    using vec1  = vec<T, 1>;
    using fvec1 = vec1<f32>;
    using ivec1 = vec1<i32>;
    using uvec1 = vec1<u32>;

    template<cmeta::arithmetic T>
    struct vec<T, 2>: vec2_base<T> {
        using value_type = vec2_base<T>::value_type;
        using vec2_base<T>::operator->;

        STORMKIT_FOR_EACH_COMBINATION(GEN_SWIZZLE2, (x, y), (x, y))
    };

    template<cmeta::arithmetic T>
    using vec2  = vec<T, 2>;
    using fvec2 = vec2<f32>;
    using ivec2 = vec2<i32>;
    using uvec2 = vec2<u32>;

    template<cmeta::arithmetic T>
    struct vec<T, 3>: vec3_base<T> {
        using value_type = vec3_base<T>::value_type;
        using vec3_base<T>::operator->;

        STORMKIT_FOR_EACH_COMBINATION(GEN_SWIZZLE2, (x, y), (x, y))
        STORMKIT_FOR_EACH_COMBINATION(GEN_SWIZZLE3, (x, y, z), (x, y, z), (x, y, z))
    };

    template<cmeta::arithmetic T>
    using vec3  = vec<T, 3>;
    using fvec3 = vec3<f32>;
    using ivec3 = vec3<i32>;
    using uvec3 = vec3<u32>;

    template<cmeta::arithmetic T, usize N>
        requires(N >= 4)
    struct vec<T, N>: vecn_base<T, N> {
        using value_type = vecn_base<T, N>::value_type;
        using vecn_base<T, N>::operator->;

        STORMKIT_FOR_EACH_COMBINATION(GEN_SWIZZLE2, (x, y), (x, y))
        STORMKIT_FOR_EACH_COMBINATION(GEN_SWIZZLE3, (x, y, z), (x, y, z), (x, y, z))
        STORMKIT_FOR_EACH_COMBINATION(GEN_SWIZZLE4, (x, y, z, w), (x, y, z, w), (x, y, z, w), (x, y, z, w))
    };

    template<cmeta::arithmetic T>
    using vec4  = vec<T, 4>;
    using fvec4 = vec4<f32>;
    using ivec4 = vec4<i32>;
    using uvec4 = vec4<u32>;

    template<typename CharT, cmeta::arithmetic T>
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 1>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out());

    template<typename CharT, cmeta::arithmetic T>
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 2>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out());

    template<typename CharT, cmeta::arithmetic T>
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 3>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out());

    template<typename CharT, cmeta::arithmetic T>
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 4>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out());

    template<cmeta::arithmetic To, cmeta::arithmetic T, usize N>
    constexpr auto tag_invoke(as_fn<To>, const vec<T, N>& vec, source_location_arg = std::source_location::current()) noexcept
      -> To;

    template<cmeta::arithmetic T, usize N>
    constexpr auto tag_invoke(cmeta::tag<mutable_view_of>, vec<T, N>& vec STORMKIT_LIFETIMEBOUND) noexcept -> array_view<T, N>;

    template<cmeta::arithmetic T, usize N>
    constexpr auto tag_invoke(cmeta::tag<view_of>, const vec<T, N>& vec STORMKIT_LIFETIMEBOUND) noexcept
      -> array_view<const T, N>;
} // namespace stormkit::math

export template<cmeta::arithmetic T, stormkit::usize N>
struct std::tuple_size<stormkit::math::vec<T, N>>: std::integral_constant<stormkit::usize, N> {};

export template<stormkit::usize Index, cmeta::arithmetic T, stormkit::usize N>
struct std::tuple_element<Index, stormkit::math::vec<T, N>> {
    using type = T;
};

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::math {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::operator[](this Self& self, size_type i) noexcept
      -> conditional_reference<cmeta::const_qualified<Self>> {
        EXPECTS(i < extent());
        return self.storage[i];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::data(this Self& self) noexcept
      -> conditional_pointer<cmeta::const_qualified<Self>> {
        return stdr::data(self.storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::begin(this Self& self) noexcept
      -> conditional_iterator<cmeta::const_qualified<Self>> {
        return stdr::begin(self.storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::cbegin() const & noexcept -> const_iterator {
        return stdr::cbegin(storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::end(this Self& self) noexcept
      -> conditional_iterator<cmeta::const_qualified<Self>> {
        return stdr::end(self.storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::cend() const & noexcept -> const_iterator {
        return stdr::cend(storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::rbegin(this Self& self) noexcept
      -> conditional_reverse_iterator<cmeta::const_qualified<Self>> {
        return stdr::rbegin(self.storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::crbegin() const & noexcept -> const_reverse_iterator {
        return stdr::crbegin(storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::rend(this Self& self) noexcept
      -> conditional_reverse_iterator<cmeta::const_qualified<Self>> {
        return stdr::rend(self.storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::crend() const & noexcept -> const_reverse_iterator {
        return stdr::crend(storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::fill(T value) noexcept -> void {
        return storage.fill(value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::swap(this Self& self, Self& other) noexcept -> void {
        return self.storage.swap(other.storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    STORMKIT_CONST STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::extent() noexcept -> size_type {
        return N;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N, template<class, class> typename... Bound_accessors>
    template<typename Self>
        STORMKIT_FORCE_INLINE
    constexpr auto vec_base<T, N, Bound_accessors...>::operator->(this Self&& self STORMKIT_LIFETIMEBOUND) noexcept
      -> accessor_view_type<Self&&>
        requires(sizeof...(Bound_accessors) > 0)
    {
        return accessor_view_type<Self&&> { std::forward_like<Self&&>(self.storage) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto operator==(const vec<T, N>& lhs, const vec<T, N>& rhs) noexcept -> bool {
        return lhs.storage == rhs.storage;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto operator<=>(const vec<T, N>& lhs, const vec<T, N>& rhs) noexcept -> synth_three_way_result<T> {
        return lhs.storage <=> rhs.storage;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize Index, cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto get(vec<T, N>& vec) noexcept -> T& {
        return vec[Index];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize Index, cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto get(vec<T, N>&& vec) noexcept -> T {
        return vec[Index];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize Index, cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto get(const vec<T, N>& vec) noexcept -> const T& {
        return vec[Index];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<usize Index, cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto get(const vec<T, N>&& vec) noexcept -> T {
        return vec[Index];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto swap(vec<T, N>& lhs, vec<T, N>& rhs) noexcept -> void {
        lhs.swap(rhs);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename CharT, cmeta::arithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 1>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out()) {
        format_to(ctx, "[vec1: .x = {}]", vec->x);
        return ctx.out();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename CharT, cmeta::arithmetic T>
       STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 2>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out()) {
        format_to(ctx, "[vec2: .x = {}, .y = {}]", vec->x, vec->y);
        return ctx.out();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename CharT, cmeta::arithmetic T>
       STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 3>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out()) {
        format_to(ctx, "[vec3: .x = {}, .y = {}, .z = {}]", vec->x, vec->y, vec->z);
        return ctx.out();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename CharT, cmeta::arithmetic T>
       STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(format_as_fn<CharT>, const vec<T, 4>& vec, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out()) {
        format_to(ctx, "[vec4: .x = {}, .y = {}, .z = {}, .w = {}]", vec->x, vec->y, vec->z, vec->w);
        return ctx.out();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(cmeta::tag<mutable_view_of>, vec<T, N>& vec STORMKIT_LIFETIMEBOUND) noexcept -> array_view<T, N> {
        return array_view<T, N> { vec };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(cmeta::tag<view_of>, const vec<T, N>& vec STORMKIT_LIFETIMEBOUND) noexcept
      -> array_view<const T, N> {
        return array_view<const T, N> { vec };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<cmeta::arithmetic To, cmeta::arithmetic T, usize N>
    constexpr auto tag_invoke(as_fn<To>, const vec<T, N>& vector, source_location_arg) noexcept -> vec<To, N> {
        auto out = vec<To, N> {};
        if constexpr (To::extent() > N)
            for (auto i : range(N)) out[i] = as<To>(vector[i]);
        else
            for (auto i : range(To::extent())) out[i] = as<To>(vector[i]);

        return out;
    }

    static_assert(alignof(vec<u8, 1>) == alignof(array<u8, 1>));
    static_assert(sizeof(vec<u8, 1>) == sizeof(array<u8, 1>));
    static_assert(alignof(vec<i16, 1>) == alignof(array<i16, 1>));
    static_assert(sizeof(vec<i16, 1>) == sizeof(array<i16, 1>));
    static_assert(alignof(vec<i32, 1>) == alignof(array<i32, 1>));
    static_assert(sizeof(vec<i32, 1>) == sizeof(array<i32, 1>));
    static_assert(alignof(vec<f32, 1>) == alignof(array<f32, 1>));
    static_assert(sizeof(vec<f32, 1>) == sizeof(array<f32, 1>));
    static_assert(alignof(vec<u8, 2>) == alignof(array<u8, 2>));
    static_assert(sizeof(vec<u8, 2>) == sizeof(array<u8, 2>));
    static_assert(alignof(vec<i16, 2>) == alignof(array<i16, 2>));
    static_assert(sizeof(vec<i16, 2>) == sizeof(array<i16, 2>));
    static_assert(alignof(vec<i32, 2>) == alignof(array<i32, 2>));
    static_assert(sizeof(vec<i32, 2>) == sizeof(array<i32, 2>));
    static_assert(alignof(vec<f32, 2>) == alignof(array<f32, 2>));
    static_assert(sizeof(vec<f32, 2>) == sizeof(array<f32, 2>));
    static_assert(alignof(vec<u8, 3>) == alignof(array<u8, 3>));
    static_assert(sizeof(vec<u8, 3>) == sizeof(array<u8, 3>));
    static_assert(alignof(vec<i16, 3>) == alignof(array<i16, 3>));
    static_assert(sizeof(vec<i16, 3>) == sizeof(array<i16, 3>));
    static_assert(alignof(vec<i32, 3>) == alignof(array<i32, 3>));
    static_assert(sizeof(vec<i32, 3>) == sizeof(array<i32, 3>));
    static_assert(alignof(vec<f32, 3>) == alignof(array<f32, 3>));
    static_assert(sizeof(vec<f32, 3>) == sizeof(array<f32, 3>));
    static_assert(alignof(vec<u8, 4>) == alignof(array<u8, 4>));
    static_assert(sizeof(vec<u8, 4>) == sizeof(array<u8, 4>));
    static_assert(cmeta::trivially_relocatable<vec<u8, 4>>);
    static_assert(alignof(vec<i16, 4>) == alignof(array<i16, 4>));
    static_assert(sizeof(vec<i16, 4>) == sizeof(array<i16, 4>));
    static_assert(cmeta::trivially_relocatable<vec<i16, 4>>);
    static_assert(alignof(vec<i32, 4>) == alignof(array<i32, 4>));
    static_assert(sizeof(vec<i32, 4>) == sizeof(array<i32, 4>));
    static_assert(cmeta::trivially_relocatable<vec<i32, 4>>);
    static_assert(alignof(vec<f32, 4>) == alignof(array<f32, 4>));
    static_assert(sizeof(vec<f32, 4>) == sizeof(array<f32, 4>));
    static_assert(cmeta::trivially_relocatable<vec<f32, 4>>);
} // namespace stormkit::math
