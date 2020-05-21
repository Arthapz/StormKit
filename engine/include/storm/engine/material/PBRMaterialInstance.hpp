// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/material/MaterialInstance.hpp>

namespace storm::engine {
    class STORM_PUBLIC PBRMaterialInstance final: public MaterialInstance {
      public:
        static constexpr auto ALBEDO_MAP_NAME    = std::string_view { "albedo_map" };
        static constexpr auto NORMAL_MAP_NAME    = std::string_view { "normal_map" };
        static constexpr auto METALLIC_MAP_NAME  = std::string_view { "metallic_map" };
        static constexpr auto ROUGHNESS_MAP_NAME = std::string_view { "roughness_map" };
        static constexpr auto AMBIANT_OCCLUSION_MAP_NAME =
            std::string_view { "ambiant_occlusion_map" };
        static constexpr auto EMISSIVE_MAP_NAME = std::string_view { "emissive_map" };

        static constexpr auto ALBEDO_FACTOR_NAME    = std::string_view { "albedo_factor" };
        static constexpr auto METALLIC_FACTOR_NAME  = std::string_view { "metallic_factor" };
        static constexpr auto ROUGHNESS_FACTOR_NAME = std::string_view { "roughness_factor" };
        static constexpr auto AMBIANT_OCCLUSION_FACTOR_NAME =
            std::string_view { "ambiant_occlusion_factor" };
        static constexpr auto EMISSIVE_FACTOR_NAME = std::string_view { "emissive_factor" };
        static constexpr auto DEBUG_VIEW_NAME      = std::string_view { "PAD0" };

        enum class DebugView : core::UInt32 {
            None                  = 0u,
            Albedo_Map            = 1u,
            Normal_Map            = 2u,
            Metallic_Map          = 3u,
            Roughness_Map         = 4u,
            Ambiant_Occlusion_Map = 5u,
            Emissive_Map          = 6u,

            Diffuse_Contribution  = 7u,
            Specular_Contribution = 8u,
            F                     = 9u,
            G                     = 10u,
            D                     = 11u
        };

        explicit PBRMaterialInstance(const Scene &scene, const PBRMaterial &material);
        virtual ~PBRMaterialInstance();

        PBRMaterialInstance(PBRMaterialInstance &&);
        PBRMaterialInstance &operator=(PBRMaterialInstance &&);

        inline void setAlbedoMap(const render::Texture &map,
                                 render::Sampler::Settings sampler_settings = {}) noexcept;
        inline void setNormalMap(const render::Texture &map,
                                 render::Sampler::Settings sampler_settings = {}) noexcept;
        inline void setMetallicMap(const render::Texture &map,
                                   render::Sampler::Settings sampler_settings = {}) noexcept;
        inline void setRoughnessMap(const render::Texture &map,
                                    render::Sampler::Settings sampler_settings = {}) noexcept;
        inline void
            setAmbiantOcclusionMap(const render::Texture &map,
                                   render::Sampler::Settings sampler_settings = {}) noexcept;
        inline void setEmissiveMap(const render::Texture &map,
                                   render::Sampler::Settings sampler_settings = {});

        inline void setAlbedoFactor(core::Vector4f factor) noexcept;
        inline void setMetallicFactor(float factor) noexcept;
        inline void setRoughnessFactor(float factor) noexcept;
        inline void setAmbiantOcclusionFactor(float factor) noexcept;
        inline void setEmissiveFactor(core::Vector4f factor) noexcept;

        inline void setDebugView(DebugView debug_index) noexcept;
    }; // namespace storm::engine
} // namespace storm::engine

#include "PBRMaterialInstance.inl"
