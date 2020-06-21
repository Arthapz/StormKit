// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

namespace storm::engine {
    class Mesh;
    DECLARE_PTR_AND_REF(Mesh);

    class SubMesh;
    DECLARE_PTR_AND_REF(SubMesh);

    struct MeshPrimitive;

    class Model;
    DECLARE_PTR_AND_REF(Model);

    class CubeMap;
    DECLARE_PTR_AND_REF(CubeMap);
} // namespace storm::engine
