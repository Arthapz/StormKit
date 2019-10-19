// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <vector>

#include <storm/core/Math.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/Node.hpp>

namespace storm::graphics {
	class STORM_PUBLIC PipelineNode final : public Node {
	  public:
		struct NodeData {
			render::GraphicsPipelineState pipeline_state;
		};
		using NodeDataCRef = std::reference_wrapper<const NodeData>;

		PipelineNode(std::string name = "");
		~PipelineNode() override;

		PipelineNode(PipelineNode &&);
		PipelineNode &operator=(PipelineNode &&);

		inline void setState(render::GraphicsPipelineState state) noexcept {
			m_data.pipeline_state = std::move(state);

			notify(NodeEvent::Updated, UpdatedPayload{*this});
		}

		inline const render::GraphicsPipelineState &state() const noexcept {
			return m_data.pipeline_state;
		}

		inline const NodeData &data() const noexcept {
			return m_data;
		}

	  protected:
		void visit(SceneVisitorContext &context,
				   SceneRenderer &renderer) const override;

	  private:
		NodeData m_data;
	};
} // namespace storm::graphics
