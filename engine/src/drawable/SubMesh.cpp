/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/Mesh.hpp>
#include <storm/engine/drawable/SubMesh.hpp>

#include <storm/engine/material/Material.hpp>
#include <storm/engine/material/MaterialInstance.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
SubMesh::SubMesh(Engine &engine,
                 Mesh &parent,
                 core::UInt32 vertex_count,
                 core::UInt32 first_index,
                 core::UInt32 index_count,
                 core::Vector3f min,
                 core::Vector3f max)
    : Drawable { engine }, m_parent_mesh { &parent }, m_vertex_count { vertex_count },
      m_first_index { first_index }, m_index_count { index_count } {
    const auto &material = m_parent_mesh->material();

    m_material_instance = material.createInstancePtr();

    m_bounding_box.min = std::move(min);
    m_bounding_box.max = std::move(max);

    const auto extent = m_bounding_box.max - m_bounding_box.min;

    m_bounding_box.extent.width  = extent.x;
    m_bounding_box.extent.height = extent.y;
    m_bounding_box.extent.depth  = extent.z;
}

////////////////////////////////////////
////////////////////////////////////////
SubMesh::~SubMesh() = default;

////////////////////////////////////////
////////////////////////////////////////
SubMesh::SubMesh(SubMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
SubMesh &SubMesh::operator=(SubMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void SubMesh::render(render::CommandBuffer &cmb,
                     const render::RenderPass &pass,
                     std::vector<BindableBaseConstObserverPtr> bindables,
                     render::GraphicsPipelineState state) {
    m_material_instance->flush();

    bindables.emplace_back(core::makeConstObserver(m_material_instance));

    state.rasterization_state = m_material_instance->m_rasterization_state;

    auto descriptors = std::vector<render::DescriptorSetCRef> {};
    auto offsets     = std::vector<core::UOffset> {};

    descriptors.reserve(std::size(bindables));
    offsets.reserve(std::size(bindables));

    for (const auto &bindable : bindables) {
        descriptors.push_back(bindable->descriptorSet());

        const auto &offset = bindable->offset();
        if (offset.has_value()) offsets.push_back(offset.value());

        state.layout.descriptor_set_layouts.emplace_back(
            core::makeConstObserver(bindable->descriptorLayout()));
    }

    const auto &pipeline = m_engine->pipelineCache().getPipeline(state, pass);

    cmb.bindGraphicsPipeline(pipeline);
    cmb.bindDescriptorSets(pipeline, std::move(descriptors), std::move(offsets));

    if (m_index_count > 0u) cmb.drawIndexed(m_index_count, 1u, m_first_index);
    else
        cmb.draw(m_vertex_count);
}

////////////////////////////////////////
////////////////////////////////////////
void SubMesh::recomputeBoundingBox() const noexcept {
}
