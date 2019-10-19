// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void StaticMesh::setInstanceCount(core::UInt32 count) {
        STORM_EXPECTS(count >= 1u);

        m_instance_count = count;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 StaticMesh::instanceCount() const noexcept { return m_instance_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const render::TaggedVertexInputAttributeDescriptionArray &
        StaticMesh::taggedVertexAttributes() const noexcept {
        return m_tagged_vertex_attributes;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline render::VertexInputAttributeDescriptionConstSpan StaticMesh::vertexAttributes() const
        noexcept {
        return m_vertex_attributes;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline render::VertexBindingDescriptionConstSpan StaticMesh::vertexBindings() const noexcept {
        return m_vertex_bindings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const VertexArray &StaticMesh::vertexArray() const noexcept { return m_vertex_array; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline IndexConstSpanProxy StaticMesh::indexArray() const noexcept {
        if (useLargeIndices()) return std::get<LargeIndexArray>(m_index_array);

        return std::get<IndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline IndexConstSpan StaticMesh::indexArrayAsRegular() const noexcept {
        STORM_EXPECTS(!useLargeIndices());

        return std::get<IndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline LargeIndexConstSpan StaticMesh::largeIndexArrayAsLarge() const noexcept {
        STORM_EXPECTS(useLargeIndices());

        return std::get<LargeIndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool StaticMesh::useLargeIndices() const noexcept {
        return std::holds_alternative<LargeIndexArray>(m_index_array);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool StaticMesh::hasIndices() const noexcept { return m_index_buffer != nullptr; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const render::HardwareBuffer &StaticMesh::vertexBuffer() const noexcept {
        STORM_EXPECTS(m_vertex_buffer != nullptr);

        return *m_vertex_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const render::HardwareBuffer &StaticMesh::indexBuffer() const noexcept {
        STORM_EXPECTS(hasIndices());

        return *m_index_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline StaticSubMesh &StaticMesh::addSubmesh(core::UInt32 vertex_count,
                                                 core::UInt32 first_index,
                                                 core::UInt32 index_count) {
        auto &submesh = m_submeshes.emplace_back(vertex_count, first_index, index_count);
        return submesh;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline StaticSubMeshConstSpan StaticMesh::subMeshes() const noexcept { return m_submeshes; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const BoundingBox &StaticMesh::boundingBox() const noexcept {
        if (!m_bounding_box_computed) {
            computeBoundingBox();

            m_bounding_box_computed = true;
        }

        return m_bounding_box;
    }
} // namespace storm::engine
