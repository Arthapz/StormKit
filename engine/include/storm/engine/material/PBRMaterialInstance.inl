// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setAlbedoMap(const render::Texture &map) noexcept {
        setSamplerTexture(ALBEDO_MAP_NAME, map);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setNormalMap(const render::Texture &map) noexcept {
        setSamplerTexture(NORMAL_MAP_NAME, map);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setMetallicMap(const render::Texture &map) noexcept {
        setSamplerTexture(METALLIC_MAP_NAME, map);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setRoughnessMap(const render::Texture &map) noexcept {
        setSamplerTexture(ROUGHNESS_MAP_NAME, map);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setAmbiantOcclusionMap(const render::Texture &map) noexcept {
        setSamplerTexture(AMBIANT_OCCLUSION_MAP_NAME, map);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setEmissiveMap(const render::Texture &map) {
        setSamplerTexture(EMISSIVE_MAP_NAME, map);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setAlbedoFactor(core::Vector4f factor) noexcept {
        setDataValue(ALBEDO_FACTOR_NAME, std::move(factor));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setMetallicFactor(float factor) noexcept {
        setDataValue(METALLIC_FACTOR_NAME, factor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setRoughnessFactor(float factor) noexcept {
        setDataValue(ROUGHNESS_FACTOR_NAME, factor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setAmbiantOcclusionFactor(float factor) noexcept {
        setDataValue(AMBIANT_OCCLUSION_FACTOR_NAME, factor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void PBRMaterialInstance::setEmissiveFactor(core::Vector4f factor) noexcept {
        setDataValue(EMISSIVE_FACTOR_NAME, std::move(factor));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void
        PBRMaterialInstance::setDebugView(PBRMaterialInstance::DebugView debug_index) noexcept {
        setDataValue("PAD0", debug_index);
    }
} // namespace storm::engine
