// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <unordered_set>
#include <utility>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/ResourcesPool.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/DebugGUI.hpp>
#include <storm/engine/core/PipelineBuilder.hpp>
#include <storm/engine/core/Transform.hpp>

#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/FramePassResourceHandle.hpp>

#include <storm/engine/material/Material.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>

namespace storm::engine {
    class STORM_PUBLIC Engine {
      public:
        static constexpr auto PHONG_PASS_NAME    = "StormKit:PhongPass";
        static constexpr auto RENDER2D_PASS_NAME = "StormKit:Render2D";
        static constexpr auto POSTFX_PASS_NAME   = "StormKit:PostFX";

        struct PhongPassData {
            storm::engine::FramePassTextureID output;
            storm::engine::FramePassTextureID depth_buffer;
        };

        struct Render2DPassData {
            storm::engine::FramePassTextureID output;
        };

        struct PostFXPassData {
            storm::engine::FramePassTextureID input;
            storm::engine::FramePassTextureID output;
        };

        using Callback = std::function<void(FramePassTextureID &, FrameGraph &)>;

        using ShaderPool  = core::ResourcesPool<std::string, render::Shader>;
        using TexturePool = core::ResourcesPool<std::string, render::Texture>;

        explicit Engine(const window::Window &window);
        ~Engine();

        Engine(Engine &&);
        Engine &operator=(Engine &&);

        inline void setInitFramegraphCallback(Callback &&callback);

        inline void enableMSAA() noexcept;
        inline void disableMSAA() noexcept;
        inline void toggleMSAA() noexcept;
        inline void setMSAAEnabled(bool enabled) noexcept;

        inline bool isMSAAEnabled() const noexcept;

        inline void setScene(Scene &scene) noexcept;

        [[nodiscard]] inline StaticMesh
            createStaticMesh(render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
                             render::VertexBindingDescriptionArray vertex_bindings) const;
        [[nodiscard]] inline StaticMeshOwnedPtr createStaticMeshPtr(
            render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
            render::VertexBindingDescriptionArray vertex_bindings) const;

        [[nodiscard]] inline Material createMaterial() const;
        [[nodiscard]] inline MaterialOwnedPtr createMaterialPtr() const;

        [[nodiscard]] inline Transform createTransform() const;
        [[nodiscard]] inline TransformOwnedPtr createTransformPtr() const;

        void update();
        void render();

        FramePassTextureID doInitPBRPasses(FramePassTextureID output, FrameGraph &frame_graph);
        [[nodiscard]] inline std::unordered_set<std::string> pbrPasseNames() const noexcept;

        FramePassTextureID doInitDebugGUIPasses(FramePassTextureID output, FrameGraph &frame_graph);
        [[nodiscard]] inline std::unordered_set<std::string> debugGUIPasseNames() const noexcept;

        // TODO implement gobal staging buffer

        [[nodiscard]] inline render::Instance &instance() noexcept;
        [[nodiscard]] inline render::Device &device() noexcept;
        [[nodiscard]] inline render::Surface &surface() noexcept;
        [[nodiscard]] inline render::DescriptorPool &descriptorPool() noexcept;
        [[nodiscard]] inline const render::Instance &instance() const noexcept;
        [[nodiscard]] inline const render::Device &device() const noexcept;
        [[nodiscard]] inline const render::Surface &surface() const noexcept;
        [[nodiscard]] inline const render::DescriptorPool &descriptorPool() const noexcept;

        [[nodiscard]] inline ShaderPool &shaderPool() noexcept;
        [[nodiscard]] inline const ShaderPool &shaderPool() const noexcept;

        [[nodiscard]] inline TexturePool &texturePool() noexcept;
        [[nodiscard]] inline const TexturePool &texturePool() const noexcept;

        [[nodiscard]] inline render::PipelineCache &pipelineCache() noexcept;

        [[nodiscard]] inline DebugGUI &debugGUI() noexcept;
        [[nodiscard]] inline const DebugGUI &debugGUI() const noexcept;

        [[nodiscard]] inline float getCPUTime() const noexcept;

        [[nodiscard]] inline Profiler &profiler() noexcept;
        [[nodiscard]] inline const Profiler &profiler() const noexcept;

        [[nodiscard]] inline PipelineBuilder &pipelineBuilder() noexcept;
        [[nodiscard]] inline const PipelineBuilder &pipelineBuilder() const noexcept;

        [[nodiscard]] inline render::SampleCountFlag maxSampleCount() const noexcept;

      private:
        using Clock = std::chrono::high_resolution_clock;

        render::InstanceOwnedPtr m_instance;
        render::DeviceOwnedPtr m_device;
        render::SurfaceOwnedPtr m_surface;
        render::QueueConstObserverPtr m_queue;
        render::PipelineCacheOwnedPtr m_pipeline_cache;

        std::vector<FrameGraphOwnedPtr> m_framegraphs;
        FrameGraphTexture::Descriptor m_backbuffer_descriptor;

        render::CommandBufferArray m_command_buffers;

        bool m_is_msaa_enabled = true;

        engine::SceneObserverPtr m_scene;

        render::DescriptorPoolOwnedPtr m_descriptor_pool;

        Callback m_init_framegraph_callback;

        ShaderPool m_shader_pool;
        TexturePool m_texture_pool;

        Clock::time_point m_last_tp;
        float m_cpu_time = 0.f;

        ProfilerOwnedPtr m_profiler;
        DebugGUIOwnedPtr m_debug_gui;

        PipelineBuilderOwnedPtr m_pipeline_builder;

        render::SampleCountFlag m_max_sample_count = render::SampleCountFlag::C1_BIT;
    };
} // namespace storm::engine

#include "Engine.inl"
