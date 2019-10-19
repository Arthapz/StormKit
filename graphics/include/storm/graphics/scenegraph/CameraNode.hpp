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
	class STORM_PUBLIC CameraNode final : public Node {
	  public:
		struct NodeData {
			render::mat4 view_matrix;
		};
		using NodeDataCRef = std::reference_wrapper<const NodeData>;

		CameraNode(std::string name = "");
		~CameraNode() override;

		CameraNode(CameraNode &&);
		CameraNode &operator=(CameraNode &&);

		inline void setViewMatrix(render::mat4 view_matrix) noexcept {
			m_data.view_matrix = std::move(view_matrix);

			notify(NodeEvent::Updated,
				   UpdatedPayload{*this, ObjectTreeNode::VIEW_DIRTY_BITS});
		}

		inline const render::mat4 &viewMatrix() const noexcept {
			return m_data.view_matrix;
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
