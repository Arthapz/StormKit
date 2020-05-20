// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Engine.hpp"

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void Engine::setScene(Scene &scene) noexcept { m_scene = core::makeObserver(scene); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    StaticMesh Engine::createStaticMesh(
        render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
        render::VertexBindingDescriptionArray vertex_bindings) {
        return StaticMesh { *this, std::move(vertex_attributes), std::move(vertex_bindings) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    StaticMeshOwnedPtr Engine::createStaticMeshPtr(
        render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
        render::VertexBindingDescriptionArray vertex_bindings) {
        return std::make_unique<StaticMesh>(*this,
                                            std::move(vertex_attributes),
                                            std::move(vertex_bindings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Mesh Engine::createMesh(const Material &material) { return Mesh { *this, material }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    MeshOwnedPtr Engine::createMeshPtr(const Material &material) {
        return std::make_unique<Mesh>(*this, material);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    v2::Model Engine::createModel(TexturePool &texture_pool, MaterialPool &material_pool) {
        return v2::Model { *this, texture_pool, material_pool };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    v2::ModelOwnedPtr Engine::createModelPtr(TexturePool &texture_pool,
                                             MaterialPool &material_pool) {
        return std::make_unique<v2::Model>(*this, texture_pool, material_pool);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Transform Engine::createTransform() { return Transform { *this }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    TransformOwnedPtr Engine::createTransformPtr() { return std::make_unique<Transform>(*this); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::Instance &Engine::instance() noexcept { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::Instance &Engine::instance() const noexcept { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::Device &Engine::device() noexcept { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::Device &Engine::device() const noexcept { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::Surface &Engine::surface() noexcept { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::Surface &Engine::surface() const noexcept { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::DescriptorPool &Engine::descriptorPool() noexcept { return *m_descriptor_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::DescriptorPool &Engine::descriptorPool() const noexcept {
        return *m_descriptor_pool;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::PipelineCache &Engine::pipelineCache() noexcept { return *m_pipeline_cache; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::PipelineCache &Engine::pipelineCache() const noexcept {
        return *m_pipeline_cache;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    DebugGUI &Engine::debugGUI() noexcept { return *m_debug_gui; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const DebugGUI &Engine::debugGUI() const noexcept { return *m_debug_gui; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    float Engine::getCPUTime() const noexcept { return m_cpu_time; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Profiler &Engine::profiler() noexcept { return *m_profiler; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const Profiler &Engine::profiler() const noexcept { return *m_profiler; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::SampleCountFlag Engine::maxSampleCount() const noexcept { return m_max_sample_count; }
} // namespace storm::engine
