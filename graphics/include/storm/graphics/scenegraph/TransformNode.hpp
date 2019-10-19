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
	class STORM_PUBLIC TransformNode final : public Node {
	  public:
		struct NodeData {
			render::mat4 transform = render::mat4{1.f};
		};
		using NodeDataCRef = std::reference_wrapper<const NodeData>;

		TransformNode(std::string name = "");
		~TransformNode() override;

		TransformNode(TransformNode &&);
		TransformNode &operator=(TransformNode &&);

		inline void setTransform(render::mat4 transform) noexcept {
			m_data.transform = std::move(transform);

			notify(NodeEvent::Updated,
				   UpdatedPayload{*this, ObjectTreeNode::TRANSFORM_DIRTY_BITS});
		}

		inline const render::mat4 &transform() const noexcept {
			return m_data.transform;
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
