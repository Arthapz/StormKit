// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <gsl/gsl_assert>
#include <storm/core/Span.hpp>

#include <storm/core/Filesystem.hpp>
#include <storm/core/ForwardDeclarations.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/TreeNode.hpp>

namespace storm::core {
	template <typename TreeNodeClass> class Tree final {
	  public:
		constexpr static const std::size_t DEFAULT_PREALLOCATED_TREE_SIZE =
			1000;

		using TreeNodeType		   = TreeNodeClass;
		using TreeNodeIndexType	= typename TreeNodeType::IndexType;
		using TreeNodeDirtyBitType = typename TreeNodeType::DirtyBitType;

		explicit Tree();
		~Tree();

		Tree(const Tree &);
		Tree(Tree &&);
		Tree &operator=(const Tree &);
		Tree &operator=(Tree &&);

		TreeNodeIndexType getFreeNode();

		TreeNodeIndexType insert(TreeNodeType &&node,
								 TreeNodeIndexType parent_index,
								 TreeNodeIndexType previous_sibling);
		void remove(TreeNodeIndexType index);

		void markDirty(TreeNodeIndexType index, TreeNodeDirtyBitType bits);

		inline const TreeNodeType &operator[](TreeNodeIndexType index) const
			noexcept;
		inline TreeNodeType &operator[](TreeNodeIndexType index) noexcept;

		inline std::size_t size() const noexcept;

		inline auto begin() noexcept;
		inline auto begin() const noexcept;
		inline auto cbegin() const noexcept;

		inline auto end() noexcept;
		inline auto end() const noexcept;
		inline auto cend() const noexcept;

		inline void clearDirties() noexcept;
		inline storm::core::span<const TreeNodeIndexType> dirties() const
			noexcept;

		void genDotFile(filesystem::path filepath,
						std::function<std::string_view(std::string_view)>
							colorize_node) const;

		void genDotFile(filesystem::path filepath, std::uint32_t highlight,
						std::function<std::string_view(std::string_view)>
							colorize_node) const;

	  private:
		TreeNodeIndexType m_first_free_index = 0;
		std::vector<TreeNodeType> m_tree;
		std::vector<TreeNodeIndexType> m_dirties;
	};
} // namespace storm::core

#include "Tree.inl"
