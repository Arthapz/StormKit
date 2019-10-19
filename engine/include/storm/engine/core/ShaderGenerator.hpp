// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Pimpl.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    struct ShaderGeneratorPrivate;
    class STORM_PUBLIC ShaderGenerator {
      public:
        struct Shaders {
            render::ShaderOwnedPtr vertex;
            render::ShaderOwnedPtr fragment;
        };

        ShaderGenerator(const Engine &engine);
        ~ShaderGenerator();

        ShaderGenerator(ShaderGenerator &&);
        ShaderGenerator &operator=(ShaderGenerator &&);

        Shaders generateShaders(const render::GraphicsPipelineState &pipeline) const;

      private:
        EngineConstObserverPtr m_engine;

        core::Pimpl<ShaderGeneratorPrivate> m_pimpl;
    };
} // namespace storm::engine

#include "ShaderGenerator.inl"