// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    void StaticSubMesh::setMaterialID(core::UInt32 index) noexcept { m_material_id = index; }

    /////////////////////////////////////
    /////////////////////////////////////
    void StaticSubMesh::setBoundingBox(core::Vector3f min, core::Vector3f max) noexcept {
        m_bounding_box.min = std::move(min);
        m_bounding_box.max = std::move(max);

        const auto extent = m_bounding_box.max - m_bounding_box.min;

        m_bounding_box.extent.width  = extent.x;
        m_bounding_box.extent.height = extent.y;
        m_bounding_box.extent.depth  = extent.z;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void StaticSubMesh::setMatrix(core::Matrixf matrix) noexcept {
        m_matrix = std::move(matrix);
        m_dirty  = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 StaticSubMesh::materialID() const noexcept { return m_material_id; }

    /////////////////////////////////////
    /////////////////////////////////////
    const BoundingBox &StaticSubMesh::boundingBox() const noexcept { return m_bounding_box; }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Matrixf &StaticSubMesh::matrix() const noexcept { return m_matrix; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 StaticSubMesh::vertexCount() const noexcept { return m_vertex_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 StaticSubMesh::indexCount() const noexcept { return m_index_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 StaticSubMesh::firstIndex() const noexcept { return m_first_index; }

    /////////////////////////////////////
    /////////////////////////////////////
    bool StaticSubMesh::isIndexed() const noexcept { return m_index_count > 0u; }
} // namespace storm::engine
