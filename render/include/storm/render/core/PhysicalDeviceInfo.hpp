// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/render/core/Enums.hpp>
#include <string>

namespace storm::render {
	struct PhysicalDeviceInfo {
		std::string device_name;
		std::string vendor_name;

		std::uint32_t api_major_version;
		std::uint32_t api_minor_version;
		std::uint32_t api_patch_version;

		std::uint32_t driver_major_version;
		std::uint32_t driver_minor_version;
		std::uint32_t driver_patch_version;

		PhysicalDeviceType type;
	};
} // namespace storm::render
