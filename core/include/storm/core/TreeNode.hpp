// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>

namespace storm::core {
	class STORM_PUBLIC TreeNode {
	  public:
		using IndexType	= std::uint32_t;
		using DirtyBitType = std::uint32_t;

		constexpr static inline const IndexType INVALID_INDEX = ~0U;

		inline std::string_view name() const noexcept;
		inline void setName(std::string name) noexcept;

		inline IndexType parent() const noexcept;
		inline void setParent(IndexType index) noexcept;

		inline IndexType nextSibling() const noexcept;
		inline void setNextSibling(IndexType index) noexcept;

		inline IndexType firstChild() const noexcept;
		inline void setFirstChild(IndexType index) noexcept;

		inline const DirtyBitType &dirtyBits() const noexcept;
		inline void setDirtyBits(DirtyBitType bits) noexcept;

		void invalidate() noexcept;

	  private:
		IndexType m_parent		  = INVALID_INDEX;
		IndexType m_next_sibling  = INVALID_INDEX;
		IndexType m_first_child   = INVALID_INDEX;
		DirtyBitType m_dirty_bits = 0;

		std::string m_name;
	};
} // namespace storm::core

#include "TreeNode.inl"
