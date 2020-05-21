// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/material/PBRMaterialInstance.hpp>

namespace storm::engine {
    class STORM_PUBLIC PBRMaterial final: public Material {
      public:
        static inline const auto PBR_COLOR_PASS_VERT_SHADER_NAME =
            std::string { "StormKit:PBRColorPass:Vert" };
        static inline const auto PBR_COLOR_PASS_FRAG_SHADER_NAME =
            std::string { "StormKit:PBRColorPass:Frag" };

        explicit PBRMaterial(Scene &scene);
        virtual ~PBRMaterial();

        PBRMaterial(PBRMaterial &&);
        PBRMaterial &operator=(PBRMaterial &&);

        [[nodiscard]] MaterialInstanceOwnedPtr createInstancePtr() const noexcept override;

      private:
        void buildShaders();
    };
} // namespace storm::engine

HASH_FUNC(storm::engine::PBRMaterial)

#include "PBRMaterial.inl"
