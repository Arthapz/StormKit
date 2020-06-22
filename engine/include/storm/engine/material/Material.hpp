// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <cstdint>
#include <map>
#include <string>
#include <variant>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC Material {
      public:
        using Binding = core::UInt32;

        enum class UniformType {
            UInt8,
            UInt16,
            UInt32,
            Int8,
            Int16,
            Int32,
            Float,
            Double,
            Vec2u,
            Vec2i,
            Vec2f,
            Vec4u,
            Vec4i,
            Vec4f,
            Mat4u,
            Mat4i,
            Mat4f
        };

        struct MaterialData {
            render::GraphicsPipelineShaderState shader_state;

            struct Sampler {
                std::string name;
                render::TextureViewType type;
            };

            std::vector<std::pair<Binding, Sampler>> samplers;
            std::vector<std::pair<std::string, UniformType>> uniforms;
        };

        enum class AlphaMode { Opaque, Mask, Blend };

        explicit Material(Scene &scene);
        virtual ~Material();

        Material(Material &&);
        Material &operator=(Material &&);

        /* immuable data */
        inline void addShader(const render::Shader &shader);

        /* instance based data */
        inline void addSampler(Binding binding,
                               std::string name,
                               render::TextureViewType = render::TextureViewType::T2D);
        inline void addUniform(std::string name, UniformType type);

        void finalize() noexcept;

        [[nodiscard]] inline const MaterialData &data() const noexcept;

        [[nodiscard]] virtual MaterialInstanceOwnedPtr createInstancePtr() const noexcept;

        [[nodiscard]] inline core::Hash64 hash() const noexcept;

      protected:
        SceneObserverPtr m_scene;

        MaterialData m_data;

        render::DescriptorSetLayoutOwnedPtr m_descriptor_set_layout;
        core::Hash64 m_hash;
        bool m_finalized = false;

        friend class MaterialInstance;
        friend class Mesh;
        friend class CubeMap;
    }; // namespace storm::engine
} // namespace storm::engine

HASH_FUNC(storm::engine::Material)

#include "Material.inl"
