// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setAlbedoMap(
        const render::Texture &map,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        setSampledTexture(ALBEDO_MAP_NAME,
                          map,
                          render::TextureViewType::T2D,
                          std::nullopt,
                          std::move(sampler_settings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setNormalMap(
        const render::Texture &map,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        setSampledTexture(NORMAL_MAP_NAME,
                          map,
                          render::TextureViewType::T2D,
                          std::nullopt,
                          std::move(sampler_settings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setMetallicMap(
        const render::Texture &map,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        setSampledTexture(METALLIC_MAP_NAME,
                          map,
                          render::TextureViewType::T2D,
                          std::nullopt,
                          std::move(sampler_settings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setRoughnessMap(
        const render::Texture &map,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        setSampledTexture(ROUGHNESS_MAP_NAME,
                          map,
                          render::TextureViewType::T2D,
                          std::nullopt,
                          std::move(sampler_settings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setAmbiantOcclusionMap(
        const render::Texture &map,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        setSampledTexture(AMBIANT_OCCLUSION_MAP_NAME,
                          map,
                          render::TextureViewType::T2D,
                          std::nullopt,
                          std::move(sampler_settings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setEmissiveMap(
        const render::Texture &map,
        std::optional<render::Sampler::Settings> sampler_settings) noexcept {
        setSampledTexture(EMISSIVE_MAP_NAME,
                          map,
                          render::TextureViewType::T2D,
                          std::nullopt,
                          std::move(sampler_settings));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setAlbedoFactor(core::Vector4f factor) noexcept {
        setDataValue(ALBEDO_FACTOR_NAME, std::move(factor));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setMetallicFactor(float factor) noexcept {
        setDataValue(METALLIC_FACTOR_NAME, factor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setRoughnessFactor(float factor) noexcept {
        setDataValue(ROUGHNESS_FACTOR_NAME, factor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setAmbiantOcclusionFactor(float factor) noexcept {
        setDataValue(AMBIANT_OCCLUSION_FACTOR_NAME, factor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setEmissiveFactor(core::Vector4f factor) noexcept {
        setDataValue(EMISSIVE_FACTOR_NAME, std::move(factor));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void PBRMaterialInstance::setDebugView(PBRMaterialInstance::DebugView debug_index) noexcept {
        setDataValue(DEBUG_VIEW_NAME, debug_index);
    }
} // namespace storm::engine
