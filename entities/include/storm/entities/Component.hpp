// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <storm/core/Assert.hpp>
#include <storm/core/Hash.hpp>
#include <storm/core/Memory.hpp>

#include <storm/entities/Fwd.hpp>

namespace storm::entities {
	struct STORM_PUBLIC Component {
		explicit Component();
		virtual ~Component() = 0;

		Component(const Component &);
		Component &operator=(const Component &);

		Component(Component &&);
		Component &operator=(Component &&);

		using Type = std::uint64_t;

		static constexpr Type INVALID_TYPE = 0;
		static constexpr Type TYPE		   = INVALID_TYPE;
	};
} // namespace storm::entities

constexpr storm::entities::Component::Type
	operator"" _component_type(const char *str, std::size_t size) {
	return storm::core::ComponentHash<storm::entities::Component::Type>(str,
																		size);
}
