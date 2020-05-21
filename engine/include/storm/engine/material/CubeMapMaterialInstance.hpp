// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/material/MaterialInstance.hpp>

namespace storm::engine {
    class STORM_PUBLIC CubeMapMaterialInstance final: public MaterialInstance {
      public:
        static constexpr auto CUBE_MAP_NAME = std::string_view { "cubemap" };

        CubeMapMaterialInstance(const Scene &scene, const CubeMapMaterial &material);
        virtual ~CubeMapMaterialInstance();

        CubeMapMaterialInstance(CubeMapMaterialInstance &&);
        CubeMapMaterialInstance &operator=(CubeMapMaterialInstance &&);

        inline void setCubeMap(const render::Texture &map) noexcept;
    }; // namespace storm::engine
} // namespace storm::engine

#include "CubeMapMaterialInstance.inl"
