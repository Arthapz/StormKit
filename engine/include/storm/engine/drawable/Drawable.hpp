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

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/drawable/BoundingBox.hpp>

namespace storm::engine {
    class STORM_PUBLIC Drawable: public core::NonCopyable {
      public:
        Drawable(Engine &engine);
        virtual ~Drawable();

        Drawable(Drawable &&);
        Drawable &operator=(Drawable &&);

        [[nodiscard]] inline const BoundingBox &boundingBox() const noexcept;

        virtual void render(render::CommandBuffer &cmb,
                            const render::RenderPass &pass,
                            std::vector<BindableBaseConstObserverPtr> bindables,
                            render::GraphicsPipelineState state) = 0;

      protected:
        virtual void recomputeBoundingBox() const noexcept = 0;

        EngineObserverPtr m_engine;

        mutable BoundingBox m_bounding_box;
        mutable bool m_is_bounding_box_dirty = true;
    };
} // namespace storm::engine

#include "Drawable.inl"
