// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/drawable/Mesh.hpp>

namespace tinygltf {
    struct Mesh;
    class Model;
} // namespace tinygltf

namespace storm::engine::v2 {
    class STORM_PUBLIC Model final: public core::NonCopyable {
      public:
        Model(Engine &engine, TexturePool &texture_pool, MaterialPool &material_pool);
        virtual ~Model();

        Model(Model &&);
        Model &operator=(Model &&);

        void load(const std::filesystem::path &path);

        [[nodiscard]] inline const std::filesystem::path &filepath() const noexcept;
        [[nodiscard]] inline bool loaded() const noexcept;

        [[nodiscard]] MeshArray createMeshes() noexcept;
        [[nodiscard]] MeshOwnedPtrArray createMeshesPtr() noexcept;

      private:
        struct Mesh {
            struct Submesh {
                core::UInt32 vertex_count;
                core::UInt32 first_index;
                core::UInt32 index_count;
                core::Vector3f min;
                core::Vector3f max;

                std::vector<std::pair<std::string_view, render::TextureConstObserverPtr>> samplers;
                std::vector<std::pair<std::string_view, core::ByteArray>> datas;
            };
            render::VertexInputAttributeDescriptionArray attributes;
            render::VertexBindingDescriptionArray bindings;

            core::Matrixf initial_transform;
            VertexArray vertex_array;
            bool has_indices = false;
            IndexArrayProxy index_array;

            std::vector<Submesh> submeshes;
        };

        Mesh doParseMesh(const tinygltf::Model &gltf_model, const tinygltf::Mesh &gltf_mesh);

        EngineObserverPtr m_engine;
        TexturePoolObserverPtr m_texture_pool;
        MaterialPoolObserverPtr m_material_pool;

        std::filesystem::path m_filepath;

        std::vector<Mesh> m_meshes;
        bool m_loaded = false;
    };
} // namespace storm::engine::v2

#include "Model.inl"
