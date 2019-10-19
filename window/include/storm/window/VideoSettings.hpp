// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>
#include <vector>

namespace storm::window {
	struct VideoSettings {
		core::Extent size;
		union {
			std::uint8_t bitsPerPixel;
			std::uint8_t bpp;
		};
		union {
			std::uint16_t dotsPerInch;
			std::uint16_t dpi;
		};

		STORM_PUBLIC static std::vector<VideoSettings> getDesktopModes();
	};

} // namespace storm::window
