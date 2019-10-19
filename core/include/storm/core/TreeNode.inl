// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline std::string_view TreeNode::name() const noexcept { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void TreeNode::setName(std::string name) noexcept { m_name = std::move(name); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline TreeNode::IndexType TreeNode::parent() const noexcept { return m_parent; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void TreeNode::setParent(IndexType index) noexcept { m_parent = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline TreeNode::IndexType TreeNode::nextSibling() const noexcept { return m_next_sibling; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void TreeNode::setNextSibling(IndexType index) noexcept { m_next_sibling = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline TreeNode::IndexType TreeNode::firstChild() const noexcept { return m_first_child; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void TreeNode::setFirstChild(IndexType index) noexcept { m_first_child = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const TreeNode::DirtyBitType &TreeNode::dirtyBits() const noexcept { return m_dirty_bits; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void TreeNode::setDirtyBits(DirtyBitType bits) noexcept { m_dirty_bits = bits; }
} // namespace storm::tools
