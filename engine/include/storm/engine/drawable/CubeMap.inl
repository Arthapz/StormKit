// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void CubeMap::setTexture(const render::Texture &texture,
                             render::TextureSubresourceRange subresource_range) noexcept {
        m_texture = core::makeConstObserver(texture);

        auto &instance = static_cast<CubeMapMaterialInstance &>(*m_material_instance);
        instance.setCubeMap(texture, std::move(subresource_range));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::Texture &CubeMap::texture() const noexcept {
        STORM_EXPECTS(m_texture != nullptr);
        return *m_texture;
    }
} // namespace storm::engine
