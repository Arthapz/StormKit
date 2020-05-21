// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void CubeMapMaterialInstance::setCubeMap(const render::Texture &map,
                                             render::Sampler::Settings sampler_settings) noexcept {
        STORM_EXPECTS(map.layers() == 6);

        setSampledTexture("cubemap",
                          map,
                          render::TextureViewType::Cube,
                          { .layer_count = 6u },
                          std::move(sampler_settings));
    }
} // namespace storm::engine
