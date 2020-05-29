// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Bindable.hpp>
#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/drawable/Drawable.hpp>
#include <storm/engine/drawable/SubMesh.hpp>

namespace storm::engine {
    struct MeshFlag;
    class STORM_PUBLIC Mesh final: public Drawable, public StaticBindable<MeshFlag> {
      public:
        explicit Mesh(Engine &engine, const Material &material, std::string name = "");
        ~Mesh() override;

        Mesh(Mesh &&);
        Mesh &operator=(Mesh &&);

        void setVertexArray(VertexArray array,
                            render::VertexInputAttributeDescriptionArray attributes,
                            render::VertexBindingDescriptionArray bindings);
        [[nodiscard]] inline const VertexArray &vertexArray() const noexcept;

        void setIndexArray(IndexArrayProxy array);
        [[nodiscard]] inline IndexConstSpanProxy indexArray() const noexcept;
        [[nodiscard]] inline IndexConstSpan indexArrayAsRegular() const noexcept;
        [[nodiscard]] inline LargeIndexConstSpan largeIndexArrayAsLarge() const noexcept;
        [[nodiscard]] inline bool useLargeIndices() const noexcept;

        [[nodiscard]] inline const Material &material() const noexcept;

        [[nodiscard]] inline Transform &transform() noexcept;
        [[nodiscard]] inline const Transform &transform() const noexcept;

        [[nodiscard]] inline SubMesh &addSubmesh(core::UInt32 vertex_count,
                                                 core::UInt32 first_index,
                                                 core::UInt32 index_count,
                                                 core::Vector3f min,
                                                 core::Vector3f max);
        [[nodiscard]] inline SubMeshSpan subMeshes() noexcept;
        [[nodiscard]] inline SubMeshConstSpan subMeshes() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state) override;

        void flush() noexcept;

        inline void setMatrix(core::Matrixf matrix) noexcept;
        [[nodiscard]] inline const core::Matrixf &matrix() const noexcept;

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        std::string m_name;

        MaterialConstObserverPtr m_material;
        TransformOwnedPtr m_transform;

        bool m_dirty_vertices = true;
        render::VertexInputAttributeDescriptionArray m_attributes;
        render::VertexBindingDescriptionArray m_bindings;
        VertexArray m_vertex_array;
        render::HardwareBufferOwnedPtr m_vertex_buffer;

        bool m_dirty_indices = true;
        IndexArrayProxy m_index_array;
        render::HardwareBufferOwnedPtr m_index_buffer;

        SubMeshArray m_submeshes;

        render::CommandBufferOwnedPtr m_update_cmb;

        bool m_matrix_dirty    = true;
        core::Matrixf m_matrix = core::Matrixf { 1.f };
        RingHardwareBufferOwnedPtr m_mesh_data_buffer;
    };
} // namespace storm::engine

#include "Mesh.inl"
