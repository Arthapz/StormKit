// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>
#include <vector>

#include <storm/core/Hash.hpp>
#include <storm/core/Platform.hpp>

#include <storm/core/Math.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

namespace storm::graphics {
	struct VertexP2C3 {
		core::Vector2 position;
		core::Vector3 color;

		STORM_PUBLIC static const render::VertexInputAttributeDescriptions
			ATTRIBUTE_DESCRIPTIONS;

		inline bool operator==(const VertexP2C3 &other) const {
			return position == other.position && other.color == color;
		}
    };

    struct VertexP2UV2 {
        core::Vector2 position;
        core::Vector2 uv;

        STORM_PUBLIC static const render::VertexInputAttributeDescriptions
            ATTRIBUTE_DESCRIPTIONS;

        inline bool operator==(const VertexP2UV2 &other) const {
            return position == other.position && other.uv == uv;
        }
    };

	struct VertexP3C3 {
		core::Vector3 position;
		core::Vector3 color;

		STORM_PUBLIC static const render::VertexInputAttributeDescriptions
			ATTRIBUTE_DESCRIPTIONS;

		inline bool operator==(const VertexP3C3 &other) const {
			return position == other.position && other.color == color;
		}
	};

	struct VertexP3N3 {
		core::Vector3 position;
		core::Vector3 normal;

		STORM_PUBLIC static const render::VertexInputAttributeDescriptions
			ATTRIBUTE_DESCRIPTIONS;

		inline bool operator==(const VertexP3N3 &other) const {
			return position == other.position && other.normal == normal;
		}
	};

	struct VertexP3C3N3 {
		core::Vector3 position;
		core::Vector3 color;
		core::Vector3 normal;

		STORM_PUBLIC static const render::VertexInputAttributeDescriptions
			ATTRIBUTE_DESCRIPTIONS;

		inline bool operator==(const VertexP3C3N3 &other) const {
			return position == other.position && other.color == color &&
				   other.normal == normal;
		}
	};

	struct VertexP3C3N3UV2 {
		core::Vector3 position;
		core::Vector3 color;
		core::Vector3 normal;
		core::Vector2 uv;

		STORM_PUBLIC static const render::VertexInputAttributeDescriptions
			ATTRIBUTE_DESCRIPTIONS;

		inline bool operator==(const VertexP3C3N3UV2 &other) const {
			return position == other.position && other.color == color &&
				   other.normal == normal && uv == other.uv;
		}
	};

	template <typename VertexType, std::size_t size>
	using StaticVertexArray = std::array<VertexType, size>;
	template <typename VertexType> using VertexArray = std::vector<VertexType>;
	using VertexP2C3Array							 = VertexArray<VertexP2C3>;
    using VertexP2UV2Array							 = VertexArray<VertexP2UV2>;
    using VertexP3C3Array							 = VertexArray<VertexP3C3>;
	using VertexP3N3Array							 = VertexArray<VertexP3N3>;
	using VertexP3C3N3Array	= VertexArray<VertexP3C3N3>;
	using VertexP3C3N3UV2Array = VertexArray<VertexP3C3N3UV2>;

	using VertexArrayProxy =
        std::variant<VertexP2C3Array, VertexP2UV2Array, VertexP3C3Array,
                     VertexP3N3Array, VertexP3C3N3Array, VertexP3C3N3UV2Array,
					 VertexArray<float>>;

	template <typename T> using ValueType = typename T::value_type;
	template <typename T>
	static constexpr auto const ValueTypeSize = sizeof(ValueType<T>);

	using ShortIndexArray = std::vector<std::uint16_t>;
	using LargeIndexArray = std::vector<std::uint32_t>;

	using IndexArrayProxy = std::variant<ShortIndexArray, LargeIndexArray>;
} // namespace storm::graphics

namespace std {
	template <> struct hash<storm::graphics::VertexP2C3> {
		std::size_t
			operator()(const storm::graphics::VertexP2C3 &vertex) const {
			static auto vec2_hasher = std::hash<storm::core::Vector2>{};
			static auto vec3_hasher = std::hash<storm::core::Vector3>{};

			auto hash = vec2_hasher(vertex.position);
			storm::core::hash_combine(hash, vec3_hasher(vertex.color));

			return hash;
		}
	};
} // namespace std

namespace std {
    template <> struct hash<storm::graphics::VertexP2UV2> {
        std::size_t
            operator()(const storm::graphics::VertexP2UV2 &vertex) const {
            static auto vec2_hasher = std::hash<storm::core::Vector2>{};

            auto hash = vec2_hasher(vertex.position);
            storm::core::hash_combine(hash, vec2_hasher(vertex.uv));

            return hash;
        }
    };
} // namespace std

namespace std {
	template <> struct hash<storm::graphics::VertexP3C3> {
		std::size_t
			operator()(const storm::graphics::VertexP3C3 &vertex) const {
			static auto vec3_hasher = std::hash<storm::core::Vector3>{};

			auto hash = vec3_hasher(vertex.position);
			storm::core::hash_combine(hash, vec3_hasher(vertex.color));

			return hash;
		}
	};
} // namespace std

namespace std {
	template <> struct hash<storm::graphics::VertexP3N3> {
		std::size_t
			operator()(const storm::graphics::VertexP3N3 &vertex) const {
			static auto vec3_hasher = std::hash<storm::core::Vector3>{};

			auto hash = vec3_hasher(vertex.position);
			storm::core::hash_combine(hash, vec3_hasher(vertex.normal));

			return hash;
		}
	};
} // namespace std

namespace std {
	template <> struct hash<storm::graphics::VertexP3C3N3> {
		std::size_t
			operator()(const storm::graphics::VertexP3C3N3 &vertex) const {
			static auto vec3_hasher = std::hash<storm::core::Vector3>{};

			auto hash = vec3_hasher(vertex.position);
			storm::core::hash_combine(hash, vec3_hasher(vertex.color));
			storm::core::hash_combine(hash, vec3_hasher(vertex.normal));

			return hash;
		}
	};
} // namespace std

namespace std {
	template <> struct hash<storm::graphics::VertexP3C3N3UV2> {
		std::size_t
			operator()(const storm::graphics::VertexP3C3N3UV2 &vertex) const {
			static auto vec2_hasher = std::hash<storm::core::Vector2>{};
			static auto vec3_hasher = std::hash<storm::core::Vector3>{};

			auto hash = vec3_hasher(vertex.position);
			storm::core::hash_combine(hash, vec3_hasher(vertex.color));
			storm::core::hash_combine(hash, vec3_hasher(vertex.normal));
			storm::core::hash_combine(hash, vec2_hasher(vertex.uv));

			return hash;
		}
	};
} // namespace std
