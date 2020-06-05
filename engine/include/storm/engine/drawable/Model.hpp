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
        struct Node {
            std::optional<Mesh> mesh;
        };

        struct Mesh {
            struct AnimationChannel {
                enum class PathType { Translation, Rotation, Scale } path_type;

                core::UInt32 sampler_index;
            };

            struct AnimationSampler {
                enum class InterpolationType { Linear, Step, Cubic_Spline } interpolation;

                std::vector<float> inputs;
                std::vector<core::Vector4f> outputs;
            };

            struct Animation {
                std::string name;

                std::vector<AnimationSampler> samplers;
                std::vector<AnimationChannel> channels;

                float start = std::numeric_limits<float>::max();
                float end   = std::numeric_limits<float>::min();
            };

            struct Join;
            using JoinObserverPtr = _std::observer_ptr<Join>;
            struct Join {
                std::vector<JoinObserverPtr> childrens;
            };

            struct Skin {
                std::string name;

                std::vector<core::Matrixf> inverse_bind_matrices;

                std::vector<Join> joints;
            };

            struct Submesh {
                core::UInt32 vertex_count;
                core::UInt32 first_index;
                core::UInt32 index_count;
                core::Vector3f min;
                core::Vector3f max;

                std::vector<std::pair<std::string_view, render::TextureConstObserverPtr>> samplers;
                std::vector<std::pair<std::string_view, core::ByteArray>> datas;

                bool double_sided = false;
            };
            render::VertexInputAttributeDescriptionArray attributes;
            render::VertexBindingDescriptionArray bindings;

            core::Matrixf initial_transform;
            VertexArray vertex_array;
            bool has_indices = false;
            IndexArrayProxy index_array;

            Skin skin;

            std::vector<Submesh> submeshes;
        };

        void doParseScene(const tinygltf::Scene &gltf_scene);
        void doParseMesh(const tinygltf::Model &gltf_model,
                         const tinygltf::Mesh &gltf_mesh,
                         Mesh &mesh);
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
