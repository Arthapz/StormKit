// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void Engine::setScene(Scene &scene) noexcept { m_scene = core::makeObserver(scene); }

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
    float Engine::maxAnisotropy() const noexcept { return m_max_anisotropy; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::SampleCountFlag Engine::maxSampleCount() const noexcept { return m_max_sample_count; }
} // namespace storm::engine
