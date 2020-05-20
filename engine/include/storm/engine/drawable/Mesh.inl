// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    const VertexArray &Mesh::vertexArray() const noexcept { return m_vertex_array; }

    /////////////////////////////////////
    /////////////////////////////////////
    IndexConstSpanProxy Mesh::indexArray() const noexcept {
        if (useLargeIndices()) return std::get<LargeIndexArray>(m_index_array);

        return std::get<IndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    IndexConstSpan Mesh::indexArrayAsRegular() const noexcept {
        STORM_EXPECTS(!useLargeIndices());

        return std::get<IndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    LargeIndexConstSpan Mesh::largeIndexArrayAsLarge() const noexcept {
        STORM_EXPECTS(useLargeIndices());

        return std::get<LargeIndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    bool Mesh::useLargeIndices() const noexcept {
        return std::holds_alternative<LargeIndexArray>(m_index_array);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] const Material &Mesh::material() const noexcept { return *m_material; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] Transform &Mesh::transform() noexcept { return *m_transform; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] const Transform &Mesh::transform() const noexcept { return *m_transform; }

    /////////////////////////////////////
    /////////////////////////////////////
    SubMesh &Mesh::addSubmesh(core::UInt32 vertex_count,
                              core::UInt32 first_index,
                              core::UInt32 index_count,
                              core::Vector3f min,
                              core::Vector3f max) {
        auto &submesh = m_submeshes.emplace_back(*m_engine,
                                                 *this,
                                                 vertex_count,
                                                 first_index,
                                                 index_count,
                                                 std::move(min),
                                                 std::move(max));
        return submesh;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    SubMeshSpan Mesh::subMeshes() noexcept { return m_submeshes; }

    /////////////////////////////////////
    /////////////////////////////////////
    SubMeshConstSpan Mesh::subMeshes() const noexcept { return m_submeshes; }
} // namespace storm::engine
