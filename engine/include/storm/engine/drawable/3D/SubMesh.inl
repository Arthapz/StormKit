// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    void SubMesh::addPrimitive(MeshPrimitive &&primitive) {
        m_primitives.emplace_back(std::move(primitive));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::span<const MeshPrimitive> SubMesh::primitives() const noexcept { return m_primitives; }

    /////////////////////////////////////
    /////////////////////////////////////
    std::string_view SubMesh::name() const noexcept { return m_name; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::span<MeshPrimitive> SubMesh::primitives() noexcept { return m_primitives; }
} // namespace storm::engine
