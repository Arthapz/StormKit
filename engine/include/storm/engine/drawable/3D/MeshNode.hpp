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

    class MeshNodeFlag;
    class STORM_PUBLIC MeshNode: public Drawable, public StaticBindable<MeshNodeFlag> {
      public:
        static constexpr auto MAX_JOINT_COUNT = 128;
        explicit MeshNode(Engine &engine, std::string name = "");
        ~MeshNode();

        MeshNode(MeshNode &&);
        MeshNode &operator=(MeshNode &&);

        inline void addPrimitive(MeshPrimitive &&primitive);
        [[nodiscard]] inline core::span<const MeshPrimitive> primitives() const noexcept;

        inline void setTransform(core::Matrixf transform) noexcept;
        [[nodiscard]] inline const core::Matrixf &transform() const noexcept;

        [[nodiscard]] inline std::string_view name() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state) override;

        void flush();

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        using Bitset = std::bitset<1>;

        static constexpr auto TRANSFORM_DIRTY = 0b0;

        struct Data {
            core::Matrixf transform = core::Matrixf { 1.f };
            std::array<core::Matrixf, MAX_JOINT_COUNT> joints;
            float joint_count;
        };

        [[nodiscard]] inline core::span<MeshPrimitive> primitives() noexcept;

        std::string m_name;

        std::vector<MeshPrimitive> m_primitives;

        Data m_data;
        Bitset m_bitset;
        render::HardwareBufferOwnedPtr m_mesh_data_buffer;

        friend class Mesh;
    };
} // namespace storm::engine

#include "MeshNode.inl"
