// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    void StaticMesh::setMaterialInstances(std::vector<MaterialInstanceOwnedPtr> instances) {
        m_material_instances = std::move(instances);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    std::vector<MaterialInstanceObserverPtr> StaticMesh::materialInstances() noexcept {
        auto vec = std::vector<MaterialInstanceObserverPtr> {};
        vec.reserve(std::size(m_material_instances));

        for (auto &ptr : m_material_instances) vec.emplace_back(ptr.get());

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Transform &StaticMesh::transform() noexcept { return *m_transform; }

    /////////////////////////////////////
    /////////////////////////////////////
    const Transform &StaticMesh::transform() const noexcept { return *m_transform; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::span<const MaterialInstanceOwnedPtr> StaticMesh::materialInstances() const noexcept {
        return m_material_instances;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void StaticMesh::setInstanceCount(core::UInt32 count) {
        STORM_EXPECTS(count >= 1u);

        m_instance_count = count;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 StaticMesh::instanceCount() const noexcept { return m_instance_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    const VertexArray &StaticMesh::vertexArray() const noexcept { return m_vertex_array; }

    /////////////////////////////////////
    /////////////////////////////////////
    IndexConstSpanProxy StaticMesh::indexArray() const noexcept {
        if (useLargeIndices()) return std::get<LargeIndexArray>(m_index_array);

        return std::get<IndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    IndexConstSpan StaticMesh::indexArrayAsRegular() const noexcept {
        STORM_EXPECTS(!useLargeIndices());

        return std::get<IndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    LargeIndexConstSpan StaticMesh::largeIndexArrayAsLarge() const noexcept {
        STORM_EXPECTS(useLargeIndices());

        return std::get<LargeIndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    bool StaticMesh::useLargeIndices() const noexcept {
        return std::holds_alternative<LargeIndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    bool StaticMesh::hasIndices() const noexcept { return m_index_buffer != nullptr; }

    /////////////////////////////////////
    /////////////////////////////////////
    const render::HardwareBuffer &StaticMesh::vertexBuffer() const noexcept {
        STORM_EXPECTS(m_vertex_buffer != nullptr);

        return *m_vertex_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const render::HardwareBuffer &StaticMesh::indexBuffer() const noexcept {
        STORM_EXPECTS(hasIndices());

        return *m_index_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    StaticSubMesh &StaticMesh::addSubmesh(core::UInt32 vertex_count,
                                          core::UInt32 first_index,
                                          core::UInt32 index_count) {
        auto &submesh = m_submeshes.emplace_back(*m_engine, vertex_count, first_index, index_count);
        return submesh;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    StaticSubMeshConstSpan StaticMesh::subMeshes() const noexcept { return m_submeshes; }

    /////////////////////////////////////
    /////////////////////////////////////
    const BoundingBox &StaticMesh::boundingBox() const noexcept {
        if (!m_bounding_box_computed) {
            computeBoundingBox();

            m_bounding_box_computed = true;
        }

        return m_bounding_box;
    }
} // namespace storm::engine
