// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PbrMesh::setVertices(VertexArray vertices) -> void {
        m_vertices = std::move(vertices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PbrMesh::setIndices(LargeIndexArray indices) -> void {
        m_indices = std::move(indices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto PbrMesh::vertexSize() const noexcept -> core::ArraySize {
        return sizeof(Vertex);
    }
} // namespace storm::engine
