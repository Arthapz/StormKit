// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "CubeMapMaterialInstance.hpp"

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void CubeMapMaterialInstance::setCubeMap(
        const render::Texture &map,
        std::optional<render::TextureSubresourceRange> subresource_range,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        auto settings = sampler_settings.value_or(
            render::Sampler::Settings { .address_mode_u = render::SamplerAddressMode::Clamp_To_Edge,
                                        .address_mode_v = render::SamplerAddressMode::Clamp_To_Edge,
                                        .address_mode_w = render::SamplerAddressMode::Clamp_To_Edge,
                                        .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .border_color = render::BorderColor::Float_Opaque_White,
                                        .compare_operation = render::CompareOperation::Never });

        auto range = subresource_range.value_or(
            render::TextureSubresourceRange { .level_count = map.mipLevels(), .layer_count = 6 });

        setSampledTexture("cubemap",
                          map,
                          render::TextureViewType::Cube,
                          std::move(range),
                          std::move(settings));
    }
} // namespace storm::engine
