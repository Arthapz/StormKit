// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Scene.hpp"

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    Mesh Scene::createPBRMesh() {
        auto &default_pbr_material = m_material_pool.get(DEFAULT_PBR_MATERIAL_NAME);

        return Mesh { *m_engine, *default_pbr_material };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    MeshOwnedPtr Scene::createPBRMeshPtr() {
        auto &default_pbr_material = m_material_pool.get(DEFAULT_PBR_MATERIAL_NAME);

        return std::make_unique<Mesh>(*m_engine, *default_pbr_material);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Model Scene::createModel() { return m_engine->createModel(m_texture_pool, m_material_pool); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline ModelOwnedPtr Scene::createModelPtr() {
        return m_engine->createModelPtr(m_texture_pool, m_material_pool);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    CubeMap Scene::createCubeMap() { return CubeMap { *this }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline CubeMapOwnedPtr Scene::createCubeMapPtr() { return std::make_unique<CubeMap>(*this); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void Scene::setCamera(Camera &camera) noexcept { m_camera = core::makeObserver(camera); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Camera &Scene::camera() noexcept { return *m_camera; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const Camera &Scene::camera() const noexcept { return *m_camera; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void Scene::enableDepthTest(bool enable) noexcept {
        m_pipeline_state.depth_stencil_state.depth_write_enable = enable;
        m_pipeline_state.depth_stencil_state.depth_test_enable  = enable;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Scene::isDepthTestEnabled() const noexcept {
        return m_pipeline_state.depth_stencil_state.depth_test_enable;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void Scene::setMSAASampleCount(render::SampleCountFlag samples) noexcept {
        m_pipeline_state.multisample_state.sample_shading_enable =
            samples != render::SampleCountFlag::C1_BIT;
        m_pipeline_state.multisample_state.rasterization_samples = samples;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Scene::isMSAAEnabled() const noexcept {
        return m_pipeline_state.multisample_state.sample_shading_enable;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ShaderPool &Scene::shaderPool() noexcept { return m_shader_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const ShaderPool &Scene::shaderPool() const noexcept { return m_shader_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    TexturePool &Scene::texturePool() noexcept { return m_texture_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const TexturePool &Scene::texturePool() const noexcept { return m_texture_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    MaterialPool &Scene::materialPool() noexcept { return m_material_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const MaterialPool &Scene::materialPool() const noexcept { return m_material_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Engine &Scene::engine() noexcept { return *m_engine; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const Engine &Scene::engine() const noexcept { return *m_engine; }
} // namespace storm::engine
