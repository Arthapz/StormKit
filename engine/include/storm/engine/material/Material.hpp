// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
        struct alignas(16) Data {
            core::Vector4f base_color_factor = { 1.f, 1.f, 1.f, 1.f };
            core::Vector4f emissive_factor   = { 0.f, 0.f, 0.f, 1.f };
            float metallic_factor            = 1.f;
            float roughness_factor           = 1.f;
            float ambiant_occlusion_factor   = 1.f;
            float PAD0                       = 0.f;
        };

        explicit Material(const Engine &engine);
        ~Material();

        Material(Material &&);
        Material &operator=(Material &&);

        inline void setGraphicsPipelineState(render::GraphicsPipelineState state) noexcept;
        inline const render::GraphicsPipelineState &pipelineState() const noexcept;

        inline void setBaseColorMap(const render::Texture &map) noexcept;
        [[nodiscard]] inline bool hasBaseColorMap() const noexcept;
        inline void setNormalMap(const render::Texture &map) noexcept;
        [[nodiscard]] inline bool hasNormalMap() const noexcept;
        inline void setMetallicRoughnessMap(const render::Texture &map) noexcept;
        [[nodiscard]] inline bool hasMetallicRoughnessMap() const noexcept;
        inline void setAmbiantOcclusionMap(const render::Texture &map) noexcept;
        [[nodiscard]] inline bool hasAmbiantOcclusionMap() const noexcept;
        inline void setEmissiveMap(const render::Texture &map) noexcept;
        [[nodiscard]] inline bool hasEmissiveMap() const noexcept;

        inline void setBaseColorFactor(const core::RGBColorF &color) noexcept;
        inline core::RGBColorF baseColorFactor() const noexcept;

        inline void setMetallicFactor(float factor) noexcept;
        inline float metallicFactor() const noexcept;

        inline void setRoughnessFactor(float factor) noexcept;
        inline float roughnessFactor() const noexcept;

        inline void setAmbiantOcclusionFactor(float factor) noexcept;
        inline float ambiantOcclusionFactor() const noexcept;

        inline void setEmissiveFactor(const core::RGBColorF &factor) noexcept;
        inline core::RGBColorF emissiveFactor() const noexcept;

        inline bool isDirty() const noexcept;

        inline const std::vector<std::byte> &pushConstantsData() const noexcept;

        inline const render::DescriptorSet &descriptorSet() const noexcept;

        bool ensureIsUpdated();
        inline core::UInt64 hash() const noexcept;

      private:
        void recomputeHash() const noexcept;

        EngineConstObserverPtr m_engine;

        render::DescriptorSetOwnedPtr m_descriptor_set;

        render::GraphicsPipelineState m_pipeline_state;

        render::TextureViewOwnedPtr m_default_map_view;

        render::TextureConstObserverPtr m_base_color_map;
        render::TextureViewOwnedPtr m_base_color_map_view;

        render::TextureConstObserverPtr m_normal_map;
        render::TextureViewOwnedPtr m_normal_map_view;

        render::TextureConstObserverPtr m_metallic_roughness_map;
        render::TextureViewOwnedPtr m_metallic_roughness_map_view;

        render::TextureConstObserverPtr m_ambiant_occlusion_map;
        render::TextureViewOwnedPtr m_ambiant_occlusion_map_view;

        render::TextureConstObserverPtr m_emissive_map;
        render::TextureViewOwnedPtr m_emissive_map_view;

        render::SamplerOwnedPtr m_sampler;

        Data m_data;
        std::vector<std::byte> m_push_constant_data;

        bool m_is_dirty = true;

        mutable core::UInt64 m_hash;
        mutable bool m_need_recompute_hash = true;
    }; // namespace storm::engine
} // namespace storm::engine

HASH_FUNC(storm::engine::Material)

#include "Material.inl"
