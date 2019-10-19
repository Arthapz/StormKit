// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
	struct VertexInputAttributeDescription {
		std::uint32_t binding;
		std::uint32_t location;
		Format format;
		std::ptrdiff_t offset;
	};

	using VertexInputAttributeDescriptions =
		std::vector<VertexInputAttributeDescription>;
} // namespace storm::render

HASH_FUNC(storm::render::VertexInputAttributeDescription)
HASH_FUNC(storm::render::VertexInputAttributeDescriptions)
