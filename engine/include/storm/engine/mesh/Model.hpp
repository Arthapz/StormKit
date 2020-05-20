// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>
#include <optional>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/core/Fwd.hpp>
#include <storm/engine/core/Vertex.hpp>

/////////// - StormKit::material - ///////////
#include <storm/engine/material/Material.hpp>

struct aiMesh;
struct aiScene;

namespace storm::engine {
    class STORM_PUBLIC Model: public core::NonCopyable {
      public:
        struct SubModel {
            VertexArray vertices;
            LargeIndexArray indices = {};
        };

        enum class Type { GLTF, GLB };

        explicit Model(Scene &scene);
        ~Model();

        Model(Model &&);
        Model &operator=(Model &&);

        std::optional<StaticMesh> loadStaticMeshFromFile(const std::filesystem::path &path,
                                                         Type type);

        inline gsl::span<const SubModel> subModels() const noexcept { return m_sub_models; }

      private:
        SceneObserverPtr m_scene;

        std::filesystem::path m_filepath;

        std::vector<SubModel> m_sub_models;
        std::vector<Material> m_material_compilers;

        render::TextureOwnedPtrArray m_textures;
        render::TextureViewOwnedPtrArray m_texture_views;
        render::SamplerOwnedPtrArray m_samplers;

        std::vector<Mesh> m_meshes;
    };
} // namespace storm::engine
