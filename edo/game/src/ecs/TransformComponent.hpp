#pragma once

#include <storm/entities/Component.hpp>
#include <storm/core/Math.hpp>

struct TransformComponent : public storm::entities::Component {
    explicit TransformComponent() {
    }
    ~TransformComponent() override {
    }

    TransformComponent(const TransformComponent &);
    TransformComponent &operator=(const TransformComponent &);

    TransformComponent(TransformComponent &&);
    TransformComponent &operator=(TransformComponent &&);

    storm::core::Matrix transform_matrix = storm::core::Matrix{1.f};

    static constexpr Type TYPE = "TransformComponent"_component_type;
};
