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

#include <storm/engine/drawable/Drawable.hpp>

namespace storm::engine {
    class STORM_PUBLIC SubMesh final: public Drawable {
      public:
        SubMesh(Engine &engine,
                Mesh &parent,
                core::UInt32 vertex_count,
                core::UInt32 first_index,
                core::UInt32 index_count,
                core::Vector3f min,
                core::Vector3f max);
        ~SubMesh() override;

        SubMesh(SubMesh &&);
        SubMesh &operator=(SubMesh &&);

        inline void setMatrix(core::Matrixf matrix) noexcept;
        [[nodiscard]] const core::Matrixf &matrix() const noexcept;

        [[nodiscard]] inline MaterialInstance &materialInstance() noexcept;
        [[nodiscard]] inline const MaterialInstance &materialInstance() const noexcept;

        [[nodiscard]] inline core::UInt32 vertexCount() const noexcept;
        [[nodiscard]] inline core::UInt32 indexCount() const noexcept;
        [[nodiscard]] inline core::UInt32 firstIndex() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state) override;

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        MeshObserverPtr m_parent_mesh;

        MaterialInstanceOwnedPtr m_material_instance;

        core::Matrixf m_matrix = core::Matrixf { 1.f };

        core::UInt32 m_vertex_count = 0u;
        core::UInt32 m_first_index  = 0u;
        core::UInt32 m_index_count  = 0u;
    };
} // namespace storm::engine

#include "SubMesh.inl"
