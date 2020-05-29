/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/Mesh.hpp>
#include <storm/engine/drawable/SubMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Engine &engine, const Material &material, std::string name)
    : Drawable { engine }, m_name { std::move(name) }, m_material { &material } {
    const auto buffering_count = m_engine->surface().bufferingCount();
    auto &device               = m_engine->device();
    const auto &pool           = m_engine->descriptorPool();
    const auto &queue          = [&device]() -> const render::Queue & {
        if (device.hasAsyncTransfertQueue()) return device.asyncTransfertQueue();

        return device.graphicsQueue();
    }();

    m_mesh_data_buffer = std::make_unique<RingHardwareBuffer>(buffering_count,
                                                              device,
                                                              render::HardwareBufferUsage::Uniform,
                                                              sizeof(core::Matrixf));

    const auto descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_mesh_data_buffer->buffer()),
                                   .range   = sizeof(core::Matrixf),
                                   .offset  = 0u };
    const auto descriptors =
        render::DescriptorStaticArray<1> { render::Descriptor { std::move(descriptor) } };

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());
    descriptorSet().update(descriptors);

    m_update_cmb = queue.createCommandBufferPtr();

    m_transform = m_engine->createTransformPtr();
}

////////////////////////////////////////
////////////////////////////////////////
Mesh::~Mesh() = default;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(Mesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Mesh &Mesh::operator=(Mesh &&) = default;

void Mesh::setVertexArray(VertexArray array,
                          render::VertexInputAttributeDescriptionArray attributes,
                          render::VertexBindingDescriptionArray bindings) {
    STORM_EXPECTS(!std::empty(array) && !std::empty(attributes) && !std::empty(bindings));

    auto &device = m_engine->device();

    m_vertex_array = std::move(array);

    m_vertex_buffer = device.createVertexBufferPtr(std::size(m_vertex_array) * sizeof(float),
                                                   render::MemoryProperty::Device_Local,
                                                   true);
    device.setObjectName(*m_vertex_buffer, fmt::format("{}:VertexArray", m_name));

    m_attributes = std::move(attributes);
    m_bindings   = std::move(bindings);

    m_dirty_vertices = true;
}

void Mesh::setIndexArray(IndexArrayProxy array) {
    auto &device = m_engine->device();

    m_index_array = std::move(array);

    auto staging_buffer = render::HardwareBufferOwnedPtr {};

    std::visit(
        [this, &device](const auto &index_array) {
            STORM_EXPECTS(!std::empty(index_array));

            using IndexArrayType     = std::remove_reference_t<decltype(index_array)>;
            using IndexArrayTypeType = typename IndexArrayType::value_type;

            const auto buffer_size = sizeof(IndexArrayTypeType) * std::size(index_array);

            m_index_buffer = device.createIndexBufferPtr(buffer_size,
                                                         render::MemoryProperty::Device_Local,
                                                         true);
            device.setObjectName(*m_index_buffer, fmt::format("{}:VertexArray", m_name));
        },
        m_index_array);
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::render(storm::render::CommandBuffer &cmb,
                  const storm::render::RenderPass &pass,
                  std::vector<BindableBaseConstObserverPtr> bindables,
                  storm::render::GraphicsPipelineState state) {
    flush();
    m_transform->flush();

    const auto indexed = m_index_buffer != nullptr;

    cmb.bindVertexBuffers({ *m_vertex_buffer }, { 0u });

    if (indexed) { cmb.bindIndexBuffer(*m_index_buffer, 0, useLargeIndices()); }

    bindables.emplace_back(core::makeConstObserver(m_transform));
    bindables.emplace_back(core::makeConstObserver(this));

    state.vertex_input_state.binding_descriptions         = m_bindings;
    state.vertex_input_state.input_attribute_descriptions = m_attributes;
    state.shader_state                                    = m_material->m_data.shader_state;

    for (auto &submesh : m_submeshes) { submesh.render(cmb, pass, bindables, state); }
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::recomputeBoundingBox() const noexcept {
    m_bounding_box.min = { 0.f, 0.f, 0.f };
    m_bounding_box.max = { 0.f, 0.f, 0.f };

    for (const auto &submesh : m_submeshes) {
        const auto &bounding_box = submesh.boundingBox();

        m_bounding_box.min = core::min(m_bounding_box.min, bounding_box.min);
        m_bounding_box.max = core::max(m_bounding_box.max, bounding_box.max);
    }

    const auto extent = m_bounding_box.max - m_bounding_box.min;

    m_bounding_box.extent.width  = extent.x;
    m_bounding_box.extent.height = extent.y;
    m_bounding_box.extent.depth  = extent.z;
}

void Mesh::flush() noexcept {
    if (GSL_LIKELY(!m_dirty_vertices && !m_dirty_indices && !m_matrix_dirty)) return;

    if (m_matrix_dirty) {
        m_mesh_data_buffer->next();
        m_mesh_data_buffer->upload<core::Matrixf>({ &m_matrix, 1 });

        m_offset = m_mesh_data_buffer->currentOffset();

        m_matrix_dirty = false;
    }

    if (m_dirty_vertices || m_dirty_indices) {
        auto &device = m_engine->device();
        auto fence   = device.createFence();

        auto vertex_byte_count = 0u;
        auto index_byte_count  = 0u;
        auto indices_data      = core::ByteConstSpan {};

        if (m_dirty_vertices) vertex_byte_count = std::size(m_vertex_array) * sizeof(float);

        if (m_dirty_indices) {
            index_byte_count = std::visit(
                [&indices_data](const auto &index_array) {
                    STORM_EXPECTS(!std::empty(index_array));

                    using IndexArrayType     = std::remove_reference_t<decltype(index_array)>;
                    using IndexArrayTypeType = typename IndexArrayType::value_type;

                    const auto buffer_size = sizeof(IndexArrayTypeType) * std::size(index_array);

                    indices_data = core::ByteConstSpan { reinterpret_cast<const core::Byte *>(
                                                             std::data(index_array)),
                                                         buffer_size };

                    return buffer_size;
                },
                m_index_array);
        }

        auto staging_buffer = device.createStagingBuffer(vertex_byte_count + index_byte_count);

        if (m_dirty_vertices) staging_buffer.upload<float>(std::data(m_vertex_array), 0u);
        if (m_dirty_indices)
            staging_buffer.upload<const core::Byte>(indices_data, vertex_byte_count);

        m_update_cmb->reset();
        m_update_cmb->begin(true);
        if (m_dirty_vertices)
            m_update_cmb->copyBuffer(staging_buffer, *m_vertex_buffer, vertex_byte_count);
        if (m_dirty_indices)
            m_update_cmb->copyBuffer(staging_buffer,
                                     *m_index_buffer,
                                     index_byte_count,
                                     vertex_byte_count);
        m_update_cmb->end();
        m_update_cmb->build();

        m_update_cmb->submit({}, {}, core::makeObserver(fence));

        fence.wait();

        m_dirty_vertices = false;
        m_dirty_indices  = false;
    }
}
