// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:utils.color;

import std;

import :hash;
import :typesafe.integer;
import :math.linear.vector;
import :meta;

export namespace stormkit { inline namespace core {
    namespace meta {
        template<class T>
        concept ColorComponentStorageType = SameAs<T, f32> or SameAs<T, u8>;
    }

    enum class ColorLayout {
        R,
        RG,
        RGB,
        BGR,
        RGBA,
        ARGB,
        BGRA,
        ABGR,
    };

    template<meta::ColorComponentStorageType T>
    struct ColorComponent {
        static constexpr auto max() noexcept -> T;
    };

    template<meta::ColorComponentStorageType To, meta::ColorComponentStorageType From>
    constexpr auto as_impl(ColorComponent<From> component) noexcept -> ColorComponent<To>;

    template<ColorLayout A, meta::ColorComponentStorageType B>
    struct color;

    template<ColorLayout LAYOUT, ColorLayout LAYOUT_T, meta::ColorComponentStorageType T>
    constexpr auto to_layout(const color<LAYOUT_T, T>& color) noexcept -> stormkit::color<LAYOUT, T>;

    template<meta::ColorComponentStorageType U, ColorLayout LAYOUT_T, meta::ColorComponentStorageType T>
    constexpr auto to_storage(const color<LAYOUT_T, T>& color) noexcept -> stormkit::color<LAYOUT_T, U>;

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::R, T> {
        static constexpr auto LAYOUT = ColorLayout::R;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 1;
        T                     r;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::RG, T> {
        static constexpr auto LAYOUT = ColorLayout::RG;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 2;
        T                     r;
        T                     g;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::RGB, T> {
        static constexpr auto LAYOUT = ColorLayout::RGB;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 3;
        T                     r;
        T                     g;
        T                     b;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::RGBA, T> {
        static constexpr auto LAYOUT = ColorLayout::RGBA;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 4;
        T                     r;
        T                     g;
        T                     b;
        T                     a;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::ARGB, T> {
        static constexpr auto LAYOUT = ColorLayout::ARGB;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 4;
        T                     a;
        T                     r;
        T                     g;
        T                     b;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::BGR, T> {
        static constexpr auto LAYOUT = ColorLayout::BGR;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 3;
        T                     b;
        T                     g;
        T                     r;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::BGRA, T> {
        static constexpr auto LAYOUT = ColorLayout::BGRA;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 4;
        T                     b;
        T                     g;
        T                     r;
        T                     a;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    struct color<ColorLayout::ABGR, T> {
        static constexpr auto LAYOUT = ColorLayout::ABGR;
        using Storage                = T;

        static constexpr auto COMPONENTS_COUNT = 4;
        T                     a;
        T                     b;
        T                     g;
        T                     r;

        constexpr auto operator==(const color& other) const noexcept -> bool;
    };

    template<meta::ColorComponentStorageType T>
    using color_r = color<ColorLayout::R, T>;
    template<meta::ColorComponentStorageType T>
    using color_rg = color<ColorLayout::RG, T>;
    template<meta::ColorComponentStorageType T>
    using color_rgb = color<ColorLayout::RGB, T>;
    template<meta::ColorComponentStorageType T>
    using color_rgba = color<ColorLayout::RGBA, T>;
    template<meta::ColorComponentStorageType T>
    using color_argb = color<ColorLayout::ARGB, T>;
    template<meta::ColorComponentStorageType T>
    using color_bgr = color<ColorLayout::BGR, T>;
    template<meta::ColorComponentStorageType T>
    using color_bgra = color<ColorLayout::BGRA, T>;
    template<meta::ColorComponentStorageType T>
    using color_abgr = color<ColorLayout::ABGR, T>;

    using fcolor_r    = color_r<f32>;
    using fcolor_rg   = color_rg<f32>;
    using fcolor_rgb  = color_rgb<f32>;
    using fcolor_rgba = color_rgba<f32>;
    using fcolor_argb = color_argb<f32>;
    using fcolor_bgr  = color_bgr<f32>;
    using fcolor_bgra = color_bgra<f32>;
    using fcolor_abgr = color_abgr<f32>;

    using ucolor_r    = color_r<u8>;
    using ucolor_rg   = color_rg<u8>;
    using ucolor_rgb  = color_rgb<u8>;
    using ucolor_rgba = color_rgba<u8>;
    using ucolor_argb = color_argb<u8>;
    using ucolor_bgr  = color_bgr<u8>;
    using ucolor_bgra = color_bgra<u8>;
    using ucolor_abgr = color_abgr<u8>;

    constexpr auto as_string(ColorLayout layout) noexcept -> std::string_view;
    constexpr auto to_string(ColorLayout layout) noexcept -> std::string;

    template<ColorLayout LAYOUT, meta::ColorComponentStorageType T>
    constexpr auto to_string(const color<LAYOUT, T>& color) noexcept -> std::string;

    template<ColorLayout LAYOUT, meta::ColorComponentStorageType T, typename FormatContext>
    auto format_as(const color<LAYOUT, T>& color, FormatContext& ctx) noexcept -> decltype(ctx.out());

    template<meta::HashType Ret = hash32, ColorLayout LAYOUT, meta::ColorComponentStorageType T>
    constexpr auto hasher(const color<LAYOUT, T>& color) noexcept -> Ret;
}} // namespace stormkit::core

namespace stormkit { inline namespace core { namespace details {
    template<ColorLayout LAYOUT, meta::ColorComponentStorageType T>
    struct ImplicitConverter {
        template<ColorLayout LAYOUTU, meta::ColorComponentStorageType U>
        constexpr operator color<LAYOUTU, U>() const noexcept;

        color<LAYOUT, T> c;
    };
}}} // namespace stormkit::core::details

export namespace stormkit { inline namespace core {
    /// This namespace contain preset colors, these colors are defined from [HTML 4.01
    /// colors](https://en.wikipedia.org/wiki/Web_colors#HTML_color_names).
    ///```
    /// | name        | red |green|blue |alpha|
    /// |-------------|-----|-----|-----|-----|
    /// | BLACK       | 0   | 0   | 0   | 255 |
    /// | Gray        | 127 | 127 | 127 | 255 |
    /// | SILVER      | 190 | 190 | 190 | 255 |
    /// | WHITE       | 255 | 255 | 255 | 255 |
    /// | Maroon      | 127 | 0   | 0   | 255 |
    /// | RED         | 255 | 0   | 0   | 255 |
    /// | Olive       | 127 | 127 | 0   | 255 |
    /// | YELLOW      | 255 | 255 | 0   | 255 |
    /// | GREEN       | 0   | 127 | 0   | 255 |
    /// | LIME        | 0   | 255 | 0   | 255 |
    /// | TEAL        | 0   | 127 | 127 | 255 |
    /// | AQUA        | 0   | 255 | 255 | 255 |
    /// | NAVY        | 0   | 0   | 127 | 255 |
    /// | BLUE        | 0   | 0   | 255 | 255 |
    /// | PURPLE      | 127 | 0   | 127 | 255 |
    /// | FUSCHIA     | 255 | 0   | 255 | 255 |
    /// | TRANSPARENT | 0   | 0   | 0   | 0   |
    ///```

    namespace colors {
        template<meta::ColorComponentStorageType T>
        inline constexpr auto BLACK = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = 0, .b = 0, .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto GRAY = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max() / T { 2 },
                  .g = ColorComponent<T>::max() / T { 2 },
                  .b = ColorComponent<T>::max() / T { 2 },
                  .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto SILVER = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max() / T { 2 } + ColorComponent<T>::max() / T { 4 },
                  .g = ColorComponent<T>::max() / T { 2 } + ColorComponent<T>::max() / T { 4 },
                  .b = ColorComponent<T>::max() / T { 2 } + ColorComponent<T>::max() / T { 4 },
                  .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto WHITE = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max(),
                  .g = ColorComponent<T>::max(),
                  .b = ColorComponent<T>::max(),
                  .a = ColorComponent<T>::max() }
        };

        template<meta::IsColorComponent T>
        inline constexpr auto MAROON = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max() / T { 2 }, .g = T { 0 }, .b = T { 0 }, .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto RED = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max(), .g = 0, .b = 0, .a = ColorComponent<T>::max() }
        };

        template<meta::IsColorComponent T>
        inline constexpr auto OLIVE = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max() / T { 2 },
                  .g = ColorComponent<T>::max() / T { 2 },
                  .b = T { 0 },
                  .a = ColorComponent<T>::max() }
        };

        template<meta::IsColorComponent T>
        inline constexpr auto YELLOW = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max(), .g = ColorComponent<T>::max(), .b = T { 0 }, .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto GREEN = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = ColorComponent<T>::max() / T { 2 }, .b = 0, .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto LIME = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = ColorComponent<T>::max(), .b = 0, .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto TEAL = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0,
                  .g = ColorComponent<T>::max() / T { 2 },
                  .b = ColorComponent<T>::max() / T { 2 },
                  .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto AQUA = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = ColorComponent<T>::max(), .b = ColorComponent<T>::max(), .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto NAVY = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = 0, .b = ColorComponent<T>::max() / T { 2 }, .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto BLUE = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = 0, .b = ColorComponent<T>::max(), .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto PURPLE = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max() / T { 2 },
                  .g = 0,
                  .b = ColorComponent<T>::max() / T { 2 },
                  .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto FUSCHIA = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = ColorComponent<T>::max(), .g = 0, .b = ColorComponent<T>::max(), .a = ColorComponent<T>::max() }
        };

        template<meta::ColorComponentStorageType T>
        inline constexpr auto TRANSPARENT = details::ImplicitConverter<ColorLayout::RGBA, T> {
            .c = { .r = 0, .g = 0, .b = 0, .a = 0 }
        };
    } // namespace colors
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorLayout LAYOUT, ColorLayout LAYOUT_T, meta::ColorComponentStorageType T>
    constexpr auto to_layout(const color<LAYOUT_T, T>& in) noexcept -> stormkit::color<LAYOUT, T> {
        if constexpr (LAYOUT == LAYOUT_T) return in;
        else {
            using InColor  = color<LAYOUT_T, T>;
            using OutColor = color<LAYOUT, T>;
            auto out       = OutColor {};
            out.r          = in.r;

            if constexpr (OutColor::COMPONENTS_COUNT > 1 and InColor::COMPONENTS_COUNT > 1) {
                out.g = in.g;
                if constexpr (OutColor::COMPONENTS_COUNT > 2 and InColor::COMPONENTS_COUNT > 2) {
                    out.b = in.b;
                    if constexpr (OutColor::COMPONENTS_COUNT > 3 and InColor::COMPONENTS_COUNT > 3) out.a = in.a;
                }
            }

            return out;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType U, ColorLayout LAYOUT_T, meta::ColorComponentStorageType T>
    constexpr auto to_storage(const color<LAYOUT_T, T>& in) noexcept -> stormkit::color<LAYOUT_T, U> {
        if constexpr (meta::SameAs<T, U>) return in;
        else {
            using OutColor = color<LAYOUT_T, U>;
            auto out       = OutColor {};
            out.r          = as<U>(in.r);

            if constexpr (OutColor::COMPONENTS_COUNT > 1) {
                out.g = as<U>(in.g);
                if constexpr (OutColor::COMPONENTS_COUNT > 2) {
                    out.b = as<U>(in.b);
                    if constexpr (OutColor::COMPONENTS_COUNT > 3) out.a = as<U>(in.a);
                }
            }

            return out;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto color_component_as(u8 component) noexcept -> f32 {
        return as<f32>(component) / 255.f;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto color_component_as(f32 component) noexcept -> u8 {
        EXPECTS(component <= 1.f);
        return as<u8>(component * 255u);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsColorComponent T>
    constexpr auto max_color_component_value() noexcept -> T {
        if constexpr (std::is_same_v<T, f32>) return 1.f;

        return 255u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    constexpr auto ColorComponent<T>::max() noexcept -> T {
        if constexpr (meta::SameAs<T, f32>) return 1.f;
        else
            return 255u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType To, meta::ColorComponentStorageType From>
    constexpr auto as_impl(ColorComponent<From> component) noexcept -> ColorComponent<To> {
        if constexpr (meta::SameAs<To, f32>) return ColorComponent<To> { as<f32>(component.value) / 255.f };
        else
            return ColorComponent<To> { as<u8>(component.value) * 255.f };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorLayout LAYOUT, meta::ColorComponentStorageType T>
    template<ColorLayout LAYOUTU, meta::ColorComponentStorageType U>
    constexpr details::ImplicitConverter<LAYOUT, T>::operator color<LAYOUTU, U>() const noexcept {
        return to_storage<U>(to_layout<LAYOUTU>(c));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::R, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::RG, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::RGB, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g and b == other.b;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::RGBA, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g and b == other.b and a == other.a;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::ARGB, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g and b == other.b and a == other.a;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::BGR, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g and b == other.b;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::BGRA, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g and b == other.b and a == other.a;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto color<ColorLayout::ABGR, T>::operator==(const color& other) const noexcept -> bool {
        return r == other.r and g == other.g and b == other.b and a == other.a;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto as_string(ColorLayout layout) noexcept -> std::string_view {
        switch (layout) {
            case ColorLayout::R: return "R";
            case ColorLayout::RG: return "RG";
            case ColorLayout::RGB: return "RGB";
            case ColorLayout::BGR: return "BGR";
            case ColorLayout::RGBA: return "RGBA";
            case ColorLayout::ARGB: return "ARGB";
            case ColorLayout::BGRA: return "BGRA";
            case ColorLayout::ABGR: return "ABGR";
            default: break;
        }
        std::unreachable();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto to_string(ColorLayout layout) noexcept -> std::string {
        return std::string { as_string(layout) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorLayout LAYOUT, meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto to_string(const color<LAYOUT, T>& color) noexcept -> std::string {
        return std::format("{}", color);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorLayout LAYOUT, meta::ColorComponentStorageType T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const color<LAYOUT, T>& color, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        if constexpr (LAYOUT == ColorLayout::R) return std::format_to(ctx.out(), "[color layout: R, red: {}]", color.r);
        else if constexpr (LAYOUT == ColorLayout::RG)
            return std::format_to(ctx.out(), "[color layout: RG, red: {}, green: {}]", color.r, color.g);
        else if constexpr (LAYOUT == ColorLayout::RGB)
            return std::format_to(ctx.out(), "[color layout: RGB, red: {}, green: {}, blue: {}]", color.r, color.g, color.b);
        else if constexpr (LAYOUT == ColorLayout::BGR)
            return std::format_to(ctx.out(), "[color layout: BGR, blue: {}, green: {}, red: {}]", color.r, color.g);
        else if constexpr (LAYOUT == ColorLayout::RGBA)
            return std::format_to(ctx.out(),
                                  "[color layout: RGBA, red: {}, green: {}, blue: {}, alpha: {}]",
                                  color.r,
                                  color.g,
                                  color.b,
                                  color.a);
        else if constexpr (LAYOUT == ColorLayout::ARGB)
            return std::format_to(ctx.out(),
                                  "[color layout: ARGB, alpha: {}, red: {}, green: {}, blue: {}]",
                                  color.a,
                                  color.r,
                                  color.g,
                                  color.b);
        else if constexpr (LAYOUT == ColorLayout::BGRA)
            return std::format_to(ctx.out(),
                                  "[color layout: BGRA, bue: {}, green: {}, red: {}, alpha: {}]",
                                  color.b,
                                  color.g,
                                  color.r,
                                  color.a);
        else if constexpr (LAYOUT == ColorLayout::ABGR)
            return std::format_to(ctx.out(),
                                  "[color layout: ABGR, alpha: {}, blue: {}, green: {}, red: {}]",
                                  color.a,
                                  color.b,
                                  color.g,
                                  color.r);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, ColorLayout LAYOUT, meta::ColorComponentStorageType T>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const color<LAYOUT, T>& color) noexcept -> Ret {
        if constexpr (LAYOUT == ColorLayout::R) return hash(color.r);
        else if constexpr (LAYOUT == ColorLayout::RG)
            return hash(color.r, color.g);
        else if constexpr (LAYOUT == ColorLayout::RGB)
            return hash(color.r, color.g, color.b);
        else if constexpr (LAYOUT == ColorLayout::BGR)
            return hash(color.b, color.g, color.r);
        else if constexpr (LAYOUT == ColorLayout::RGBA)
            return hash(color.r, color.g, color.b, color.a);
        else if constexpr (LAYOUT == ColorLayout::ARGB)
            return hash(color.a, color.r, color.g, color.b);
        else if constexpr (LAYOUT == ColorLayout::BGRA)
            return hash(color.b, color.g, color.r, color.a);
        else if constexpr (LAYOUT == ColorLayout::ABGR)
            return hash(color.a, color.b, color.g, color.r);
    }
}} // namespace stormkit::core
