// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <array>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/VertexBindingDescription.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>
#include <storm/engine/mesh/StaticSubMesh.hpp>

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto makeTriangle(Engine &engine, core::Extentf size = { 0.5f, 0.5f }) {
        auto attributes = render::TaggedVertexInputAttributeDescriptionArray {};
        auto bindings   = render::VertexBindingDescriptionArray {};

        attributes.emplace("Position",
                           render::VertexInputAttributeDescription { 0u,
                                                                     0u,
                                                                     render::Format::Float2,
                                                                     0u });
        bindings.emplace_back(render::VertexBindingDescription { 0u, sizeof(float) * 2 });

        auto vertex_array = VertexArray {};
        vertex_array.push_back(0.f);
        vertex_array.push_back(-size.h);
        vertex_array.push_back(size.w);
        vertex_array.push_back(size.h);
        vertex_array.push_back(-size.w);
        vertex_array.push_back(size.h);

        auto mesh = StaticMesh { engine, std::move(attributes), std::move(bindings) };
        mesh.setVertexArray(std::move(vertex_array));

        auto &submesh = mesh.addSubmesh(3u, 0, 0);
        submesh.setBoundingBox({ -0.5f, -0.5f, 0.f }, { 0.5f, 0.5f, 0.f });

        return mesh;
    }
} // namespace storm::engine
