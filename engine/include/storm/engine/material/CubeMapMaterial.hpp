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

#include <storm/engine/material/Material.hpp>

namespace storm::engine {
    class STORM_PUBLIC CubeMapMaterial final: public Material {
      public:
        static inline const auto CUBE_MAP_VERT_SHADER_NAME =
            std::string { "StormKit:CubeMap:Vert" };
        static inline const auto CUBE_MAP_FRAG_SHADER_NAME =
            std::string { "StormKit:CubeMap:Frag" };
        static inline const auto DEFAULT_CUBE_MAP_TEXTURE =
            std::string { "StormKit:DefaultCubeMap" };

        explicit CubeMapMaterial(Scene &scene);
        virtual ~CubeMapMaterial();

        CubeMapMaterial(CubeMapMaterial &&);
        CubeMapMaterial &operator=(CubeMapMaterial &&);

        [[nodiscard]] MaterialInstanceOwnedPtr createInstancePtr() const noexcept override;

      private:
        void buildShaders();
    };
} // namespace storm::engine

HASH_FUNC(storm::engine::CubeMapMaterial)
