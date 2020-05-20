// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

namespace storm::engine {
    class Model;
    DECLARE_PTR_AND_REF(Model);

    class StaticMesh;
    DECLARE_PTR_AND_REF(StaticMesh);

    class StaticSubMesh;
    DECLARE_PTR_AND_REF(StaticSubMesh);

    class StaticMeshRenderSystem;
    DECLARE_PTR_AND_REF(StaticMeshRenderSystem);
} // namespace storm::engine
