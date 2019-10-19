// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::engine {
    class Scene;
    DECLARE_PTR_AND_REF(Scene);

    class Camera;
    DECLARE_PTR_AND_REF(Camera);

    class FPSCamera;
    DECLARE_PTR_AND_REF(FPSCamera);
} // namespace storm::engine
