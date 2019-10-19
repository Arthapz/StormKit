#pragma once

#include <storm/entities/Component.hpp>
#include <storm/core/Math.hpp>

struct CameraComponent : public storm::entities::Component {
    explicit CameraComponent() {
    }
    ~CameraComponent() override {
    }

    CameraComponent(const CameraComponent &);
    CameraComponent &operator=(const CameraComponent &);

    CameraComponent(CameraComponent &&);
    CameraComponent &operator=(CameraComponent &&);

    storm::core::Matrix projection_matrix;
    storm::core::Matrix view_matrix;

    static constexpr Type TYPE = "CameraComponent"_component_type;
};
