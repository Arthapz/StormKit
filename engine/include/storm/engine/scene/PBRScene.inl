// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    Mesh PBRScene::createPBRMesh() {
        auto &default_pbr_material = m_material_pool.get(DEFAULT_PBR_MATERIAL_NAME);

        return Mesh { *m_engine, *default_pbr_material };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    MeshOwnedPtr PBRScene::createPBRMeshPtr() {
        auto &default_pbr_material = m_material_pool.get(DEFAULT_PBR_MATERIAL_NAME);

        return std::make_unique<Mesh>(*m_engine, *default_pbr_material);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    CubeMap PBRScene::createCubeMap() { return CubeMap { *this }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline CubeMapOwnedPtr PBRScene::createCubeMapPtr() { return std::make_unique<CubeMap>(*this); }
} // namespace storm::engine
