// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <bitset>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/TreeNode.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/VertexBindingDescription.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/drawable/BoundingBox.hpp>
#include <storm/engine/drawable/Drawable.hpp>

#include <storm/engine/material/MaterialInstance.hpp>

namespace storm::engine {
    struct MeshPrimitive {
        const std::string name;

        VertexArray vertices;
        IndexArrayProxy indices;

        render::VertexInputAttributeDescriptionArray attributes;
        render::VertexBindingDescriptionArray bindings;

        std::uint32_t first_vertex = 0u;
        std::uint32_t vertex_count = 0u;

        std::uint32_t first_index = 0u;
        std::uint32_t index_count = 0u;
        bool large_indices        = false;

        BoundingBox bounding_box;

        MaterialInstanceOwnedPtr material_instance;
    };

    class SubMeshFlag;
    class STORM_PUBLIC SubMesh: public Drawable {
      public:
        explicit SubMesh(Engine &engine, std::string name = "");
        ~SubMesh();

        SubMesh(SubMesh &&);
        SubMesh &operator=(SubMesh &&);

        inline void addPrimitive(MeshPrimitive &&primitive);
        [[nodiscard]] inline core::span<const MeshPrimitive> primitives() const noexcept;

        [[nodiscard]] inline std::string_view name() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state,
                    float delta_time = 0.f) override;

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        [[nodiscard]] inline core::span<MeshPrimitive> primitives() noexcept;

        std::string m_name;

        std::vector<MeshPrimitive> m_primitives;

        friend class Mesh;
    };
} // namespace storm::engine

#include "SubMesh.inl"
