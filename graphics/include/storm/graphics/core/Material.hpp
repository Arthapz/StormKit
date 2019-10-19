#pragma once

#include <storm/core/Hash.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/resource/Fwd.hpp>

#include <storm/graphics/core/Fwd.hpp>

namespace storm::graphics {
    class Material {
      public:
        render::TextureConstObserverPtr diffuse_map			= nullptr;
        render::SamplerConstObserverPtr diffuse_map_sampler = nullptr;
    };
} // namespace storm::graphics

HASH_FUNC(storm::graphics::Material)
