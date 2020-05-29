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
#include <storm/engine/core/Transform.hpp>

#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/FramePassResourceHandle.hpp>

#include <storm/engine/material/Material.hpp>

#include <storm/engine/drawable/Mesh.hpp>
#include <storm/engine/drawable/Model.hpp>

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

        explicit Engine(const window::Window &window, std::string app_name);
        ~Engine();

        Engine(Engine &&);
        Engine &operator=(Engine &&);

        inline void setScene(Scene &scene) noexcept;

        [[nodiscard]] inline Mesh createMesh(const Material &material);
        [[nodiscard]] inline MeshOwnedPtr createMeshPtr(const Material &material);

        [[nodiscard]] inline Model createModel(TexturePool &texture_pool,
                                               MaterialPool &material_pool);
        [[nodiscard]] inline ModelOwnedPtr createModelPtr(TexturePool &texture_pool,
                                                          MaterialPool &material_pool);

        [[nodiscard]] inline Transform createTransform();
        [[nodiscard]] inline TransformOwnedPtr createTransformPtr();

        void update();
        void render();

        // TODO implement gobal staging buffer

        [[nodiscard]] inline render::Instance &instance() noexcept;
        [[nodiscard]] inline const render::Instance &instance() const noexcept;

        [[nodiscard]] inline render::Device &device() noexcept;
        [[nodiscard]] inline const render::Device &device() const noexcept;

        [[nodiscard]] inline render::Surface &surface() noexcept;
        [[nodiscard]] inline const render::Surface &surface() const noexcept;

        [[nodiscard]] inline render::DescriptorPool &descriptorPool() noexcept;
        [[nodiscard]] inline const render::DescriptorPool &descriptorPool() const noexcept;

        [[nodiscard]] inline render::PipelineCache &pipelineCache() noexcept;
        [[nodiscard]] inline const render::PipelineCache &pipelineCache() const noexcept;

        [[nodiscard]] inline DebugGUI &debugGUI() noexcept;
        [[nodiscard]] inline const DebugGUI &debugGUI() const noexcept;

        [[nodiscard]] inline float getCPUTime() const noexcept;

        [[nodiscard]] inline Profiler &profiler() noexcept;
        [[nodiscard]] inline const Profiler &profiler() const noexcept;

        [[nodiscard]] inline float maxAnisotropy() const noexcept;
        [[nodiscard]] inline render::SampleCountFlag maxSampleCount() const noexcept;

        FramePassTextureID doInitDebugGUIPasses(FramePassTextureID output, FrameGraph &frame_graph);

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

        Clock::time_point m_last_tp;
        float m_cpu_time = 0.f;

        ProfilerOwnedPtr m_profiler;
        DebugGUIOwnedPtr m_debug_gui;

        float m_max_anisotropy                     = 1.f;
        render::SampleCountFlag m_max_sample_count = render::SampleCountFlag::C1_BIT;
    };
} // namespace storm::engine

#include "Engine.inl"
