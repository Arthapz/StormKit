// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::setInitFramegraphCallback(Callback &&callback) {
        m_init_framegraph_callback = std::move(callback);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::enableMSAA() noexcept { setMSAAEnabled(true); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::disableMSAA() noexcept { setMSAAEnabled(false); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::toggleMSAA() noexcept { setMSAAEnabled(!isMSAAEnabled()); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::setMSAAEnabled(bool enabled) noexcept { m_is_msaa_enabled = enabled; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool Engine::isMSAAEnabled() const noexcept { return m_is_msaa_enabled; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Engine::setScene(Scene &scene) noexcept { m_scene = core::makeObserver(scene); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline StaticMesh Engine::createStaticMesh(
        render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
        render::VertexBindingDescriptionArray vertex_bindings) const {
        return StaticMesh { *this, std::move(vertex_attributes), std::move(vertex_bindings) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline StaticMeshOwnedPtr Engine::createStaticMeshPtr(
        render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
        render::VertexBindingDescriptionArray vertex_bindings) const {
        return std::make_unique<StaticMesh>(*this,
                                            std::move(vertex_attributes),
                                            std::move(vertex_bindings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Material Engine::createMaterial() const { return Material { *this }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline MaterialOwnedPtr Engine::createMaterialPtr() const {
        return std::make_unique<Material>(*this);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline std::unordered_set<std::string> Engine::pbrPasseNames() const noexcept {
        return { "ColorPass" };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline std::unordered_set<std::string> Engine::debugGUIPasseNames() const noexcept {
        return { "DebugGUI" };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Transform Engine::createTransform() const { return Transform { *this }; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline TransformOwnedPtr Engine::createTransformPtr() const {
        return std::make_unique<Transform>(*this);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Instance &Engine::instance() noexcept { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Device &Engine::device() noexcept { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Surface &Engine::surface() noexcept { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::DescriptorPool &Engine::descriptorPool() noexcept { return *m_descriptor_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Instance &Engine::instance() const noexcept { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Device &Engine::device() const noexcept { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Surface &Engine::surface() const noexcept { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::DescriptorPool &Engine::descriptorPool() const noexcept {
        return *m_descriptor_pool;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Engine::ShaderPool &Engine::shaderPool() noexcept { return m_shader_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Engine::ShaderPool &Engine::shaderPool() const noexcept { return m_shader_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline Engine::TexturePool &Engine::texturePool() noexcept { return m_texture_pool; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const Engine::TexturePool &Engine::texturePool() const noexcept {
        return m_texture_pool;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::PipelineCache &Engine::pipelineCache() noexcept { return *m_pipeline_cache; }

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
    inline PipelineBuilder &Engine::pipelineBuilder() noexcept { return *m_pipeline_builder; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const PipelineBuilder &Engine::pipelineBuilder() const noexcept {
        return *m_pipeline_builder;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::SampleCountFlag Engine::maxSampleCount() const noexcept {
        return m_max_sample_count;
    }
} // namespace storm::engine
