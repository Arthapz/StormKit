// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    void MeshNode::addPrimitive(MeshPrimitive &&primitive) {
        m_primitives.emplace_back(std::move(primitive));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::span<const MeshPrimitive> MeshNode::primitives() const noexcept { return m_primitives; }

    /////////////////////////////////////
    /////////////////////////////////////
    void MeshNode::setTransform(core::Matrixf transform) noexcept {
        m_transform = std::move(transform);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Matrixf &MeshNode::transform() const noexcept { return m_transform; }

    /////////////////////////////////////
    /////////////////////////////////////
    std::string_view MeshNode::name() const noexcept { return m_name; }
} // namespace storm::engine
