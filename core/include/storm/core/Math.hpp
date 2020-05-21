// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <gsl/string_span>

#include <storm/core/Hash.hpp>
#include <storm/core/NamedType.hpp>
#include <storm/core/Strings.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RIGHT_HANDED

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace storm::core {
    using namespace glm;

    template<typename T>
    using Vector2 = glm::vec<2, T, glm::defaultp>;
    template<typename T>
    using Vector3 = glm::vec<3, T, glm::defaultp>;
    template<typename T>
    using Vector4  = glm::vec<4, T, glm::defaultp>;
    using Vector2f = glm::vec2;
    using Vector3f = glm::vec3;
    using Vector4f = glm::vec4;
    using Vector2u = glm::uvec2;
    using Vector3u = glm::uvec3;
    using Vector4u = glm::uvec4;
    using Vector2i = glm::ivec2;
    using Vector3i = glm::ivec3;
    using Vector4i = glm::ivec4;
    template<typename T>
    using Matrix      = glm::mat<4, 4, T, glm::defaultp>;
    using Matrixu     = glm::umat4x4;
    using Matrixi     = glm::imat4x4;
    using Matrixf     = glm::mat4;
    using Quaternionf = glm::quat;

    using Position2f = core::NamedType<Vector2f, struct Position2fTag>;
    using Position2u = core::NamedType<Vector2u, struct Position2uTag>;
    using Position2i = core::NamedType<Vector2i, struct Position2iTag>;
    using Position3f = core::NamedType<Vector3f, struct Position3fTag>;
    using Position3u = core::NamedType<Vector3u, struct Position3uTag>;
    using Position3i = core::NamedType<Vector3i, struct Position3iTag>;

    using MoveOffset2f = core::NamedType<Vector2f, struct MoveOffset2fTag>;
    using MoveOffset2u = core::NamedType<Vector2u, struct MoveOffset2uTag>;
    using MoveOffset2i = core::NamedType<Vector2i, struct MoveOffset2iTag>;
    using MoveOffset3f = core::NamedType<Vector3f, struct MoveOffset3fTag>;
    using MoveOffset3u = core::NamedType<Vector3u, struct MoveOffset3uTag>;
    using MoveOffset3i = core::NamedType<Vector3i, struct MoveOffset3iTag>;

    using Scale2f = core::NamedType<Vector2f, struct Scale2fTag>;
    using Scale2u = core::NamedType<Vector2u, struct Scale2uTag>;
    using Scale2i = core::NamedType<Vector2i, struct Scale2iTag>;
    using Scale3f = core::NamedType<Vector3f, struct Scale3fTag>;
    using Scale3u = core::NamedType<Vector3u, struct Scale3uTag>;
    using Scale3i = core::NamedType<Vector3i, struct Scale3iTag>;

    using Orientation2f = core::NamedType<Vector2f, struct Orientation2fTag>;
    using Orientation2u = core::NamedType<Vector2u, struct Orientation2uTag>;
    using Orientation2i = core::NamedType<Vector2i, struct Orientation2iTag>;
    using Orientation3f = core::NamedType<Vector3f, struct Orientation3fTag>;
    using Orientation3u = core::NamedType<Vector3u, struct Orientation3uTag>;
    using Orientation3i = core::NamedType<Vector3i, struct Orientation3iTag>;

    using Rotation2f = core::NamedType<Vector2f, struct Rotation2fTag>;
    using Rotation2u = core::NamedType<Vector2u, struct Rotation2uTag>;
    using Rotation2i = core::NamedType<Vector2i, struct Rotation2iTag>;
    using Rotation3f = core::NamedType<Vector3f, struct Rotation3fTag>;
    using Rotation3u = core::NamedType<Vector3u, struct Rotation3uTag>;
    using Rotation3i = core::NamedType<Vector3i, struct Rotation3iTag>;

    template<typename T>
    struct ExtentBase {
        using value_type = T;

        union {
            T width;
            T w;
        };
        union {
            T height;
            T h;
        };
        union {
            T depth = 1;
            T d;
        };

        inline bool operator==(const ExtentBase<T> &other) const noexcept {
            if constexpr (std::is_floating_point_v<T>)
                return realIsEqual(w, other.w) && realIsEqual(h, other.h) &&
                       realIsEqual(d, other.d);
            else
                return w == other.w && h == other.h && d == other.d;
        }

        inline bool operator!=(const ExtentBase<T> &other) const noexcept {
            return !operator==(other);
        }

        inline ExtentBase<T> operator*(T factor) const noexcept {
            return { .w = w * factor, .h = h * factor, .d = d * factor };
        }
        inline ExtentBase<T> operator/(T factor) const noexcept {
            return { .w = w / factor, .h = h / factor, .d = d / factor };
        }

        template<typename U>
        inline U convertTo() const noexcept {
            return { static_cast<typename U::value_type>(width),
                     static_cast<typename U::value_type>(height),
                     static_cast<typename U::value_type>(depth) };
        }
    };

    using Extenti = ExtentBase<core::Int32>;
    using Extentu = ExtentBase<core::UInt32>;
    using Extentf = ExtentBase<float>;

    inline std::string to_string(const storm::core::Extentu &extent) {
        return fmt::format("Extent { width: %{1}, height: %{2} }", extent.width, extent.height);
    }
} // namespace storm::core

HASH_FUNC(storm::core::Extentu)
HASH_FUNC(storm::core::Extentf)

CUSTOM_FORMAT(storm::core::Vector2f, "Vector2 {{ .x = {}, .y = {} }}", data.x, data.y)
CUSTOM_FORMAT(storm::core::Position2f, "Position2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::MoveOffset2f, "MoveOffset2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Scale2f, "Scale2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Orientation2f, "Orientation2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Rotation2f, "Rotation2 {{ .x = {}, .y = {} }}", data->x, data->y)

CUSTOM_FORMAT(storm::core::Vector3f,
              "Vector3 {{ .x = {}, .y = {}, .z = {} }}",
              data.x,
              data.y,
              data.z)
CUSTOM_FORMAT(storm::core::Position3f,
              "Position3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::MoveOffset3f,
              "MoveOffset3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Scale3f,
              "Scale3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Orientation3f,
              "Orientation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Rotation3f,
              "Rotation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)

CUSTOM_FORMAT(storm::core::Vector4f,
              "Vector4 {{ .x = {}, .y = {}, .z = {}, .w = {} }}",
              data.x,
              data.y,
              data.z,
              data.w)

CUSTOM_FORMAT(storm::core::Vector2u, "Vector2u {{ .x = {}, .y = {} }}", data.x, data.y)
CUSTOM_FORMAT(storm::core::Position2u, "Position2u {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::MoveOffset2u, "MoveOffset2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Scale2u, "Scale2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Orientation2u, "Orientation2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Rotation2u, "Rotation2 {{ .x = {}, .y = {} }}", data->x, data->y)

CUSTOM_FORMAT(storm::core::Vector3u,
              "Vector3u {{ .x = {}, .y = {}, .z = {} }}",
              data.x,
              data.y,
              data.z)
CUSTOM_FORMAT(storm::core::Position3u, "Position3u {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::MoveOffset3u,
              "MoveOffset3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Scale3u,
              "Scale3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Orientation3u,
              "Orientation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Rotation3u,
              "Rotation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)

CUSTOM_FORMAT(storm::core::Vector4u,
              "Vector4u {{ .x = {}, .y = {}, .z = {}, .w = {} }}",
              data.x,
              data.y,
              data.z,
              data.w)

CUSTOM_FORMAT(storm::core::Vector2i, "Vector2i {{ .x = {}, .y = {} }}", data.x, data.y)
CUSTOM_FORMAT(storm::core::Position2i, "Position2i {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::MoveOffset2i, "MoveOffset2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Scale2i, "Scale2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Orientation2i, "Orientation2 {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::Rotation2i, "Rotation2 {{ .x = {}, .y = {} }}", data->x, data->y)

CUSTOM_FORMAT(storm::core::Vector3i,
              "Vector3i {{ .x = {}, .y = {}, .z = {} }}",
              data.x,
              data.y,
              data.z)
CUSTOM_FORMAT(storm::core::Position3i, "Position3i {{ .x = {}, .y = {} }}", data->x, data->y)
CUSTOM_FORMAT(storm::core::MoveOffset3i,
              "MoveOffset3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Scale3i,
              "Scale3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Orientation3i,
              "Orientation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
CUSTOM_FORMAT(storm::core::Rotation3i,
              "Rotation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)

CUSTOM_FORMAT(storm::core::Vector4i,
              "Vector4i {{ .x = {}, .y = {}, .z = {}, .w = {} }}",
              data.x,
              data.y,
              data.z,
              data.w)

CUSTOM_FORMAT(storm::core::Matrixf,
              "Matrix {{\n    {},\n    {},\n    {},\n    {}\n}}",
              data[0],
              data[1],
              data[2],
              data[3])

CUSTOM_FORMAT(storm::core::Extenti,
              "Extent {{ .width = {}, .height = {} }}",
              data.width,
              data.height)
CUSTOM_FORMAT(storm::core::Extentu,
              "Extent {{ .width = {}, .height = {} }}",
              data.width,
              data.height)
CUSTOM_FORMAT(storm::core::Extentf,
              "Extent {{ .width = {}, .height = {} }}",
              data.width,
              data.height)
