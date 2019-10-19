// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <unordered_set>

/////////// - StormKit::core - ///////////
#include <storm/core/Pimpl.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>
#include <storm/render/pipeline/VertexBindingDescription.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC PipelineBuilder {
      public:
        explicit PipelineBuilder(Engine &engine);
        ~PipelineBuilder();

        PipelineBuilder(PipelineBuilder &&);
        PipelineBuilder &operator=(PipelineBuilder &&);

        void createPBRPipeline(const StaticMesh &mesh,
                               Material &material,
                               bool msaa                                      = false,
                               std::optional<render::SampleCountFlag> samples = std::nullopt);

        inline const render::DescriptorSetLayout &cameraLayout() const noexcept;
        inline const render::DescriptorSetLayout &transformLayout() const noexcept;
        inline const render::DescriptorSetLayout &pbrMaterialLayout() const noexcept;

        inline const render::GraphicsPipelineLayout &pbrPipelineLayout() const noexcept;

      private:
        void createCommonLayouts();
        void createPBRLayouts();
        void createPBRVertexShader(
            std::string_view name,
            const render::TaggedVertexInputAttributeDescriptionArray &attributes);
        void createPBRFragmentShader(std::string_view name, const Material &mesh_material);

        EngineObserverPtr m_engine;

        render::DescriptorSetLayoutOwnedPtr m_camera_layout;
        render::DescriptorSetLayoutOwnedPtr m_transform_layout;
        render::DescriptorSetLayoutOwnedPtr m_pbr_material_layout;

        render::GraphicsPipelineLayout m_pbr_pipeline_layout;
    };
} // namespace storm::engine

#include "PipelineBuilder.inl"
