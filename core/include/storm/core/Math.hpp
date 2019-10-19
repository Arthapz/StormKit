// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/Hash.hpp>
#include <storm/core/NamedType.hpp>
#include <storm/core/Strings.hpp>
#include <storm/core/Types.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace storm::core {
	using namespace glm;

    using Vector2	 = glm::vec2;
    using Vector3	 = glm::vec3;
    using Vector4	 = glm::vec4;
    using Vector2u	 = glm::uvec2;
    using Vector3u	 = glm::uvec3;
    using Vector4u	 = glm::uvec4;
    using Vector2i	 = glm::ivec2;
    using Vector3i	 = glm::ivec3;
    using Vector4i	 = glm::ivec4;
	using Matrix	 = glm::mat4;
	using Quaternion = glm::quat;

    using Position2	 = core::NamedType<Vector2, struct Position2Tag>;
	using Position2u = core::NamedType<Vector2u, struct Position2uTag>;
	using Position2i = core::NamedType<Vector2i, struct Position2iTag>;
    using Position3	 = core::NamedType<Vector3, struct Position3Tag>;
	using Position3u = core::NamedType<Vector3u, struct Position3uTag>;
	using Position3i = core::NamedType<Vector3i, struct Position3iTag>;

	template <typename T> struct ExtentBase {
		using value_type = T;

		union {
			T width;
			T w;
		};
		union {
			T height;
			T h;
		};

		inline bool operator==(const ExtentBase<T> &other) {
			if constexpr (std::is_floating_point_v<T>)
				return realIsEqual(w, other.w) && realIsEqual(h, other.h);
			else
				return w == other.w && h == other.h;
		}

		template <typename U> inline U convertTo() const noexcept {
			return {static_cast<typename U::value_type>(width),
					static_cast<typename U::value_type>(height)};
		}
	};

	using Extent  = ExtentBase<std::uint32_t>;
	using Extentf = ExtentBase<float>;

	inline std::string to_string(const storm::core::Extent &extent) {
		return fmt::format("Extent { width: %{1}, height: %{2} }", extent.width,
						   extent.height);
	}
} // namespace storm::core

HASH_FUNC(storm::core::Extent)
HASH_FUNC(storm::core::Extentf)

CUSTOM_FORMAT(storm::core::Vector2, "Vector2 {{ .x = {}, .y = {} }}", data.x,
			  data.y)
CUSTOM_FORMAT(storm::core::Position2, "Position2 {{ .x = {}, .y = {} }}",
			  data->x, data->y)
CUSTOM_FORMAT(storm::core::Vector3, "Vector3 {{ .x = {}, .y = {}, .z = {} }}",
			  data.x, data.y, data.z)
CUSTOM_FORMAT(storm::core::Position3, "Position3 {{ .x = {}, .y = {} }}",
			  data->x, data->y)
CUSTOM_FORMAT(storm::core::Vector4,
			  "Vector4 {{ .x = {}, .y = {}, .z = {}, .w = {} }}", data.x,
			  data.y, data.z, data.w)
CUSTOM_FORMAT(storm::core::Vector2u, "Vector2u {{ .x = {}, .y = {} }}", data.x,
			  data.y)
CUSTOM_FORMAT(storm::core::Position2u, "Position2u {{ .x = {}, .y = {} }}",
			  data->x, data->y)
CUSTOM_FORMAT(storm::core::Vector3u, "Vector3u {{ .x = {}, .y = {}, .z = {} }}",
			  data.x, data.y, data.z)
CUSTOM_FORMAT(storm::core::Position3u, "Position3u {{ .x = {}, .y = {} }}",
			  data->x, data->y)
CUSTOM_FORMAT(storm::core::Vector4u,
			  "Vector4u {{ .x = {}, .y = {}, .z = {}, .w = {} }}", data.x,
			  data.y, data.z, data.w)
CUSTOM_FORMAT(storm::core::Vector2i, "Vector2i {{ .x = {}, .y = {} }}", data.x,
			  data.y)
CUSTOM_FORMAT(storm::core::Position2i, "Position2i {{ .x = {}, .y = {} }}",
			  data->x, data->y)
CUSTOM_FORMAT(storm::core::Vector3i, "Vector3i {{ .x = {}, .y = {}, .z = {} }}",
			  data.x, data.y, data.z)
CUSTOM_FORMAT(storm::core::Position3i, "Position3i {{ .x = {}, .y = {} }}",
			  data->x, data->y)
CUSTOM_FORMAT(storm::core::Vector4i,
			  "Vector4i {{ .x = {}, .y = {}, .z = {}, .w = {} }}", data.x,
			  data.y, data.z, data.w)
CUSTOM_FORMAT(storm::core::Matrix,
			  "Matrix {{\n    {},\n    {},\n    {},\n    {}\n}}", data[0],
			  data[1], data[2], data[3])
CUSTOM_FORMAT(storm::core::Extent, "Extent {{ .width = {}, .height = {} }}",
			  data.width, data.height)
CUSTOM_FORMAT(storm::core::Extentf, "Extent {{ .width = {}, .height = {} }}",
			  data.width, data.height)
