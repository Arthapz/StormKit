// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

namespace storm::engine {
    class Drawable;
    DECLARE_PTR_AND_REF(Drawable);

    class Mesh;
    DECLARE_PTR_AND_REF(Mesh);

    class SubMesh;
    DECLARE_PTR_AND_REF(SubMesh);

    namespace v2 {
        class Model;
        DECLARE_PTR_AND_REF(Model);
    } // namespace v2
} // namespace storm::engine
