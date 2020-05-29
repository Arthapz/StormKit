// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Color.hpp
/// \author Arthapz

#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <type_traits>

#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Numerics.hpp>

namespace storm::core {
    /**
     * @struct RGBColor
     * @ingroup Core
     * @brief Color class
     *
     * Utility class for manipulating RGBA colors
     */

    template<typename T>
    constexpr inline T maxColorComponentValue() noexcept {
        if constexpr (std::is_same_v<T, float>) return 1.f;
        else if constexpr (std::is_same_v<T, core::UInt8>)
            return 255u;

        return T { 0 };
    }

    template<typename T, typename U>
    constexpr inline T color_component_type_conversion(U component) noexcept;

    template<>
    constexpr inline float color_component_type_conversion<float, Int8>(Int8 component) noexcept {
        return static_cast<float>(component) / 255.f;
    }

    template<>
    constexpr inline core::UInt8
        color_component_type_conversion<core::UInt8, float>(float component) noexcept {
        return static_cast<core::UInt8>(component * 255.f);
    }

    template<typename T>
    struct RGBColor {
        constexpr RGBColor(T _red,
                           T _green,
                           T _blue,
                           T _alpha = maxColorComponentValue<T>()) noexcept
            : r { _red }, g { _green }, b { _blue }, a { _alpha } {}

        template<typename U>
        constexpr RGBColor(RGBColor<U> color) noexcept
            : r { color_component_type_conversion<T, U>(color.r) },
              g { color_component_type_conversion<T, U>(color.g) },
              b { color_component_type_conversion<T, U>(color.b) }, a {
                  color_component_type_conversion<T, U>(color.a)
              } {}

        inline constexpr core::Vector3<T> toVector3() const noexcept { return { r, g, b }; }

        inline constexpr Vector4<T> toVector4() const noexcept { return { r, g, b, a }; }

        inline constexpr void fromVector3(const Vector3<T> &vector) noexcept {
            r = vector.r;
            g = vector.g;
            b = vector.b;
        }

        inline constexpr void fromVector4(const Vector4<T> &vector) noexcept {
            r = vector.r;
            g = vector.g;
            b = vector.b;
            a = vector.a;
        }

        template<typename U>
        inline constexpr RGBColor &operator=(RGBColor<U> color) noexcept {
            r = color_component_type_conversion<T, U>(color.r);
            g = color_component_type_conversion<T, U>(color.g);
            b = color_component_type_conversion<T, U>(color.b);
            a = color_component_type_conversion<T, U>(color.a);
        }

        union {
            T r;
            T red;
        };
        union {
            T g;
            T green;
        };
        union {
            T b;
            T blue;
        };
        union {
            T a;
            T alpha;
        };
    };

    using RGBColorB = RGBColor<Int8>;
    using RGBColorF = RGBColor<float>;

    namespace RGBColorDef {
        // HTML 4.01 colors:
        // https://en.wikipedia.org/wiki/Web_colors#HTML_color_names

        template<typename T>
        static constexpr auto Black =
            RGBColor<T> { T { 0 }, T { 0 }, T { 0 }, maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Gray = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Silver = RGBColor<T> {
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            maxColorComponentValue<T>()
        };

        template<typename T>
        static constexpr auto White = RGBColor<T> { maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Maroon = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                     T { 0 },
                                                     T { 0 },
                                                     maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Red = RGBColor<T> { maxColorComponentValue<T>(),
                                                  T { 0 },
                                                  T { 0 },
                                                  maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Olive = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                    maxColorComponentValue<T>() / T { 2 },
                                                    T { 0 },
                                                    maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Yellow = RGBColor<T> { maxColorComponentValue<T>(),
                                                     maxColorComponentValue<T>(),
                                                     T { 0 },
                                                     maxColorComponentValue<T>() };
        template<typename T>
        static constexpr auto Green = RGBColor<T> { T { 0 },
                                                    maxColorComponentValue<T>() / T { 2 },
                                                    T { 0 },
                                                    maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Lime = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   T { 0 },
                                                   maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Teal = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Aqua = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Navy = RGBColor<T> { T { 0 },
                                                   T { 0 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Blue = RGBColor<T> { T { 0 },
                                                   T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Purple = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                     T { 0 },
                                                     maxColorComponentValue<T>() / T { 2 },
                                                     maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Fuchsia = RGBColor<T> { maxColorComponentValue<T>(),
                                                      T { 0 },
                                                      maxColorComponentValue<T>(),
                                                      maxColorComponentValue<T>() };

        template<typename T>
        static constexpr auto Transparent = RGBColor<T> { T { 0 }, T { 0 }, T { 0 }, T { 0 } };
    } // namespace RGBColorDef
} // namespace storm::core

namespace std {
    template<typename T>
    struct hash<storm::core::RGBColor<T>> {
        constexpr inline storm::core::Hash64 operator()(const storm::core::RGBColor<T> &col) const {
            auto hash = storm::core::Hash64 { 0u };

            storm::core::hash_combine(hash, col.r);
            storm::core::hash_combine(hash, col.g);
            storm::core::hash_combine(hash, col.b);
            storm::core::hash_combine(hash, col.a);

            return hash;
        }
    };
} // namespace std
