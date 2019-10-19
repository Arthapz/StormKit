// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/graphics/scenegraph/Fwd.hpp>
#include <storm/graphics/scenegraph/SceneNode.hpp>

namespace storm::graphics {
	class STORM_PUBLIC SceneGraph : public core::NonCopyable {
	  public:
		SceneGraph();
		~SceneGraph();

		SceneGraph(SceneGraph &&);
		SceneGraph &operator=(SceneGraph &&);

		template <typename NodeType, typename... Args>
		NodeType makeNode(Args &&... args) noexcept {
			static_assert(std::is_base_of_v<Node, NodeType>,
						  "NodeType need to inherit from Node class");
			static_assert(!std::is_same_v<SceneNode, NodeType>,
						  "NodeType can't be a SceneNode");

			if constexpr (sizeof...(Args) == 0)
				return NodeType{};
			else
				return NodeType{std::forward<Args>(args)...};
		}

		template <typename NodeType, typename... Args>
		std::unique_ptr<NodeType> makeNodeOwnedPtr(Args &&... args) {
			static_assert(std::is_base_of_v<Node, NodeType>,
						  "NodeType need to inherit from Node class");
			static_assert(!std::is_same_v<SceneNode, NodeType>,
						  "NodeType can't be a SceneNode");

			if constexpr (sizeof...(Args) == 0)
				return std::make_unique<NodeType>();
			else
				return std::make_unique<NodeType>(std::forward<Args>(args)...);
		}

		inline SceneNode &rootNode() noexcept {
			return m_root_node;
		}
		inline const SceneNode &rootNode() const noexcept {
			return m_root_node;
		}

		SceneNode m_root_node;
	};
} // namespace storm::graphics
