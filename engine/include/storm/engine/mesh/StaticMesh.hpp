// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <unordered_map>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/mesh/BoundingBox.hpp>
#include <storm/engine/mesh/StaticSubMesh.hpp>

#include <storm/engine/material/MaterialInstance.hpp>

namespace storm::engine {
    class STORM_PUBLIC StaticMesh {
      public:
        explicit StaticMesh(Engine &engine,
                            render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
                            render::VertexBindingDescriptionArray vertex_bindings) noexcept;
        ~StaticMesh();

        StaticMesh(StaticMesh &&);
        StaticMesh &operator=(StaticMesh &&);

        void setVertexArray(VertexArray array);
        void setIndexArray(IndexArrayProxy array);

        inline void setMaterialInstances(std::vector<MaterialInstanceOwnedPtr> instances);
        [[nodiscard]] inline std::vector<MaterialInstanceObserverPtr> materialInstances() noexcept;
        [[nodiscard]] inline core::span<const MaterialInstanceOwnedPtr>
            materialInstances() const noexcept;

        [[nodiscard]] inline Transform &transform() noexcept;
        [[nodiscard]] inline const Transform &transform() const noexcept;

        inline void setInstanceCount(core::UInt32 count);
        [[nodiscard]] inline core::UInt32 instanceCount() const noexcept;

        [[nodiscard]] inline const VertexArray &vertexArray() const noexcept;

        [[nodiscard]] inline IndexConstSpanProxy indexArray() const noexcept;
        [[nodiscard]] inline IndexConstSpan indexArrayAsRegular() const noexcept;
        [[nodiscard]] inline LargeIndexConstSpan largeIndexArrayAsLarge() const noexcept;
        [[nodiscard]] inline bool useLargeIndices() const noexcept;

        [[nodiscard]] inline bool hasIndices() const noexcept;

        [[nodiscard]] inline const render::HardwareBuffer &vertexBuffer() const noexcept;
        [[nodiscard]] inline const render::HardwareBuffer &indexBuffer() const noexcept;

        [[nodiscard]] inline StaticSubMesh &addSubmesh(core::UInt32 vertex_count,
                                                       core::UInt32 first_index,
                                                       core::UInt32 index_count);
        [[nodiscard]] inline StaticSubMeshConstSpan subMeshes() const noexcept;

        inline const BoundingBox &boundingBox() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state);

      private:
        void computeBoundingBox() const noexcept;

        EngineObserverPtr m_engine;

        render::GraphicsPipelineVertexInputState m_vertex_input_state;
        render::GraphicsPipelineInputAssemblyState m_input_assembly_state;
        render::VertexInputAttributeDescriptionArray m_vertex_attributes;
        render::VertexBindingDescriptionArray m_vertex_bindings;

        core::UInt32 m_instance_count = 1;

        VertexArray m_vertex_array;
        render::HardwareBufferOwnedPtr m_vertex_buffer;

        IndexArrayProxy m_index_array;
        render::HardwareBufferOwnedPtr m_index_buffer;

        StaticSubMeshArray m_submeshes;

        std::vector<MaterialInstanceOwnedPtr> m_material_instances;

        TransformOwnedPtr m_transform;

        mutable BoundingBox m_bounding_box;
        mutable bool m_bounding_box_computed = false;
    };
} // namespace storm::engine

#include "StaticMesh.inl"
