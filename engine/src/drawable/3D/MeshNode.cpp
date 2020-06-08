/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/3D/MeshNode.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
MeshNode::MeshNode(Engine &engine, std::string name)
    : Drawable { engine }, m_name { std::move(name) } {
    const auto &device = m_engine->device();
    const auto &pool   = m_engine->descriptorPool();

    m_mesh_data_buffer =
        device.createUniformBufferPtr(sizeof(Data), render::MemoryProperty::Device_Local, true);

    const auto descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_mesh_data_buffer),
                                   .range   = sizeof(Data),
                                   .offset  = 0u };
    const auto descriptors =
        render::DescriptorStaticArray<1> { render::Descriptor { std::move(descriptor) } };

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());
    descriptorSet().update(descriptors);
}

////////////////////////////////////////
////////////////////////////////////////
MeshNode::~MeshNode() = default;

////////////////////////////////////////
////////////////////////////////////////
MeshNode::MeshNode(MeshNode &&) = default;

////////////////////////////////////////
////////////////////////////////////////
MeshNode &MeshNode::operator=(MeshNode &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void MeshNode::render(render::CommandBuffer &cmb,
                      const render::RenderPass &pass,
                      std::vector<BindableBaseConstObserverPtr> bindables,
                      render::GraphicsPipelineState state) {
    if (m_bitset.test(TRANSFORM_DIRTY)) flush();

    for (const auto &primitive : m_primitives) {
        primitive.material_instance->flush();

        state.vertex_input_state.binding_descriptions         = primitive.bindings;
        state.vertex_input_state.input_attribute_descriptions = primitive.attributes;
        state.rasterization_state = primitive.material_instance->m_rasterization_state;

        bindables.emplace_back(core::makeConstObserver(this));
        bindables.emplace_back(core::makeConstObserver(primitive.material_instance));

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

        if (primitive.index_count > 0u)
            cmb.drawIndexed(primitive.index_count, 1u, primitive.first_index);
        else
            cmb.draw(primitive.vertex_count, 0u, primitive.first_vertex);
    }
}

////////////////////////////////////////
////////////////////////////////////////
void MeshNode::recomputeBoundingBox() const noexcept {
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

////////////////////////////////////////
////////////////////////////////////////
void MeshNode::flush() {
    const auto &device = m_engine->device();

    auto staging_buffer = device.createStagingBuffer(sizeof(Data));
    staging_buffer.upload<Data>({ &m_data, 1 });

    auto cmb = device.graphicsQueue().createCommandBuffer();
    cmb.begin(true);
    cmb.copyBuffer(staging_buffer, *m_mesh_data_buffer, sizeof(Data));
    cmb.end();

    cmb.build();

    auto fence = device.createFence();
    cmb.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}
