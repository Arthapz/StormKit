// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
PipelineCache::PipelineCache(const Device &device)
	: m_device{core::makeObserver(&device)} {
}

/////////////////////////////////////
/////////////////////////////////////
PipelineCache::~PipelineCache() = default;

/////////////////////////////////////
/////////////////////////////////////
PipelineCache::PipelineCache(PipelineCache &&) = default;

/////////////////////////////////////
/////////////////////////////////////
PipelineCache &PipelineCache::operator=(PipelineCache &&) = default;

GraphicsPipeline &PipelineCache::getPipeline(const GraphicsPipelineState &state,
											 const RenderPass &pass) {
	if (!has(state)) {
		auto pipeline = m_device->createGraphicsPipeline();
		pipeline->setState(state);
		pipeline->setRenderPass(pass);
		pipeline->build();

		m_pipelines[state] = std::move(pipeline);
	}

	return *m_pipelines[state];
}

bool PipelineCache::has(const GraphicsPipelineState &state) noexcept {
	auto it = m_pipelines.find(state);

	return it != std::cend(m_pipelines);
}
