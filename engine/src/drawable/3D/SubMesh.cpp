/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/3D/SubMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
SubMesh::SubMesh(Engine &engine, std::string name)
    : Drawable { engine }, m_name { std::move(name) } {
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
                     render::GraphicsPipelineState state,
                     [[maybe_unused]] float delta_time) {
    for (const auto &primitive : m_primitives) {
        primitive.material_instance->flush();

        auto primitive_state = state;

        primitive_state.vertex_input_state.binding_descriptions         = primitive.bindings;
        primitive_state.vertex_input_state.input_attribute_descriptions = primitive.attributes;
        primitive_state.rasterization_state = primitive.material_instance->m_rasterization_state;

        auto primitive_bindables = bindables;
        primitive_bindables.emplace_back(core::makeConstObserver(primitive.material_instance));

        auto descriptors = std::vector<render::DescriptorSetCRef> {};
        auto offsets     = std::vector<core::UOffset> {};

        descriptors.reserve(std::size(primitive_bindables));
        offsets.reserve(std::size(primitive_bindables));

        for (const auto &bindable : primitive_bindables) {
            descriptors.push_back(bindable->descriptorSet());

            const auto &offset = bindable->offset();
            if (offset.has_value()) offsets.push_back(offset.value());

            primitive_state.layout.descriptor_set_layouts.emplace_back(
                core::makeConstObserver(bindable->descriptorLayout()));
        }

        const auto &pipeline = m_engine->pipelineCache().getPipeline(primitive_state, pass);

        cmb.bindGraphicsPipeline(pipeline);
        cmb.bindDescriptorSets(pipeline, std::move(descriptors), std::move(offsets));

        if (primitive.index_count > 0u)
            cmb.drawIndexed(primitive.index_count,
                            1u,
                            primitive.first_index,
                            primitive.first_vertex);
        else
            cmb.draw(primitive.vertex_count, 0u, primitive.first_vertex);
    }
}

////////////////////////////////////////
////////////////////////////////////////
void SubMesh::recomputeBoundingBox() const noexcept {
    m_bounding_box.min = { 0.f, 0.f, 0.f };
    m_bounding_box.max = { 0.f, 0.f, 0.f };

    for (const auto &primitive : m_primitives) {
        m_bounding_box.min = core::min(m_bounding_box.min, primitive.bounding_box.min);
        m_bounding_box.max = core::max(m_bounding_box.max, primitive.bounding_box.max);
    }

    const auto extent = m_bounding_box.max - m_bounding_box.min;

    m_bounding_box.extent.width  = extent.x;
    m_bounding_box.extent.height = extent.y;
    m_bounding_box.extent.depth  = extent.z;
}
