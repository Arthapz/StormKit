// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    MaterialInstance &SubMesh::materialInstance() noexcept { return *m_material_instance; }

    /////////////////////////////////////
    /////////////////////////////////////
    const MaterialInstance &SubMesh::materialInstance() const noexcept {
        return *m_material_instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 SubMesh::vertexCount() const noexcept { return m_vertex_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 SubMesh::indexCount() const noexcept { return m_index_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 SubMesh::firstIndex() const noexcept { return m_first_index; }
} // namespace storm::engine
