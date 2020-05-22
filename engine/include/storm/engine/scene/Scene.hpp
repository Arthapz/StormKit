// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <unordered_map>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/ResourcesPool.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Shader.hpp>
#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/CubeMap.hpp>
#include <storm/engine/drawable/Mesh.hpp>

#include <storm/engine/scene/Camera.hpp>

namespace storm::engine {
    class STORM_PUBLIC Scene: public core::NonCopyable {
      public:
        static constexpr auto DEFAULT_PBR_MATERIAL_NAME = "StormKit:DefaultPBRMaterial";
        static constexpr auto CUBEMAP_MATERIAL_NAME     = "StormKit:CubeMap";

        explicit Scene(engine::Engine &engine);
        virtual ~Scene();

        Scene(Scene &&);
        Scene &operator=(Scene &&);

        [[nodiscard]] inline Mesh createPBRMesh();
        [[nodiscard]] inline MeshOwnedPtr createPBRMeshPtr();

        [[nodiscard]] inline Model createModel();
        [[nodiscard]] inline ModelOwnedPtr createModelPtr();

        [[nodiscard]] inline CubeMap createCubeMap();
        [[nodiscard]] inline CubeMapOwnedPtr createCubeMapPtr();

        void update(float delta) noexcept;
        void render(FrameGraph &framegraph, storm::engine::FramePassTextureID backbuffer) noexcept;

        inline void setCamera(Camera &camera) noexcept;
        [[nodiscard]] inline Camera &camera() noexcept;
        [[nodiscard]] inline const Camera &camera() const noexcept;

        inline void enableDepthTest(bool enable) noexcept;
        [[nodiscard]] inline bool isDepthTestEnabled() const noexcept;

        inline void setMSAASampleCount(render::SampleCountFlag samples) noexcept;
        [[nodiscard]] inline bool isMSAAEnabled() const noexcept;

        [[nodiscard]] inline ShaderPool &shaderPool() noexcept;
        [[nodiscard]] inline const ShaderPool &shaderPool() const noexcept;

        [[nodiscard]] inline TexturePool &texturePool() noexcept;
        [[nodiscard]] inline const TexturePool &texturePool() const noexcept;

        [[nodiscard]] inline MaterialPool &materialPool() noexcept;
        [[nodiscard]] inline const MaterialPool &materialPool() const noexcept;

        [[nodiscard]] inline Engine &engine() noexcept;
        [[nodiscard]] inline const Engine &engine() const noexcept;

      protected:
        virtual void doRenderScene(FrameGraph &framegraph,
                                   FramePassTextureID backbuffer,
                                   std::vector<BindableBaseConstObserverPtr> bindables,
                                   render::GraphicsPipelineState &state) = 0;

        CameraOwnedPtr m_default_camera;

        EngineObserverPtr m_engine;

        ShaderPool m_shader_pool;
        TexturePool m_texture_pool;
        MaterialPool m_material_pool;

        CameraObserverPtr m_camera;

        render::GraphicsPipelineState m_pipeline_state;
    };
} // namespace storm::engine

#include "Scene.inl"
