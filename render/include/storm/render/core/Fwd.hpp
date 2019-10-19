// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::render {
	struct PhysicalDeviceInfo;
	struct RenderCapabilities;

	class Instance;
	DECLARE_PTR_AND_REF(Instance);

	class PhysicalDevice;
	DECLARE_PTR_AND_REF(PhysicalDevice);

	class Device;
	DECLARE_PTR_AND_REF(Device);

	class Surface;
	DECLARE_PTR_AND_REF(Surface);

	class Queue;
	DECLARE_PTR_AND_REF(Queue);

	class CommandBuffer;
	DECLARE_PTR_AND_REF(CommandBuffer);
} // namespace storm::render
