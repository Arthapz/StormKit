// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/Math.hpp>

namespace storm::render {
	struct Viewport {
		core::Vector2 position;
		core::Extentf extent;
		core::Vector2 depth;
	};

	struct Scissor {
		core::Vector2i offset;
		core::Extent extent;
	};

	struct ClearColor {
        core::RGBColorF color = storm::core::RGBColorDef::Silver;
	};

	struct ClearDepthStencil {
		float depth			  = 1.f;
		std::uint32_t stencil = 0;
	};

	using ClearValue = std::variant<ClearColor, ClearDepthStencil>;

} // namespace storm::render

HASH_FUNC(storm::render::Viewport)
HASH_FUNC(storm::render::Scissor)
