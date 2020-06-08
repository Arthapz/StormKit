// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

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

namespace storm::engine {
    struct MeshPrimitive {
        const std::string name;

        VertexArray vertices;
        IndexArrayProxy indices;

        render::VertexInputAttributeDescriptionArray attributes;
        render::VertexBindingDescriptionArray bindings;

        std::uint32_t first_vertex;
        std::uint32_t vertex_count;

        std::uint32_t first_index;
        std::uint32_t index_count;
        bool has_indices;

        MaterialInstanceOwnedPtr material_instance;
    };

    class STORM_PUBLIC MeshNode: public core::TreeNode {
      public:
        explicit MeshNode(std::string name = "");
        ~MeshNode();

        MeshNode(MeshNode &&);
        MeshNode &operator=(MeshNode &&);

        inline void addPrimitive(MeshPrimitive &&primitive);
        [[nodiscard]] inline core::span<const MeshPrimitive> primitives() const noexcept;

        inline void setTransform(core::Matrixf transform) noexcept;
        [[nodiscard]] inline const core::Matrixf &transform() const noexcept;

        [[nodiscard]] inline std::string_view name() const noexcept;

      private:
        std::string m_name;

        core::Matrixf m_transform;

        std::vector<MeshPrimitive> m_primitives;
    };
} // namespace storm::engine

#include "Mesh.inl"
