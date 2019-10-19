// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/graphics/scenegraph/Node.hpp>
#include <storm/graphics/scenegraph/ObjectTree.hpp>

namespace storm::graphics {
	class STORM_PUBLIC SceneGraphObserver
		: public core::Observer<NodeEvent, NodePayloadVariant>,
		  public core::NonCopyable {
	  public:
		SceneGraphObserver(ObjectTree &object_tree);
		~SceneGraphObserver() override;

		SceneGraphObserver(SceneGraphObserver &&);
		SceneGraphObserver &operator=(SceneGraphObserver &&);

		inline const std::unordered_map<ObjectTreeNode::IndexType, const Node *>
			&nodeLinks() const noexcept {
			return m_node_links;
		}

	  protected:
		void onNotified(NodeEvent event, NodePayloadVariant &&payload) override;

	  private:
		void onChildNodeAdded(ChildAddedPayload &payload);
		void onChildNodeRemoved(ChildRemovedPayload &payload);
		void onUpdated(UpdatedPayload &payload);

		ObjectTreeRef m_object_tree;

		std::unordered_map<ObjectTreeNode::IndexType, const Node *>
			m_node_links;

		friend class SceneRenderer;
	};
} // namespace storm::graphics
