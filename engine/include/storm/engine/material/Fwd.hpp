// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::engine {
    class Material;
    DECLARE_PTR_AND_REF(Material);

    class MaterialInstance;
    DECLARE_PTR_AND_REF(MaterialInstance);

    class PBRMaterial;
    DECLARE_PTR_AND_REF(PBRMaterial);

    class PBRMaterialInstance;
    DECLARE_PTR_AND_REF(PBRMaterialInstance);
} // namespace storm::engine
