// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <storm/core/Hash.hpp>
#include <storm/core/Numerics.hpp>
#include <type_traits>

#ifdef STORM_GLM_CONVERSION
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#endif

namespace storm::core {
	/**
	 * @struct RGBColor
	 * @ingroup Core
	 * @brief Color class
	 *
	 * Utility class for manipulating RGBA colors
	 */
	struct RGBColorF;
	struct RGBColor {
		constexpr RGBColor(std::uint8_t red = 0, std::uint8_t green = 0,
						   std::uint8_t blue  = 0,
						   std::uint8_t alpha = 255) noexcept
			: a(alpha), g(green), b(blue), r(red) {
		}
		constexpr RGBColor(std::uint32_t argb) noexcept
			: a(255), g(0), b(0), r(0) {
			r = argb & 0xFF;
			g = (argb >> 8) & 0xFF;
			b = (argb >> 16) & 0xFF;
			a = (argb >> 24) & 0xFF;
		}

		constexpr operator RGBColorF() const noexcept;

		constexpr operator std::uint32_t() const noexcept {
			return ((std::uint32_t(a) << 24) & 0xFF000000) |
				   ((std::uint32_t(r) << 16) & 0x00FF0000) |
				   ((std::uint32_t(g) << 8) & 0x0000FF00) |
				   (std::uint32_t(b) & 0x000000FF);
		}

		static inline RGBColor random() {
			return {static_cast<std::uint8_t>(rand<std::uint16_t>(0, 255)),
					static_cast<std::uint8_t>(rand<std::uint16_t>(0, 255)),
					static_cast<std::uint8_t>(rand<std::uint16_t>(0, 255)),
					255};
		}

#ifdef STORM_GLM_CONVERSION
		template <typename T = glm::uvec3,
				  typename   = std::enable_if_t<std::is_same_v<
						std::decay_t<std::remove_cv_t<T>>, glm::uvec3>>>
		static RGBColor fromVec3(T &&vec) {
			return RGBColor(vec.x, vec.y, vec.z);
		}

		template <typename T = glm::uvec4,
				  typename   = std::enable_if_t<std::is_same_v<
						std::decay_t<std::remove_cv_t<T>>, glm::uvec4>>>
		static RGBColor fromVec4(T &&vec) {
			return RGBColor(vec.x, vec.y, vec.z, vec.w);
		}

		operator glm::uvec3() const noexcept {
			return {r, g, b};
		}

		operator glm::uvec4() const noexcept {
			return {r, g, b, a};
		}
#endif

		std::uint8_t a;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t r;
	};

	namespace RGBColorDef {
		// HTML 4.01 colors:
		// https://en.wikipedia.org/wiki/Web_colors#HTML_color_names
		static constexpr RGBColor Black		  = {0, 0, 0, 255};
		static constexpr RGBColor Gray		  = {128, 128, 128, 255};
		static constexpr RGBColor Silver	  = {192, 192, 192, 255};
		static constexpr RGBColor White		  = {255, 255, 255, 255};
		static constexpr RGBColor Maroon	  = {128, 0, 0, 255};
		static constexpr RGBColor Red		  = {255, 0, 0, 255};
		static constexpr RGBColor Olive		  = {128, 128, 0, 255};
		static constexpr RGBColor Yellow	  = {255, 255, 0, 255};
		static constexpr RGBColor Green		  = {0, 128, 0, 255};
		static constexpr RGBColor Lime		  = {0, 255, 0, 255};
		static constexpr RGBColor Teal		  = {0, 128, 128, 255};
		static constexpr RGBColor Aqua		  = {0, 255, 255, 255};
		static constexpr RGBColor Navy		  = {0, 0, 128, 255};
		static constexpr RGBColor Blue		  = {0, 0, 255, 255};
		static constexpr RGBColor Purple	  = {128, 0, 128, 255};
		static constexpr RGBColor Fuchsia	 = {255, 0, 255, 255};
		static constexpr RGBColor Transparent = {0, 0, 0, 0};
	} // namespace RGBColorDef

	struct RGBColorF {
		constexpr RGBColorF(float red = 0.f, float green = 0.f,
							float blue = 0.f, float alpha = 1.f) noexcept
			: r(red), g(green), b(blue), a(alpha) {
		}
		template <typename T = RGBColor,
				  typename   = std::enable_if_t<std::is_same_v<
						std::decay_t<std::remove_cv_t<T>>, RGBColor>>>
		constexpr explicit RGBColorF(T &&color) noexcept
			: r(color.r), g(color.g), b(color.b), a(color.a) {
		}

		constexpr inline bool operator==(const RGBColor &color) const noexcept {
			auto _r = float(color.r) / 255.f;
			auto _g = float(color.g) / 255.f;
			auto _b = float(color.b) / 255.f;
			auto _a = float(color.a) / 255.f;

			return realIsEqual(r, _r) && realIsEqual(g, _g) &&
				   realIsEqual(b, _b) && realIsEqual(a, _a);
		}

		constexpr inline bool operator==(const RGBColorF &color) const
			noexcept {
			return realIsEqual(r, color.r) && realIsEqual(g, color.g) &&
				   realIsEqual(b, color.b) && realIsEqual(a, color.a);
		}

		static inline RGBColorF random() {
			return {rand<float>(0.f, 1.f), rand<float>(0.f, 1.f),
					rand<float>(0.f, 1.f), 1.f};
		}

		template <typename T = RGBColor,
				  typename   = std::enable_if_t<std::is_same_v<
						std::decay_t<std::remove_cv_t<T>>, RGBColor>>>
		constexpr inline void operator=(T &&color) noexcept {
			r = color.r;
			g = color.g;
			b = color.b;
			a = color.a;

			r /= 255.f;
			g /= 255.f;
			b /= 255.f;
			a /= 255.f;
		}

		float r;
		float g;
		float b;
		float a;

		constexpr operator RGBColor() const noexcept {
			return {static_cast<std::uint8_t>(r * 255),
					static_cast<std::uint8_t>(g * 255),
					static_cast<std::uint8_t>(b * 255),
					static_cast<std::uint8_t>(a * 255)};
		}

#ifdef STORM_GLM_CONVERSION
		template <typename T = glm::vec3,
				  typename   = std::enable_if_t<std::is_same_v<
						std::decay_t<std::remove_cv_t<T>>, glm::vec3>>>
		static RGBColorF fromVec3(T &&vec) {
			return RGBColorF(vec.x, vec.y, vec.z);
		}

		template <typename T = glm::vec4,
				  typename   = std::enable_if_t<std::is_same_v<
						std::decay_t<std::remove_cv_t<T>>, glm::vec4>>>
		static RGBColorF fromVec4(T &&vec) {
			return RGBColorF(vec.x, vec.y, vec.z, vec.w);
		}

		operator glm::vec3() const noexcept {
			return {r, g, b};
		}

		operator glm::vec4() const noexcept {
			return {r, g, b, a};
		}
#endif
	};

	constexpr RGBColor::operator RGBColorF() const noexcept {
		return {r / 255.f, g / 255.f, b / 255.f, a / 255.f};
	}
} // namespace storm::core

namespace std {
	template <> struct hash<storm::core::RGBColor> {
		std::size_t operator()(const storm::core::RGBColor &col) const {
			auto hash = std::hash<std::uint8_t>{}(col.r);
			storm::core::hash_combine(hash, std::hash<std::uint8_t>{}(col.g));
			storm::core::hash_combine(hash, std::hash<std::uint8_t>{}(col.b));
			storm::core::hash_combine(hash, std::hash<std::uint8_t>{}(col.a));

			return hash;
		}
	};

	template <> struct hash<storm::core::RGBColorF> {
		std::size_t operator()(const storm::core::RGBColorF &col) const {
			auto hash = std::hash<float>{}(col.r);
			storm::core::hash_combine(hash, std::hash<float>{}(col.g));
			storm::core::hash_combine(hash, std::hash<float>{}(col.b));
			storm::core::hash_combine(hash, std::hash<float>{}(col.a));

			return hash;
		}
	};
} // namespace std
