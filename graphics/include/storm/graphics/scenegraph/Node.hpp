// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#include <variant>
#include <vector>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Subject.hpp>
#include <storm/core/TreeNode.hpp>

#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/SceneRenderer.hpp>

namespace storm::graphics {
	enum class NodeEvent { ChildAdded, ChildRemoved, Updated };
	using NodeVariant =
		std::variant<CameraNodeRef, MaterialNodeRef, PipelineNodeRef,
					 SceneNodeRef, StaticGeometryNodeRef, TransformNodeRef>;

	struct ChildAddedPayload {
		NodeRef parent;
		NodeRef child;
	};
	struct ChildRemovedPayload {
		NodeRef parent;
		NodeRef child;
	};
	struct UpdatedPayload {
		NodeRef node;
		ObjectTreeNode::DirtyBitType dirty_bits =
			ObjectTreeNode::DATA_UPDATE_DIRTY_BITS;
	};
	using NodePayloadVariant =
		std::variant<ChildAddedPayload, ChildRemovedPayload, UpdatedPayload>;

	using NodeSubject = core::Subject<NodeEvent, NodePayloadVariant, true>;

	class STORM_PUBLIC Node : public core::NonCopyable, protected NodeSubject {
	  public:
		virtual ~Node();

		Node(Node &&);
		Node &operator=(Node &&);

		void addChild(Node &child);
		void removeChild(Node &child, bool all = true);

		inline void setName(std::string name) noexcept {
			m_name = std::move(name);
		}
		inline std::string_view name() const noexcept {
			return m_name;
		}
		inline std::string_view identifier() const noexcept {
			return m_identifier;
		}

	  protected:
		Node(std::string identifier);

		void addParent(const Node &parent);
		void removeParent(const Node &parent, bool all);

		void setObserver(NodeSubject::ObserverType &graph);

		virtual void visit(SceneVisitorContext &context,
						   SceneRenderer &renderer) const = 0;

		std::string m_identifier;
		std::string m_name = "";

		std::vector<NodeCRef> m_parents;
		std::vector<NodeRef> m_childs;

		friend class SceneRenderer;
	};
} // namespace storm::graphics
