// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <string>

/////////// - StormKit::core - ///////////
#include <storm/core/HashMap.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Bindable.hpp>

#include <storm/engine/material/Material.hpp>

namespace storm::engine {
    class STORM_PUBLIC MaterialInstance: public Bindable {
      public:
        explicit MaterialInstance(const Scene &scene, const Material &material);
        virtual ~MaterialInstance();

        MaterialInstance(MaterialInstance &&);
        MaterialInstance &operator=(MaterialInstance &&);

        inline void setFrontFace(render::FrontFace face);
        [[nodiscard]] inline render::FrontFace frontFace() const noexcept;

        inline void setCullMode(render::CullMode mode);
        [[nodiscard]] inline render::CullMode cullMode() const noexcept;

        inline void setWireFrameEnabled(bool enabled) noexcept;
        [[nodiscard]] inline bool isWireFrameEnabled() const noexcept;

        inline void setSampledTexture(
            std::string_view name,
            const render::Texture &texture,
            render::TextureViewType type = render::TextureViewType::T2D,
            std::optional<render::TextureSubresourceRange> subresource_range = std::nullopt,
            std::optional<render::Sampler::Settings> sampler_settings        = std::nullopt);
        inline void setSamplerSettings(std::string_view name, render::Sampler::Settings settings);
        inline void setRawDataValue(std::string_view name, core::ByteConstSpan bytes);
        template<typename T>
        inline void setDataValue(std::string_view name, T &&value);

        void flush();

        [[nodiscard]] inline const Material &parent() const noexcept;

        [[nodiscard]] inline core::Hash64 hash() const noexcept;

        MaterialInstanceOwnedPtr clone() const;

      protected:
        EngineConstObserverPtr m_engine;

      private:
        void recomputeHash() const noexcept;

        struct SampledBinding {
            Material::Binding binding;

            render::TextureConstObserverPtr texture;
            render::TextureViewOwnedPtr view;
            render::SamplerOwnedPtr sampler;

            bool dirty = true;
        };

        SceneConstObserverPtr m_scene;
        MaterialConstObserverPtr m_parent;

        render::GraphicsPipelineRasterizationState m_rasterization_state;

        storm::core::HashMap<std::string, core::UOffset> m_data_offsets;
        RingHardwareBufferOwnedPtr m_buffer;
        core::Int32 m_buffer_binding = -1;

        core::ByteArray m_bytes;

        bool m_bytes_dirty = true;

        storm::core::HashMap<std::string, SampledBinding> m_sampled_textures;

        bool m_dirty = true;

        mutable bool m_dirty_hash   = true;
        mutable core::Hash64 m_hash = 0u;

        friend class MeshNode;
        friend class CubeMap;
    }; // namespace storm::engine
} // namespace storm::engine

HASH_FUNC(storm::engine::MaterialInstance)

#include "MaterialInstance.inl"
