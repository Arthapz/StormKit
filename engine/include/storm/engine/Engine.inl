// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Engine.hpp"

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::setScene(Scene &scene) noexcept { m_scene = core::makeObserver(scene); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline StaticMesh Engine::createStaticMesh(
        render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
        render::VertexBindingDescriptionArray vertex_bindings) {
        return StaticMesh { *this, std::move(vertex_attributes), std::move(vertex_bindings) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline StaticMeshOwnedPtr Engine::createStaticMeshPtr(
        render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
        render::VertexBindingDescriptionArray vertex_bindings) {
        return std::make_unique<StaticMesh>(*this,
                                            std::move(vertex_attributes),
                                            std::move(vertex_bindings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Transform Engine::createTransform() { return Transform { *this }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline TransformOwnedPtr Engine::createTransformPtr() {
        return std::make_unique<Transform>(*this);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Instance &Engine::instance() noexcept { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Instance &Engine::instance() const noexcept { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Device &Engine::device() noexcept { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Device &Engine::device() const noexcept { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Surface &Engine::surface() noexcept { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Surface &Engine::surface() const noexcept { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::DescriptorPool &Engine::descriptorPool() noexcept { return *m_descriptor_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::DescriptorPool &Engine::descriptorPool() const noexcept {
        return *m_descriptor_pool;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::PipelineCache &Engine::pipelineCache() noexcept { return *m_pipeline_cache; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::PipelineCache &Engine::pipelineCache() const noexcept {
        return *m_pipeline_cache;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline DebugGUI &Engine::debugGUI() noexcept { return *m_debug_gui; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const DebugGUI &Engine::debugGUI() const noexcept { return *m_debug_gui; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline float Engine::getCPUTime() const noexcept { return m_cpu_time; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Profiler &Engine::profiler() noexcept { return *m_profiler; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Profiler &Engine::profiler() const noexcept { return *m_profiler; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::SampleCountFlag Engine::maxSampleCount() const noexcept {
        return m_max_sample_count;
    }
} // namespace storm::engine
