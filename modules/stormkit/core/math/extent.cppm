// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:math.extent;

import std;

import :utils.contract;
import :meta;
import :hash;
import :typesafe;

using namespace stormkit;
using namespace std::literals;

namespace stdr = std::ranges;

export {
    namespace stormkit { inline namespace core { namespace math {
        template<core::meta::IsArithmetic T, usize N>
        struct extent;

        template<core::meta::IsArithmetic T>
        struct alignas(array<T, 2>) extent<T, 2> {
            static constexpr auto RANK = 2uz;
            using ValueType            = T;
            using OrderingType         = meta::ArithmeticOrderingType<T>;

            template<core::meta::IsArithmetic U>
            constexpr auto narrow_to() const noexcept -> extent<U, 2>;

            template<core::meta::IsArithmetic U>
            constexpr auto to(const std::source_location& = std::source_location::current()) const noexcept -> extent<U, 2>;

            template<usize NEW_RANK>
            constexpr auto to() const noexcept -> extent<T, NEW_RANK>;

            /// @brief The extent width
            ValueType width = 0;

            /// @brief The extent height
            ValueType height = 0;
        };

        template<core::meta::IsArithmetic T>
        using extent2 = extent<T, 2>;

        using fextent2 = extent2<f32>;
        using uextent2 = extent2<u32>;
        using iextent2 = extent2<i32>;

        template<core::meta::IsArithmetic T>
        struct alignas(array<T, 3>) extent<T, 3> {
            static constexpr auto RANK = 3uz;
            using ValueType            = T;
            using OrderingType         = meta::ArithmeticOrderingType<T>;

            template<core::meta::IsArithmetic U>
            constexpr auto narrow_to() const noexcept -> extent<U, 3>;

            template<core::meta::IsArithmetic U>
            constexpr auto to(const std::source_location& = std::source_location::current()) const noexcept -> extent<U, 3>;

            template<usize NEW_RANK>
            constexpr auto to() const noexcept -> extent<T, NEW_RANK>;

            /// @brief The extent width
            ValueType width = 0;

            /// @brief The extent height
            ValueType height = 0;

            /// @brief The extent depth
            ValueType depth = 1;
        };

        template<core::meta::IsArithmetic T>
        using extent3 = extent<T, 3>;

        using fextent3 = extent3<f32>;
        using uextent3 = extent3<u32>;
        using iextent3 = extent3<i32>;

        template<core::meta::IsArithmetic T>
        extent(T, T) -> extent<T, 2>;

        template<core::meta::IsArithmetic T>
        extent(T, T, T) -> extent<T, 3>;

        namespace meta {
            template<class T>
            concept IsExtent = requires(T&&) {
                typename T::ValueType;
                typename T::OrderingType;
                { T::RANK } -> core::meta::Is<const usize&>;
            };

            template<class T>
            concept IsExtent2 = IsExtent<T> and requires(T&&) { requires T::RANK == 2uz; };

            template<class T>
            concept IsExtent3 = IsExtent<T> and requires(T&&) { requires T::RANK == 3uz; };
        } // namespace meta

        /// @output_section Publics operators members
        /// @brief ValueType the ordering with an other extent.
        /// @param other ValueType extent to test
        /// @returns true if this extent is equal to `other`, otherwise returns false.
        template<meta::IsExtent Extent>
        [[nodiscard]]
        constexpr auto operator<=>(const Extent& first, const Extent& second) noexcept -> typename Extent::OrderingType;

        /// @output_section Publics operators members
        /// @brief ValueType the equality with an other extent.
        /// @param other ValueType extent to test
        /// @returns true if this extent is equal to `other`, otherwise returns false.
        template<meta::IsExtent Extent>
        [[nodiscard]]
        constexpr auto operator==(const Extent& first, const Extent& second) noexcept -> bool;

        /// @brief Multiply an extent with a factor.
        /// @param factor ValueType factor to multiply
        /// @returns A newly constructed extent equal to this extent multiplied with
        /// `factor`
        template<meta::IsExtent Extent>
        [[nodiscard]]
        constexpr auto operator*(Extent&& extent, typename Extent::ElemenType factor) noexcept
          -> core::meta::CanonicalType<Extent>;

        /// @brief Divide an extent with a factor.
        /// @param factor ValueType factor to divide
        /// @returns A newly constructed extent equal to this extent Divided with `factor`
        template<meta::IsExtent Extent>
        [[nodiscard]]
        constexpr auto operator/(Extent&& extent, typename Extent::ElemenType factor) noexcept
          -> core::meta::CanonicalType<Extent>;

        /// @brief Multiply this extent with a factor.
        /// @param factor ValueType factor to multiply
        /// @returns A reference to this after the multiplication with `factor`
        template<meta::IsExtent Extent>
        [[nodiscard]]
        constexpr auto operator*=(Extent& extent, typename Extent::ValueType factor) noexcept -> Extent&;

        /// @brief Divide this extent with a factor.
        /// @param factor ValueType factor to divide
        /// @returns A reference to this after the division with `factor`
        template<meta::IsExtent Extent>
        [[nodiscard]]
        constexpr auto operator/=(Extent& extent, typename Extent::ValueType factor) noexcept -> Extent&;

        template<meta::IsExtent2 Extent>
        auto to_string(const Extent& extent) noexcept -> string;

        template<meta::IsExtent3 Extent>
        auto to_string(const Extent& extent) noexcept -> string;

        template<core::meta::IsArithmetic T, typename FormatContext>
        auto format_as(const extent<T, 2>& extent, FormatContext& ctx) noexcept -> decltype(ctx.out());

        template<core::meta::IsArithmetic T, typename FormatContext>
        auto format_as(const extent<T, 3>& extent, FormatContext& ctx) noexcept -> decltype(ctx.out());

        template<core::meta::HashType Ret = hash32, core::meta::IsArithmetic T>
        constexpr auto hasher(const extent<T, 2>& extent) noexcept -> Ret;

        template<core::meta::HashType Ret = hash32, core::meta::IsArithmetic T>
        constexpr auto hasher(const extent<T, 3>& extent) noexcept -> Ret;
    }}} // namespace stormkit::core::math
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core { namespace math {
    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE
    constexpr auto extent<T, 2>::narrow_to() const noexcept -> extent<U, 2> {
        return { .width = narrow<U>(width), .height = narrow<U>(height) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE
    constexpr auto extent<T, 2>::to(const std::source_location& location) const noexcept -> extent<U, 2> {
        return { .width = as<U>(width, location), .height = as<U>(height, location) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<usize NEW_RANK>
    STORMKIT_PURE
    constexpr auto extent<T, 2>::to() const noexcept -> extent<T, NEW_RANK> {
        using Out        = extent<ValueType, NEW_RANK>;
        using Array      = array<ValueType, RANK>;
        using OtherArray = array<typename Out::ValueType, Out::RANK>;

        auto out = Out {};

        auto& values       = *std::bit_cast<Array*>(this);
        auto& other_values = *std::bit_cast<OtherArray*>(&out);

        if constexpr (NEW_RANK > RANK) {
            stdr::copy(values, stdr::begin(other_values));
        } else
            stdr::copy_n(stdr::begin(values), NEW_RANK, stdr::begin(other_values));

        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE
    constexpr auto extent<T, 3>::narrow_to() const noexcept -> extent<U, 3> {
        return { .width = narrow<U>(width), .height = narrow<U>(height), .depth = narrow<U>(depth) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<core::meta::IsArithmetic U>
    STORMKIT_PURE
    constexpr auto extent<T, 3>::to(const std::source_location& location) const noexcept -> extent<U, 3> {
        return { .width = as<U>(width, location), .height = as<U>(height, location), .depth = as<U>(depth, location) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T>
    template<usize NEW_RANK>
    STORMKIT_PURE
    constexpr auto extent<T, 3>::to() const noexcept -> extent<T, NEW_RANK> {
        using Out        = extent<ValueType, NEW_RANK>;
        using Array      = array<ValueType, RANK>;
        using OtherArray = array<typename Out::ValueType, Out::RANK>;

        auto out = Out {};

        auto& values       = *std::bit_cast<Array*>(this);
        auto& other_values = *std::bit_cast<OtherArray*>(&out);

        if constexpr (NEW_RANK > RANK) {
            stdr::copy(values, stdr::begin(other_values));
        } else
            stdr::copy_n(stdr::begin(values), NEW_RANK, stdr::begin(other_values));

        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsExtent Extent>
    STORMKIT_PURE
    constexpr auto operator<=>(const Extent& first, const Extent& second) noexcept -> typename Extent::OrderingType {
        using Array                = array<typename Extent::ValueType, Extent::RANK>;
        using OrderingType         = typename Extent::OrderingType;
        static constexpr auto RANK = Extent::RANK;

        const auto& values       = *std::bit_cast<Array*>(&first);
        const auto& other_values = *std::bit_cast<Array*>(&second);

        for (auto i = 0uz; i < RANK; ++i)
            if (auto ret = values[i] <=> other_values[i]; ret != 0) return ret;

        return OrderingType::equivalent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsExtent Extent>
    STORMKIT_PURE
    constexpr auto operator==(const Extent& first, const Extent& second) noexcept -> bool {
        using Array                = array<typename Extent::ValueType, Extent::RANK>;
        static constexpr auto RANK = Extent::RANK;

        const auto& values       = *std::bit_cast<Array*>(&first);
        const auto& other_values = *std::bit_cast<Array*>(&second);

        for (auto i = 0uz; i < RANK; ++i)
            if (values[i] != other_values[i]) return false;

        return true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsExtent Extent>
    STORMKIT_PURE
    constexpr auto operator*(Extent&& extent, typename Extent::ElemenType factor) noexcept -> core::meta::CanonicalType<Extent> {
        return core::meta::CanonicalType<Extent> { std::forward<Extent>(extent) } *= factor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsExtent Extent>
    STORMKIT_PURE
    constexpr auto operator/(Extent&& extent, typename Extent::ElemenType factor) noexcept -> core::meta::CanonicalType<Extent> {
        return core::meta::CanonicalType<Extent> { std::forward<Extent>(extent) } /= factor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsExtent Extent>
    STORMKIT_PURE
    constexpr auto operator*=(Extent& extent, typename Extent::ValueType factor) noexcept -> Extent& {
        using ValueType              = typename Extent::ValueType;
        static constexpr auto RANK   = Extent::RANK;
        auto&                 values = *std::bit_cast<array<ValueType, RANK>>(&extent);
        for (auto&& val : values) val *= factor;
        return extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsExtent Extent>
    STORMKIT_PURE
    constexpr auto operator/=(Extent& extent, typename Extent::ValueType factor) noexcept -> Extent& {
        using ValueType              = typename Extent::ValueType;
        static constexpr auto RANK   = Extent::RANK;
        auto&                 values = *std::bit_cast<array<ValueType, RANK>>(&extent);
        for (auto&& val : values) val /= factor;
        return extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stormkit::math::meta::IsExtent2 Extent>
    STORMKIT_FORCE_INLINE
    inline auto to_string(const Extent& extent) noexcept -> string {
        return std::format("{}", extent);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stormkit::math::meta::IsExtent3 Extent>
    STORMKIT_FORCE_INLINE
    inline auto to_string(const Extent& extent) noexcept -> string {
        return std::format("{}", extent);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const extent<T, 2>& extent, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[extent2 width: {}, height: {}]", extent.width, extent.height);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::IsArithmetic T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const extent<T, 3>& extent, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[extent3 width: {}, height: {}, depth: {}]", extent.width, extent.height, extent.depth);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret, core::meta::IsArithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const extent<T, 2>& extent) noexcept -> Ret {
        return hash<Ret>(extent.width, extent.height);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret, core::meta::IsArithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const extent<T, 3>& extent) noexcept -> Ret {
        return hash<Ret>(extent.width, extent.height, extent.depth);
    }
}}} // namespace stormkit::core::math

static_assert(sizeof(math::uextent2) == sizeof(array<u32, 2>));
static_assert(sizeof(math::uextent3) == sizeof(array<u32, 3>));

static_assert(sizeof(math::iextent2) == sizeof(array<i32, 2>));
static_assert(sizeof(math::iextent3) == sizeof(array<i32, 3>));

static_assert(sizeof(math::extent2<u16>) == sizeof(array<u16, 2>));
static_assert(sizeof(math::extent3<u16>) == sizeof(array<u16, 3>));

static_assert(sizeof(math::extent2<i16>) == sizeof(array<i16, 2>));
static_assert(sizeof(math::extent3<i16>) == sizeof(array<i16, 3>));

static_assert(sizeof(math::fextent2) == sizeof(array<f32, 2>));
static_assert(sizeof(math::fextent3) == sizeof(array<f32, 3>));

static_assert(math::fextent2::RANK == 2);
static_assert(math::fextent3::RANK == 3);
static_assert(math::fextent3::RANK != 2);
static_assert(math::fextent2::RANK != 3);

static_assert(math::meta::IsExtent<math::fextent2>);
static_assert(math::meta::IsExtent<math::fextent3>);
static_assert(math::meta::IsExtent2<math::fextent2>);
static_assert(not math::meta::IsExtent2<math::fextent3>);
static_assert(math::meta::IsExtent3<math::fextent3>);
static_assert(not math::meta::IsExtent3<math::fextent2>);
