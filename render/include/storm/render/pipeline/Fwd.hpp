// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::render {
	class AbstractPipeline;
	DECLARE_PTR_AND_REF(AbstractPipeline);

	class GraphicsPipeline;
	DECLARE_PTR_AND_REF(GraphicsPipeline);

	class RenderPass;
	DECLARE_PTR_AND_REF(RenderPass);

	class Framebuffer;
	DECLARE_PTR_AND_REF(Framebuffer);

	class DescriptorSetLayout;
	DECLARE_PTR_AND_REF(DescriptorSetLayout);

	class DescriptorPool;
	DECLARE_PTR_AND_REF(DescriptorPool);

	class DescriptorSet;
	DECLARE_PTR_AND_REF(DescriptorSet);

	class PipelineCache;
	DECLARE_PTR_AND_REF(PipelineCache);

	struct GraphicsPipelineState;
    DECLARE_PTR_AND_REF(GraphicsPipelineState);
} // namespace storm::render
