// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <gsl/string_span>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#include <unordered_map>

namespace storm::render {
	class STORM_PUBLIC PipelineCache : public core::NonCopyable {
	  public:
		explicit PipelineCache(const Device &device);
		~PipelineCache();

		PipelineCache(PipelineCache &&);
		PipelineCache &operator=(PipelineCache &&);

		render::GraphicsPipeline &
			getPipeline(const GraphicsPipelineState &state,
						const RenderPass &render_pass);

	  protected:
		bool has(const GraphicsPipelineState &state) noexcept;

		DeviceConstObserverPtr m_device;

		std::unordered_map<GraphicsPipelineState, GraphicsPipelineOwnedPtr>
			m_pipelines;
	};
} // namespace storm::render
