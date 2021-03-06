// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    std::string_view TreeNode::name() const noexcept { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void TreeNode::setName(std::string name) noexcept { m_name = std::move(name); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    TreeNode::IndexType TreeNode::parent() const noexcept { return m_parent; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void TreeNode::setParent(IndexType index) noexcept { m_parent = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    TreeNode::IndexType TreeNode::nextSibling() const noexcept { return m_next_sibling; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void TreeNode::setNextSibling(IndexType index) noexcept { m_next_sibling = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    TreeNode::IndexType TreeNode::firstChild() const noexcept { return m_first_child; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void TreeNode::setFirstChild(IndexType index) noexcept { m_first_child = index; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const TreeNode::DirtyBitType &TreeNode::dirtyBits() const noexcept { return m_dirty_bits; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void TreeNode::setDirtyBits(DirtyBitType bits) noexcept { m_dirty_bits = bits; }
} // namespace storm::core
