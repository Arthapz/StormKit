// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::graphics {
	class FrameGraph;
	DECLARE_PTR_AND_REF(FrameGraph);

	class FramePassBuilder;
	DECLARE_PTR_AND_REF(FramePassBuilder);

	class FrameGraphResources;
	DECLARE_PTR_AND_REF(FrameGraphResources);

	class FramePassBase;
	DECLARE_PTR_AND_REF(FramePassBase)

	template <typename Data> class FramePass;

	class FrameGraphTextureResource;
	DECLARE_PTR_AND_REF(FrameGraphTextureResource)

	struct FrameGraphTextureDesc;
	struct FrameGraphBufferDesc;
} // namespace storm::graphics
