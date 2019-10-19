// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/Hash.hpp>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
	struct VertexBindingDescription {
		std::uint32_t binding;
		std::size_t stride;
		VertexInputRate input_rate = VertexInputRate::Vertex;
	};

	using VertexBindingDescriptions = std::vector<VertexBindingDescription>;
} // namespace storm::render

HASH_FUNC(storm::render::VertexBindingDescription)
HASH_FUNC(storm::render::VertexBindingDescriptions)
