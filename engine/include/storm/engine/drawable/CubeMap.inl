// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void CubeMap::setTexture(const render::Texture &texture) noexcept {
        auto &instance = static_cast<CubeMapMaterialInstance &>(*m_material_instance);
        instance.setCubeMap(texture);
    }
} // namespace storm::engine
