// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>
#include <storm/core/ResourcesPool.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/resource/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/engine/material/Fwd.hpp>

namespace storm::engine {
    class Scene;
    DECLARE_PTR_AND_REF(Scene);

    class Camera;
    DECLARE_PTR_AND_REF(Camera);

    class FPSCamera;
    DECLARE_PTR_AND_REF(FPSCamera);

    using ShaderPool = core::ResourcesPool<std::string, render::Shader>;
    DECLARE_PTR_AND_REF(ShaderPool);

    using TexturePool = core::ResourcesPool<std::string, render::Texture>;
    DECLARE_PTR_AND_REF(TexturePool);

    using MaterialPool = core::ResourcesPool<std::string, MaterialOwnedPtr>;
    DECLARE_PTR_AND_REF(MaterialPool);
} // namespace storm::engine
