// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Scene.hpp"

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Scene::setCamera(Camera &camera) noexcept { m_camera = core::makeObserver(camera); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] inline Camera &Scene::camera() noexcept { return *m_camera; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] inline const Camera &Scene::camera() const noexcept { return *m_camera; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Scene::enableDepthTest(bool enable) noexcept {
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
    inline void Scene::setMSAASampleCount(render::SampleCountFlag samples) noexcept {
        m_pipeline_state.multisample_state.sample_shading_enable =
            samples != render::SampleCountFlag::C1_BIT;
        m_pipeline_state.multisample_state.rasterization_samples = samples;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool Scene::isMSAAEnabled() const noexcept {
        return m_pipeline_state.multisample_state.sample_shading_enable;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Scene::ShaderPool &Scene::shaderPool() noexcept { return m_shader_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Scene::ShaderPool &Scene::shaderPool() const noexcept { return m_shader_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Scene::TexturePool &Scene::texturePool() noexcept { return m_texture_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Scene::TexturePool &Scene::texturePool() const noexcept { return m_texture_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Scene::MaterialPool &Scene::materialPool() noexcept { return m_material_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Scene::MaterialPool &Scene::materialPool() const noexcept {
        return m_material_pool;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline entities::EntityManager &Scene::entities() noexcept { return m_entities; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Engine &Scene::engine() noexcept { return *m_engine; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Engine &Scene::engine() const noexcept { return *m_engine; }
} // namespace storm::engine
