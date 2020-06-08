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
    class Node;
    class Scene;
    class Model;
    class Primitive;
    class Skin;
} // namespace tinygltf

namespace storm::engine {
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
        MeshOwnedPtr m_mesh;

        MeshNode doParseMesh(const tinygltf::Model &model, const tinygltf::Mesh &mesh);
        MeshPrimitive doParsePrimitive(const tinygltf::Model &model,
                                       const tinygltf::Primitive &primitive);
        MaterialInstanceOwnedPtr doParseMaterialInstance(const tinygltf::Model &model,
                                                         const tinygltf::Material &material);

        void doParseScene(const tinygltf::Scene &scene);
        void doParseSkin(const tinygltf::Model &gltf_model,
                         const tinygltf::Skin &gltf_skin,
                         Mesh &mesh);

        EngineObserverPtr m_engine;
        TexturePoolObserverPtr m_texture_pool;
        MaterialPoolObserverPtr m_material_pool;

        std::filesystem::path m_filepath;

        std::vector<Mesh> m_meshes;
        bool m_loaded = false;
    };
} // namespace storm::engine

#include "Model.inl"
