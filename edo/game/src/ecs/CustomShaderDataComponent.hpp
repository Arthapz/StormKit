#pragma once

#include <vector>

#include <storm/entities/Component.hpp>

#include <storm/render/pipeline/Fwd.hpp>

struct CustomShaderDataComponent
    : public storm::entities::Component {
    explicit CustomShaderDataComponent() {
    }
    ~CustomShaderDataComponent() override {
    }

    CustomShaderDataComponent(const CustomShaderDataComponent &);
    CustomShaderDataComponent &operator=(const CustomShaderDataComponent &);

    CustomShaderDataComponent(CustomShaderDataComponent &&);
    CustomShaderDataComponent &operator=(CustomShaderDataComponent &&);

    std::vector<storm::render::DescriptorSetConstObserverPtr> sets;

    static constexpr Type TYPE = "CustomShaderDataComponent"_component_type;
};
