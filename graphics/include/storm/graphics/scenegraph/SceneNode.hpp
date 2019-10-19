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

#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/Node.hpp>

namespace storm::graphics {
	class STORM_PUBLIC SceneNode final : public Node {
	  public:
		struct NodeData {
			render::mat4 projection_matrix;
		};
		using NodeDataCRef = std::reference_wrapper<const NodeData>;

		~SceneNode() override;

		SceneNode(SceneNode &&);
		SceneNode &operator=(SceneNode &&);

		inline void
			setProjectionMatrix(render::mat4 projection_matrix) noexcept {
			m_data.projection_matrix = std::move(projection_matrix);

			notify(
				NodeEvent::Updated,
				UpdatedPayload{*this, ObjectTreeNode::PROJECTION_DIRTY_BITS});
		}

		inline const render::mat4 &projectionMatrix() const noexcept {
			return m_data.projection_matrix;
		}

		inline const NodeData &data() const noexcept {
			return m_data;
		}

	  protected:
		void visit(SceneVisitorContext &context,
				   SceneRenderer &renderer) const override;

	  private:
		SceneNode();

		NodeData m_data;

		friend class SceneGraph;
		friend class SceneRenderer;
	};
} // namespace storm::graphics
