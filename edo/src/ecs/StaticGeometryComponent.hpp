#pragma once

#include <storm/entities/Component.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/graphics/core/Fwd.hpp>
#include <storm/graphics/core/Vertex.hpp>

struct StaticGeometryComponent
    : public storm::entities::Component {
    explicit StaticGeometryComponent() {
    }
    ~StaticGeometryComponent() override {
    }

    StaticGeometryComponent(const StaticGeometryComponent &);
    StaticGeometryComponent &operator=(const StaticGeometryComponent &);

    StaticGeometryComponent(StaticGeometryComponent &&);
    StaticGeometryComponent &operator=(StaticGeometryComponent &&);

    storm::render::GraphicsPipelineStateConstObserverPtr state;

    std::uint32_t vertex_count;
    storm::graphics::VertexArrayProxy vertices;

    std::uint32_t index_count;
    std::optional<storm::graphics::IndexArrayProxy> indices;

    static constexpr Type TYPE = "StaticGeometryComponent"_component_type;
};
