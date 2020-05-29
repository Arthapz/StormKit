// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/drawable/Drawable.hpp>

#include <storm/engine/material/CubeMapMaterialInstance.hpp>

namespace storm::engine {
    class STORM_PUBLIC CubeMap: public Drawable {
      public:
        CubeMap(Scene &scene);
        ~CubeMap() override;

        CubeMap(CubeMap &&);
        CubeMap &operator=(CubeMap &&);

        inline void setTexture(const render::Texture &texture,
                               render::TextureSubresourceRange subresource_range = {}) noexcept;
        inline const render::Texture &texture() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state) override;

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        SceneObserverPtr m_scene;

        render::TextureConstObserverPtr m_texture;

        MaterialInstanceOwnedPtr m_material_instance;
    };
} // namespace storm::engine

#include "CubeMap.inl"
