// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/scene/Scene.hpp>

namespace storm::engine {
    struct SceneRawData {
        float exposure;
        float gamma;
        float prefiltered_cube_mip_levels;
    };
    class STORM_PUBLIC PBRScene: public Scene {
      public:
        static constexpr auto DEFAULT_PBR_MATERIAL_NAME        = "StormKit:DefaultPBRMaterial";
        static constexpr auto CUBEMAP_MATERIAL_NAME            = "StormKit:CubeMap";
        static constexpr auto FILTER_CUBE_MAP_VERT_SHADER_NAME = "StormKit:FilterCubeMap";
        static constexpr auto IRRADIENCE_FRAG_SHADER_NAME      = "StormKit:GenIrradienceCubeMap";
        static constexpr auto PREFILTERED_ENV_FRAG_SHADER_NAME =
            "StormKit:GenPrefilteredEnvCubeMap";
        static constexpr auto BRDF_VERT_SHADER_NAME = "StormKit:GenBRDFVert";
        static constexpr auto BRDF_FRAG_SHADER_NAME = "StormKit:GenBRDFFrag";

        explicit PBRScene(Engine &engine);
        virtual ~PBRScene();

        PBRScene(PBRScene &&);
        PBRScene &operator=(PBRScene &&);

        [[nodiscard]] inline Mesh createPBRMesh();
        [[nodiscard]] inline MeshOwnedPtr createPBRMeshPtr();

        [[nodiscard]] inline CubeMap createCubeMap();
        [[nodiscard]] inline CubeMapOwnedPtr createCubeMapPtr();

      protected:
        void insertGenerateCubeMapPass(FrameGraph &framegraph, CubeMap &cube_map);

        class SceneData: public Bindable {
          public:
            explicit SceneData(const Engine &);

            void flush(const render::TextureView &,
                       const render::TextureView &,
                       const render::TextureView &,
                       const render::HardwareBuffer &);

          private:
            EngineConstObserverPtr m_engine;

            render::SamplerOwnedPtr m_brdf_sampler;
            render::SamplerOwnedPtr m_irradience_sampler;
            render::SamplerOwnedPtr m_prefiltered_env_sampler;
        };
        std::unique_ptr<SceneData> m_data;

        storm::engine::CubeMapOwnedPtr m_cube_map;
        bool m_cube_map_dirty = true;

      private:
        void generateBRDFLUT(FrameGraph &framegraph, std::string name);
        void generateIrradience(FrameGraph &framegraph, std::string name);
        void generatePrefiltereredEnv(FrameGraph &framegraph, std::string name);

        render::TextureViewOwnedPtr m_gen_cube_view;
        render::SamplerOwnedPtr m_gen_cube_sampler;
        render::DescriptorSetLayoutOwnedPtr m_gen_cube_descriptor_layout;
        render::DescriptorSetOwnedPtr m_gen_cube_descriptor_set;

        render::TextureObserverPtr m_brdf;
        render::TextureViewOwnedPtr m_brdf_view;

        render::TextureObserverPtr m_irradience;
        render::TextureViewOwnedPtr m_irradience_view;

        render::TextureObserverPtr m_prefiltered_env;
        render::TextureViewOwnedPtr m_prefiltered_env_view;

        RingHardwareBufferOwnedPtr m_buffer;
    };
} // namespace storm::engine

#include "PBRScene.inl"
