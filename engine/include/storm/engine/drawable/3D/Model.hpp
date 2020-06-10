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

#include <storm/engine/drawable/3D/Mesh.hpp>
#include <storm/engine/drawable/3D/SubMesh.hpp>

namespace tinygltf {
    struct Mesh;
    class Node;
    class Scene;
    class Model;
    class Material;
    class Primitive;
    class Skin;
    class Animation;
} // namespace tinygltf

namespace storm::engine {
    class STORM_PUBLIC Model final: public core::NonCopyable {
      public:
        struct Vertex {
            core::Vector3f position;
            core::Vector3f normal;
            core::Vector2f texcoord;
            core::Vector4f tangent;
            core::Vector4u join_id = { 0.f, 0.f, 0.f, 0.f };
            core::Vector4f weight  = { 0.f, 0.f, 0.f, 0.f };
        };

        Model(Engine &engine, TexturePool &texture_pool, MaterialPool &material_pool);
        virtual ~Model();

        Model(Model &&);
        Model &operator=(Model &&);

        void load(const std::filesystem::path &path);

        [[nodiscard]] inline const std::filesystem::path &filepath() const noexcept;
        [[nodiscard]] inline bool loaded() const noexcept;

        [[nodiscard]] Mesh createMesh() noexcept;
        [[nodiscard]] MeshOwnedPtr createMeshPtr() noexcept;

      private:
        MeshOwnedPtr m_mesh;

        SubMesh doParseMesh(const tinygltf::Model &model, const tinygltf::Mesh &mesh);
        MeshPrimitive doParsePrimitive(const tinygltf::Model &model,
                                       const tinygltf::Primitive &primitive);
        MaterialInstanceOwnedPtr doParseMaterialInstance(const tinygltf::Model &model,
                                                         const tinygltf::Material &material);
        Mesh::Skin doParseSkin(const tinygltf::Model &model, const tinygltf::Skin &skin);
        Mesh::Animation doParseAnimation(const tinygltf::Model &model,
                                         const tinygltf::Animation &animation);

        EngineObserverPtr m_engine;
        TexturePoolObserverPtr m_texture_pool;
        MaterialPoolObserverPtr m_material_pool;

        std::filesystem::path m_filepath;

        bool m_loaded = false;
    };
} // namespace storm::engine

#include "Model.inl"
