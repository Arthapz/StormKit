#pragma once

#include <storm/entities/Component.hpp>

#include <storm/render/resource/Fwd.hpp>

struct TextureComponent : public storm::entities::Component {
    explicit TextureComponent() {
    }
    ~TextureComponent() override {
    }

    TextureComponent(const TextureComponent &);
    TextureComponent &operator=(const TextureComponent &);

    TextureComponent(TextureComponent &&);
    TextureComponent &operator=(TextureComponent &&);

    storm::render::TextureConstObserverPtr texture;
    storm::render::SamplerConstObserverPtr sampler;

    static constexpr Type TYPE = "TextureComponent"_component_type;
};
