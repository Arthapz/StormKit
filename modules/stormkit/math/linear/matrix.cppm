// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.math.linear.matrix;

import std;

import stormkit.core.meta;
import stormkit.core.types;
import stormkit.core.typesafe;
import stormkit.core.ranges;
import stormkit.core.contract;
import stormkit.core.functional;
import stormkit.core.string.format;

import stormkit.math.arithmetic;
import stormkit.math.linear.vector;
import stormkit.math.linear.angle;

namespace cmeta = stormkit::core::meta;
namespace stdr  = std::ranges;
namespace stdv  = std::views;

using namespace std::literals;
using namespace stormkit::core::literals;

export namespace stormkit::math {
    // M => rows
    // N => columns
    template<cmeta::arithmetic T, usize M, usize N>
    struct STORMKIT_TRIVIALLY_RELOCATABLE alignas(array<T, M * N>) mat {
        using value_type = T;
        using size_type  = usize;

        using row_type = vec<T, N>;
        using col_type = vec<T, M>;

        using storage_type = array<row_type, M>;

        using reference       = T&;
        using const_reference = const T&;
        template<bool Const>
        using conditional_reference = cmeta::conditional<Const, const_reference, reference>;

        constexpr mat() noexcept;

        template<cmeta::is_as_castable_to<T>... Ts>
        constexpr explicit(M == N and M == 1) mat(Ts&&... values) noexcept
            requires(sizeof...(Ts) == M * N);

        constexpr explicit mat(array<row_type, M> rows_) noexcept;

        storage_type rows = {};

        template<typename Self>
        [[nodiscard]]
        constexpr auto operator[](this Self& self STORMKIT_LIFETIMEBOUND, size_type i) noexcept
          -> cmeta::forward_const_to<Self, row_type>&;

        template<typename Self>
        [[nodiscard]]
        constexpr auto operator[](this Self& self STORMKIT_LIFETIMEBOUND, size_type i, size_type j) noexcept
          -> conditional_reference<cmeta::const_qualified<Self>>;

        template<typename Self>
        [[nodiscard]]
        constexpr auto row(STORMKIT_LIFETIMEBOUND this Self& self, usize id) noexcept -> cmeta::forward_const_to<Self, row_type>&;

        template<typename Self>
        [[nodiscard]]
        constexpr auto col(usize id) const noexcept -> col_type;

        template<typename Self>
        [[nodiscard]]
        constexpr explicit operator array_view<cmeta::forward_const_to<Self, value_type>, M* N>(this Self& self
                                                                                                  STORMKIT_LIFETIMEBOUND);

        static constexpr auto row_extent() noexcept -> size_type;
        static constexpr auto col_extent() noexcept -> size_type;

        [[nodiscard]]
        static constexpr auto identity() noexcept -> decltype(auto)
            requires(M == N);
    };

    static_assert(alignof(mat<u8, 4, 2>) == alignof(array<u8, 4 * 2>));
    static_assert(sizeof(mat<u8, 4, 2>) == sizeof(array<u8, 4 * 2>));
    static_assert(cmeta::trivially_relocatable<mat<u8, 4, 2>>);
    static_assert(alignof(mat<i16, 4, 2>) == alignof(array<i16, 4 * 2>));
    static_assert(sizeof(mat<i16, 4, 2>) == sizeof(array<i16, 4 * 2>));
    static_assert(cmeta::trivially_relocatable<mat<i16, 4, 2>>);
    static_assert(alignof(mat<i32, 4, 2>) == alignof(array<i32, 4 * 2>));
    static_assert(sizeof(mat<i32, 4, 2>) == sizeof(array<i32, 4 * 2>));
    static_assert(cmeta::trivially_relocatable<mat<i32, 4, 2>>);
    static_assert(alignof(mat<f32, 4, 2>) == alignof(array<f32, 4 * 2>));
    static_assert(sizeof(mat<f32, 4, 2>) == sizeof(array<f32, 4 * 2>));
    static_assert(cmeta::trivially_relocatable<mat<f32, 4, 2>>);

    static_assert(alignof(mat<u8, 3, 3>) == alignof(array<u8, 3 * 3>));
    static_assert(sizeof(mat<u8, 3, 3>) == sizeof(array<u8, 3 * 3>));
    static_assert(cmeta::trivially_relocatable<mat<u8, 3, 3>>);
    static_assert(alignof(mat<i16, 3, 3>) == alignof(array<i16, 3 * 3>));
    static_assert(sizeof(mat<i16, 3, 3>) == sizeof(array<i16, 3 * 3>));
    static_assert(cmeta::trivially_relocatable<mat<i16, 3, 3>>);
    static_assert(alignof(mat<i32, 3, 3>) == alignof(array<i32, 3 * 3>));
    static_assert(sizeof(mat<i32, 3, 3>) == sizeof(array<i32, 3 * 3>));
    static_assert(cmeta::trivially_relocatable<mat<i32, 3, 3>>);
    static_assert(alignof(mat<f32, 3, 3>) == alignof(array<f32, 3 * 3>));
    static_assert(sizeof(mat<f32, 3, 3>) == sizeof(array<f32, 3 * 3>));
    static_assert(cmeta::trivially_relocatable<mat<f32, 3, 3>>);

    template<typename T>
    using mat2x2 = mat<T, 2, 2>;
    template<typename T>
    using mat3x3 = mat<T, 3, 3>;
    template<typename T>
    using mat4x4 = mat<T, 4, 4>;

    template<typename T>
    using mat2x3 = mat<T, 2, 3>;
    template<typename T>
    using mat2x4 = mat<T, 2, 4>;

    template<typename T>
    using mat3x2 = mat<T, 3, 2>;
    template<typename T>
    using mat3x4 = mat<T, 3, 4>;

    template<typename T>
    using mat4x3 = mat<T, 4, 3>;
    template<typename T>
    using mat4x2 = mat<T, 4, 2>;

    using fmat2 = mat2x2<f32>;
    using imat2 = mat2x2<i32>;
    using umat2 = mat2x2<u32>;

    using fmat3 = mat3x3<f32>;
    using imat3 = mat3x3<i32>;
    using umat3 = mat3x3<u32>;

    using fmat4 = mat4x4<f32>;
    using imat4 = mat4x4<i32>;
    using umat4 = mat4x4<u32>;

    using fmat2x3 = mat2x3<f32>;
    using imat2x3 = mat2x3<i32>;
    using umat2x3 = mat2x3<u32>;

    using fmat3x2 = mat3x2<f32>;
    using imat3x2 = mat3x2<i32>;
    using umat3x2 = mat3x2<u32>;

    using fmat4x3 = mat4x3<f32>;
    using imat4x3 = mat4x3<i32>;
    using umat4x3 = mat4x3<u32>;

    using fmat3x4 = mat3x4<f32>;
    using imat3x4 = mat3x4<i32>;
    using umat3x4 = mat3x4<u32>;

    using fmat4x2 = mat4x2<f32>;
    using imat4x2 = mat4x2<i32>;
    using umat4x2 = mat4x2<u32>;

    using fmat2x4 = mat2x4<f32>;

    using imat2x4 = mat2x4<i32>;
    using umat2x4 = mat2x4<u32>;

    namespace meta {
        template<typename T>
        concept is_mat = cmeta::specialization_of_nttp_tv<T, mat>;

        template<typename T>
        concept is_square_mat = is_mat<T> and (T::row_extent() == T::col_extent());

        template<typename T, usize N>
        concept is_mat_n = is_square_mat<T> and T::row_extent() == N;

        template<typename T>
        concept is_mat_2 = is_mat_n<T, 2>;

        template<typename T>
        concept is_mat_3 = is_mat_n<T, 3>;

        template<typename T>
        concept is_mat_4 = is_mat_n<T, 4>;

        namespace plain {
            template<typename T>
            concept is_mat = meta::is_mat<cmeta::to_plain_type<T>>;

            template<typename T>
            concept is_square_mat = meta::is_square_mat<cmeta::to_plain_type<T>>;

            template<typename T, usize N>
            concept is_mat_n = meta::is_mat_n<cmeta::to_plain_type<T>, N>;

            template<typename T>
            concept is_mat_2 = meta::is_mat_2<cmeta::to_plain_type<T>>;

            template<typename T>
            concept is_mat_3 = meta::is_mat_3<cmeta::to_plain_type<T>>;

            template<typename T>
            concept is_mat_4 = meta::is_mat_4<cmeta::to_plain_type<T>>;
        } // namespace plain
    } // namespace meta

    template<typename CharT, cmeta::arithmetic T, usize M, usize N>
    constexpr auto tag_invoke(format_as_fn<CharT>, const mat<T, M, N>& matrix, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out());

    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto tag_invoke(cmeta::tag<mutable_view_of>, mat<T, M, N>& mat STORMKIT_LIFETIMEBOUND) noexcept
      -> array_view<T, M * N>;

    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto tag_invoke(cmeta::tag<view_of>, const mat<T, M, N>& mat STORMKIT_LIFETIMEBOUND) noexcept
      -> array_view<const T, M * N>;

    template<cmeta::arithmetic T, usize M>
    [[nodiscard]]
    constexpr auto determinant(const mat<T, M, M>& matrix) noexcept -> T;

    template<cmeta::arithmetic T, usize M, usize N>
    [[nodiscard]]
    constexpr auto transpose(const mat<T, M, N>& matrix) noexcept -> mat<T, N, M>;

    template<cmeta::arithmetic T, usize M>
    [[nodiscard]]
    constexpr auto cofactor(const mat<T, M, M>& matrix) noexcept -> mat<T, M, M>;

    template<cmeta::arithmetic T, usize M, usize N>
    [[nodiscard]]
    constexpr auto is_inversible(const mat<T, M, N>& matrix) noexcept -> bool;

    template<cmeta::arithmetic T, usize M>
    [[nodiscard]]
    constexpr auto inverse(const mat<T, M, M>& matrix) noexcept -> mat<T, M, M>;

    template<cmeta::arithmetic T, usize M, usize N>
    [[nodiscard]]
    constexpr auto is_orthogonal(const mat<T, M, N>& matrix) noexcept -> bool;

    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto add(const mat<T, M, N>& a, const mat<T, M, N>& b) noexcept -> mat<T, M, N>;

    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto sub(const mat<T, M, N>& a, const mat<T, M, N>& b) noexcept -> mat<T, M, N>;

    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto mul(const mat<T, M, N>& a, T b) noexcept -> mat<T, M, N>;

    template<cmeta::arithmetic T, usize M, usize N, usize K>
    [[nodiscard]]
    constexpr auto mul(const mat<T, M, N>& a, const mat<T, N, K>& b) noexcept -> mat<T, M, K>;

    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto div(const mat<T, M, N>& a, T b) noexcept -> mat<T, M, N>;

    template<cmeta::arithmetic T, usize M, usize N>
    [[nodiscard]]
    constexpr auto div(const mat<T, M, N>& a, const mat<T, N, N>& b) noexcept -> mat<T, N, N>;

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto translate(const mat<T, 4, 4>& matrix, const vec<T, 3>& translation) noexcept -> mat<T, 4, 4>;

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto scale(const mat<T, 4, 4>& matrix, const vec<T, 3>& translation) noexcept -> mat<T, 4, 4>;

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto rotate(const mat<T, 4, 4>& matrix, T angle, const vec<T, 3>& axis) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>);

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto orthographique(T left, T right, T bottom, T top, T near, T far) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>);

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto orthographique(T left, T right, T bottom, T top) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>);

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto perspective(radian<T> fov_y, T aspect, T near, T far) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>);

    template<cmeta::arithmetic T>
    [[nodiscard]]
    constexpr auto look_at(const vec<T, 3>& eye, const vec<T, 3>& center, const vec<T, 3>& up) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>);
} // namespace stormkit::math

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::math {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE
    constexpr mat<T, M, N>::mat() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    template<cmeta::is_as_castable_to<T>... Ts>
    constexpr mat<T, M, N>::mat(Ts&&... values) noexcept
        requires(sizeof...(Ts) == M * N)
    {
        auto view = mutable_view_of(*this);
        stdr::copy(array<T, M * N> { as<T>(values)... }, stdr::begin(view));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr mat<T, M, N>::mat(array<row_type, M> rows_) noexcept {
        stdr::copy(std::move(rows_), stdr::begin(rows));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto mat<T, M, N>::operator[](this Self& self STORMKIT_LIFETIMEBOUND, size_type i) noexcept
      -> cmeta::forward_const_to<Self, row_type>& {
        return self.row(i);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto mat<T, M, N>::operator[](this Self& self STORMKIT_LIFETIMEBOUND, size_type i, size_type j) noexcept
      -> conditional_reference<cmeta::const_qualified<Self>> {
        return self.row(i)[j];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto mat<T, M, N>::identity() noexcept -> decltype(auto)
        requires(M == N)
    {
        auto matrix = mat<T, M, N> {};
        for (auto i : range(M)) matrix[i, i] = T { 1 };
        return matrix;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto mat<T, M, N>::row(this Self& self, usize id) noexcept -> cmeta::forward_const_to<Self, row_type>& {
        expects(id < M);
        return self.rows[id];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    template<typename Self>
    constexpr auto mat<T, M, N>::col(usize id) const noexcept -> col_type {
        expects(id < N);
        auto out = col_type {};
        for (auto i : range(M)) out[i] = rows[i][id];
        return out;
    }

    template<cmeta::arithmetic T, usize M, usize N>
    template<typename Self>
    constexpr mat<T, M, N>::operator array_view<cmeta::forward_const_to<Self, value_type>, M* N>(this Self& self
                                                                                                   STORMKIT_LIFETIMEBOUND) {
        return array_view<cmeta::forward_const_to<Self, value_type>, M * N> {
            std::bit_cast<cmeta::forward_const_to<Self, T>*>(&self.rows),
            M * N
        };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto mat<T, M, N>::row_extent() noexcept -> size_type {
        return M;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto mat<T, M, N>::col_extent() noexcept -> size_type {
        return N;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename CharT, cmeta::arithmetic T, usize M, usize N>
    constexpr auto tag_invoke(format_as_fn<CharT>, const mat<T, M, N>& matrix, cmeta::format_context auto& ctx) noexcept
      -> decltype(ctx.out()) {
        auto i               = 0;
        auto chunk_by_extent = [&i](auto, auto) mutable {
            const auto insert = i++ < (N - 1);
            if (not insert) i = 0;

            return insert;
        };
        format_to(ctx, "[mat");
        format_to(ctx, "{}", M);
        format_to(ctx, "x");
        format_to(ctx, "{}", N);
        format_to(ctx, ":");

        auto       l                = 0_u32;
        auto       max_digit        = 0_u32;
        const auto matrix_as_linear = view_of(matrix);
        for (auto v : matrix_as_linear)
            max_digit = std::max(max_digit, unchecked_narrow<i64>(v) == 0 ? 2_u32 : unchecked_narrow<u32>(std::log10(v) + 2));

        for (auto&& slice : matrix_as_linear | stdv::chunk_by(chunk_by_extent)) {
            if constexpr (cmeta::integral<T>) {
                format_to(ctx,
                          "{}|{:n:>{}}|{}",
                          (l > 0) ? "         "sv : " "sv,
                          slice,
                          max_digit,
                          (l != (M - 1)) ? "\n"sv : "]"sv);
            } else {
                format_to(ctx,
                          "{}| {:n:>{}.5f}|{}",
                          (l > 0) ? "         "sv : " "sv,
                          slice,
                          max_digit,
                          (l != (M - 1)) ? "\n"sv : "]"sv);
            }
            ++l;
        }

        return ctx.out();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(cmeta::tag<mutable_view_of>, mat<T, M, N>& mat STORMKIT_LIFETIMEBOUND) noexcept
      -> array_view<T, M * N> {
        return static_cast<array_view<T, M * N>>(mat);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(cmeta::tag<view_of>, const mat<T, M, N>& mat STORMKIT_LIFETIMEBOUND) noexcept
      -> array_view<const T, M * N> {
        return static_cast<array_view<const T, M * N>>(mat);
    }

    // TODO rewrite without recursivity
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M>
    constexpr auto determinant(const mat<T, M, M>& matrix) noexcept -> T {
        if constexpr (M == 0) return 1;
        else if constexpr (M == 1)
            return matrix[0, 0];
        else if constexpr (M == 2)
            return matrix[0, 0] * matrix[1, 1] - matrix[0, 1] * matrix[1, 0];
        else if constexpr (M == 3)
            return matrix[0, 0] * (matrix[1, 1] * matrix[2, 2] - matrix[2, 1] * matrix[1, 2])
                   - matrix[1, 0] * (matrix[0, 1] * matrix[2, 2] - matrix[2, 1] * matrix[0, 2])
                   + matrix[2, 0] * (matrix[0, 1] * matrix[1, 2] - matrix[1, 1] * matrix[0, 2]);
        else if constexpr (M == 4) {
            const auto subfactor_0 = matrix[2, 2] * matrix[3, 3] - matrix[3, 2] * matrix[2, 3];
            const auto subfactor_1 = matrix[2, 1] * matrix[3, 3] - matrix[3, 1] * matrix[2, 3];
            const auto subfactor_2 = matrix[2, 1] * matrix[3, 2] - matrix[3, 1] * matrix[2, 2];
            const auto subfactor_3 = matrix[2, 0] * matrix[3, 3] - matrix[3, 0] * matrix[2, 3];
            const auto subfactor_4 = matrix[2, 0] * matrix[3, 2] - matrix[3, 0] * matrix[2, 2];
            const auto subfactor_5 = matrix[2, 0] * matrix[3, 1] - matrix[3, 0] * matrix[2, 1];

            const auto det_cof_0 = matrix[1, 1] * subfactor_0 - matrix[1, 2] * subfactor_1 + matrix[1, 3] * subfactor_2;
            const auto det_cof_1 = -(matrix[1, 0] * subfactor_0 - matrix[1, 2] * subfactor_3 + matrix[1, 3] * subfactor_4);
            const auto det_cof_2 = matrix[1, 0] * subfactor_1 - matrix[1, 1] * subfactor_3 + matrix[1, 3] * subfactor_5;
            const auto det_cof_3 = -(matrix[1, 0] * subfactor_2 - matrix[1, 1] * subfactor_4 + matrix[1, 2] * subfactor_5);

            return matrix[0, 0] * det_cof_0 + matrix[0, 1] * det_cof_1 + matrix[0, 2] * det_cof_2 + matrix[0, 3] * det_cof_3;
        } else {
            auto matrix_ = mat<T, M, M> {};
            stdr::copy(view_of(matrix), stdr::begin(mutable_view_of(matrix_)));

            static constexpr auto swap = [](auto& row, auto& col) static noexcept {
                const auto old_row = auto(row);
                stdr::copy(view_of(col), stdr::begin(mutable_view_of(row)));
                stdr::copy(view_of(old_row), stdr::begin(mutable_view_of(col)));
            };

            for (auto col : range(M)) {
                const auto found = init_by<bool>([&matrix_, col](auto& found) noexcept {
                    found = false;
                    for (auto row : range(col, M)) {
                        if (not is(matrix_[row, col], T { 0 })) {
                            found = true;
                            if (row != col) { swap(matrix_.row(row), matrix_.row(col)); }
                        }
                    }
                });
                expects(found, "Did not found a non-zero row!");

                auto i = 0;
                for (auto row : range(col + 1, M)) {
                    for (;;) {
                        const auto del = matrix_[row, col] / matrix_[col, col];
                        for (auto j : range(col, M)) matrix_[row, j] -= del * matrix_[col, j];

                        if (is(matrix_[row, col], T { 0 })) break;

                        swap(matrix_.row(row), matrix_.row(col));
                    }
                }
            }

            const auto res = init_by<T>([&matrix_](auto& res) noexcept {
                res = -1;
                for (auto i : range(M)) res *= matrix_[i, i];
            });

            return res;
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto transpose(const mat<T, M, N>& matrix) noexcept -> mat<T, N, M> {
        auto out = mat<T, N, M> {};
        for (auto [i, j] : multi_range(N, M)) out[i, j] = matrix[j, i];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M>
    STORMKIT_FORCE_INLINE
    constexpr auto cofactor(const mat<T, M, M>& matrix) noexcept -> mat<T, M, M> {
        static constexpr auto N = M - 1u;

        auto out = mat<T, M, M> {};

        for (auto [i, j] : multi_range(M, M)) {
            const auto submatrix = init_by<mat<T, N, N>>([i, j, &matrix](auto& out) noexcept {
                auto out_view = mutable_view_of(out);
                auto c        = 0u;
                for (auto [k, l] : multi_range(M, M)) {
                    if (k == i or l == j) continue;

                    out_view[c] = matrix[k, l];

                    ++c;
                }
            });

            out[i, j] = unchecked_narrow<T>(std::pow(-1, i + j)) * determinant(submatrix);
        }

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto is_inversible(const mat<T, M, N>& matrix) noexcept -> bool {
        if constexpr (M != N or M == 0) return false;
        else {
            const auto d = determinant(matrix);
            return d != 0;
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M>
    STORMKIT_FORCE_INLINE
    constexpr auto inverse(const mat<T, M, M>& matrix) noexcept -> mat<T, M, M> {
        auto out = mat<T, M, M> {};
        EXPECTS(is_inversible(matrix));

        if constexpr (M == 2) {
            const auto one_over_determinant = T { 1 } / determinant(matrix);

            out[0, 0] = matrix[1, 1] * one_over_determinant;
            out[0, 1] = -matrix[0, 1] * one_over_determinant;
            out[1, 0] = -matrix[1, 0] * one_over_determinant;
            out[1, 1] = matrix[0, 0] * one_over_determinant;
        } else if constexpr (M == 3) {
            const auto a = vec4<T> { matrix[0, 0], matrix[0, 1], matrix[0, 2], T { 0 } };
            const auto b = vec4<T> { matrix[1, 0], matrix[1, 1], matrix[1, 2], T { 0 } };
            const auto c = vec4<T> { matrix[2, 0], matrix[2, 1], matrix[2, 2], T { 0 } };

            const auto i0 = cross(b, c);
            const auto i1 = cross(c, a);
            const auto i2 = cross(a, b);

            const auto inverse = transpose([&i0, &i1, &i2] noexcept {
                auto inverse = mat<T, M, M> {};
                auto row_0   = mutable_view_of(inverse.row(0));
                auto row_1   = mutable_view_of(inverse.row(1));
                auto row_2   = mutable_view_of(inverse.row(2));

                stdr::copy(array { i0[0], i0[1], i0[2] }, stdr::begin(row_0));
                stdr::copy(array { i1[0], i1[1], i1[2] }, stdr::begin(row_1));
                stdr::copy(array { i2[0], i2[1], i2[2] }, stdr::begin(row_2));

                return inverse;
            }());

            const auto one_over_determinant = T { 1 } / determinant(matrix);
            out                             = mul(inverse, one_over_determinant);
            // } else if constexpr (N == 4) {
            // TODO
        } else {
            const auto one_over_determinant = T { 1 } / determinant(matrix);
            const auto transposed           = transpose(cofactor(matrix));

            out = mul(transposed, one_over_determinant);
        }
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto is_orthogonal(const mat<T, M, N>& matrix) noexcept -> bool {
        if constexpr (M != N) return false;
        else {
            if (not is_inversible(matrix)) return false;

            const auto transposed    = transpose(matrix);
            const auto result        = mul(matrix, transposed);
            const auto result_view   = view_of(result);
            const auto identity_view = view_of(mat<T, M, N>::identity());

            if consteval {
                for (auto i : range(stdr::size(result_view)))
                    if (result_view[i] != identity_view[i]) return false;

                return true;
            } else {
                return std::memcmp(stdr::data(result_view), stdr::data(identity_view), M * N * sizeof(T)) == 0;
            }
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto add(const mat<T, M, N>& a, const mat<T, M, N>& b) noexcept -> mat<T, M, N> {
        auto       out      = mat<T, M, N> {};
        const auto a_view   = view_of(a);
        const auto b_view   = view_of(b);
        auto       out_view = mutable_view_of(out);
        for (auto i : range(M * N)) out_view[i] = a_view[i] + b;
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto sub(const mat<T, M, N>& a, const mat<T, M, N>& b) noexcept -> mat<T, M, N> {
        auto       out      = mat<T, M, N> {};
        const auto a_view   = view_of(a);
        const auto b_view   = view_of(b);
        auto       out_view = mutable_view_of(out);
        for (auto i : range(M * N)) out_view[i] = a_view[i] - b;
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto mul(const mat<T, M, N>& a, T b) noexcept -> mat<T, M, N> {
        auto       out      = mat<T, M, N> {};
        const auto a_view   = view_of(a);
        auto       out_view = mutable_view_of(out);
        for (auto i : range(M * N)) out_view[i] = a_view[i] * b;
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N, usize K>
    STORMKIT_FORCE_INLINE
    constexpr auto mul(const mat<T, M, N>& a, const mat<T, N, K>& b) noexcept -> mat<T, M, K> {
        auto out = mat<T, M, K> {};
        for (auto [i, j, k] : multi_range(M, K, N)) out[i, j] += a[i, k] * b[k, j];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    constexpr auto div(const mat<T, M, N>& a, T b) noexcept -> mat<T, M, N> {
        EXPECTS(not is(b, T { 0 }));

        auto       out      = mat<T, M, N> {};
        const auto a_view   = view_of(a);
        auto       out_view = mutable_view_of(out);
        for (auto i : range(M * N)) out_view[i] = a_view[i] / b;
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T, usize M, usize N>
    STORMKIT_FORCE_INLINE
    constexpr auto div(const mat<T, M, N>& a, const mat<T, N, N>& b) noexcept -> mat<T, N, N> {
        auto       out = mat<T, N, N> {};
        const auto d   = determinant(b);
        EXPECTS(not is(d, 0));

        const auto b_inverted = inverse(b);
        return mul(a, b_inverted);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto translate(const mat<T, 4, 4>& matrix, const vec<T, 3>& translation) noexcept -> mat<T, 4, 4> {
        auto out      = mat<T, 4, 4> {};
        auto out_view = mutable_view_of(out);
        stdr::copy(view_of(matrix), stdr::begin(out_view));

        out[0, 3] = matrix[0, 3] + translation[0];
        out[1, 3] = matrix[1, 3] + translation[1];
        out[2, 3] = matrix[2, 3] + translation[2];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto scale(const mat<T, 4, 4>& matrix, const vec<T, 3>& scale_factor) noexcept -> mat<T, 4, 4> {
        auto out  = mat<T, 4, 4> {};
        out[0, 0] = matrix[0, 0] * scale_factor[0];
        out[1, 0] = matrix[1, 0] * scale_factor[0];
        out[2, 0] = matrix[2, 0] * scale_factor[0];
        out[3, 0] = matrix[3, 0] * scale_factor[0];

        out[0, 1] = matrix[0, 1] * scale_factor[1];
        out[1, 1] = matrix[1, 1] * scale_factor[1];
        out[2, 1] = matrix[2, 1] * scale_factor[1];
        out[3, 1] = matrix[3, 1] * scale_factor[1];

        out[0, 2] = matrix[0, 2] * scale_factor[2];
        out[1, 2] = matrix[1, 2] * scale_factor[2];
        out[2, 2] = matrix[2, 2] * scale_factor[2];
        out[3, 2] = matrix[3, 2] * scale_factor[2];

        out[0, 3] = matrix[0, 3];
        out[1, 3] = matrix[1, 3];
        out[2, 3] = matrix[2, 3];
        out[3, 3] = matrix[3, 3];
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    STORMKIT_FORCE_INLINE
    constexpr auto rotate(const mat<T, 4, 4>& matrix, radian<T> angle, const vec<T, 3>& axis) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>)
    {
        auto       out = mat<T, 4, 4> {};
        const auto cos = unchecked_narrow<T>(std::cos(angle.get()));
        const auto sin = unchecked_narrow<T>(std::sin(angle.get()));

        const auto axis_norm = normalize(axis);
        const auto temp      = nomarlize(mul(axis_norm, T { 1 } - cos));

        const auto rotation_matrix = init_by<mat<T, 4, 4>>([&](auto& rotation_matrix) noexcept {
            rotation_matrix[0, 0] = cos + temp[0] * axis_norm[0];
            rotation_matrix[1, 0] = temp[0] * axis_norm[1] + sin * axis_norm[2];
            rotation_matrix[2, 0] = temp[0] * axis_norm[2] - sin * axis_norm[1];

            rotation_matrix[0, 1] = temp[1] * axis_norm[0] - sin * axis_norm[2];
            rotation_matrix[1, 1] = cos + temp[1] * axis_norm[1];
            rotation_matrix[2, 1] = temp[1] * axis_norm[2] + sin * axis_norm[0];

            rotation_matrix[0, 2] = temp[2] * axis_norm[0] + sin * axis_norm[1];
            rotation_matrix[1, 2] = temp[2] * axis_norm[1] - sin * axis_norm[0];
            rotation_matrix[2, 2] = cos + temp[2] * axis_norm[2];
        });

        for (auto [i, j, k] : multi_range(3_usize, 3_usize, 3_usize)) out[j, i] += matrix[k, i] * rotation_matrix[j, k];

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
       STORMKIT_FORCE_INLINE
    constexpr auto orthographique(T left, T right, T bottom, T top) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>)
    {
        static constexpr auto far  = core::as<T>(100);
        static constexpr auto near = core::unchecked_narrow<T>(0.1);

        return orthographique(left, right, bottom, top, near, far);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
       STORMKIT_FORCE_INLINE
    constexpr auto orthographique(T left, T right, T bottom, T top, T near, T far) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>)
    {
        auto out  = mat<T, 4, 4> {};
        out[0, 0] = core::as<T>(2) / (right - left);
        out[1, 1] = core::as<T>(2) / (top - bottom);
        out[2, 2] = core::as<T>(1) / (far - near);

        out[0, 3] = -(right + left) / (right - left);
        out[1, 3] = -(top + bottom) / (top - bottom);
        out[2, 3] = -near / (far - near);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    constexpr auto perspective(radian<T> fov_y, T aspect, T near, T far) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>)
    {
        auto out = mat<T, 4, 4> {};
        EXPECTS(not is(aspect, T { 0 }));
        EXPECTS(not is(near, far));

        const auto half_fov_y = unchecked_narrow<T>(std::tan(fov_y.get() / T { 2 }));

        stdr::fill(mutable_view_of(out), T { 0 });
        out[0, 0] = T { 1 } / (aspect * half_fov_y);
        out[1, 1] = T { -1 } / half_fov_y;
        out[2, 2] = far / (far - near);
        out[2, 3] = T { 1 };
        out[3, 2] = -(far * near) / (far - near);
        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<cmeta::arithmetic T>
    constexpr auto look_at(const vec3<T>& eye, const vec3<T>& center, const vec3<T>& up) noexcept -> mat<T, 4, 4>
        requires(std::is_signed_v<T>)
    {
        auto       out = mat<T, 4, 4> {};
        const auto z   = normalize(sub(center, eye));
        const auto x   = normalize(cross(up, z));
        const auto y   = normalize(cross(z, x));

        out[0, 0] = x[0];
        out[0, 1] = y[0];
        out[0, 2] = z[0];
        out[0, 3] = 0;

        out[1, 0] = x[1];
        out[1, 1] = y[1];
        out[1, 2] = z[1];
        out[1, 3] = 0;

        out[2, 0] = x[2];
        out[2, 1] = y[2];
        out[2, 2] = z[2];
        out[2, 3] = 0;

        out[3, 0] = -dot(x, eye);
        out[3, 1] = -dot(y, eye);
        out[3, 2] = -dot(z, eye);
        out[3, 3] = 1;

        return out;
    }
} // namespace stormkit::math
