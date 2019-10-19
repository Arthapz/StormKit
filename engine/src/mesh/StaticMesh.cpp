/////////// - StormKit::core - ///////////
#include <storm/core/TypeTraits.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/mesh/Model.hpp>
#include <storm/engine/mesh/StaticMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
StaticMesh::StaticMesh(const Engine &engine,
                       render::TaggedVertexInputAttributeDescriptionArray vertex_attributes,
                       render::VertexBindingDescriptionArray vertex_bindings) noexcept
    : m_engine { &engine }, m_tagged_vertex_attributes { std::move(vertex_attributes) },
      m_vertex_bindings { std::move(vertex_bindings) } {
    m_vertex_attributes.reserve(std::size(m_tagged_vertex_attributes));
    for (const auto &[_, attr] : m_tagged_vertex_attributes) m_vertex_attributes.emplace_back(attr);

    m_index_array = IndexArray {};
}

////////////////////////////////////////
////////////////////////////////////////
StaticMesh::~StaticMesh() = default;

////////////////////////////////////////
////////////////////////////////////////
StaticMesh::StaticMesh(StaticMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
StaticMesh &StaticMesh::operator=(StaticMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void StaticMesh::setVertexArray(VertexArray array) {
    STORM_EXPECTS(!std::empty(array));

    auto &device = m_engine->device();

    m_vertex_array = std::move(array);

    const auto byte_count = m_vertex_array.size() * sizeof(float);

    auto staging_buffer = device.createStagingBuffer(byte_count);
    m_vertex_buffer =
        device.createVertexBufferPtr(byte_count, render::MemoryProperty::Device_Local, true);

    staging_buffer.upload<float>(m_vertex_array.data(), 0u);

    auto fence = device.createFence();

    auto cmb = [&device]() {
        if (device.hasAsyncTransfertQueue())
            return device.asyncTransfertQueue().createCommandBuffer();

        return device.graphicsQueue().createCommandBuffer();
    }();

    cmb.begin(true);
    cmb.copyBuffer(staging_buffer, *m_vertex_buffer, byte_count);
    cmb.end();
    cmb.build();

    cmb.submit({}, {}, core::makeObserver(fence));
    device.waitForFence(fence);
}

////////////////////////////////////////
////////////////////////////////////////
void StaticMesh::setIndexArray(IndexArrayProxy array) {
    auto &device = m_engine->device();

    m_index_array = std::move(array);

    auto staging_buffer = render::HardwareBufferOwnedPtr {};

    std::visit(
        [this, &device, &staging_buffer](const auto &index_array) {
            STORM_EXPECTS(!std::empty(index_array));

            using IndexArrayType     = std::remove_reference_t<decltype(index_array)>;
            using IndexArrayTypeType = typename IndexArrayType::value_type;

            const auto buffer_size = sizeof(IndexArrayTypeType) * std::size(index_array);

            m_index_buffer = device.createIndexBufferPtr(buffer_size,
                                                         render::MemoryProperty::Device_Local,
                                                         true);
            staging_buffer = device.createStagingBufferPtr(buffer_size);
            staging_buffer->upload<IndexArrayTypeType>(index_array, 0u);
            staging_buffer->flush(0, buffer_size);

            return buffer_size;
        },
        m_index_array);

    auto fence = device.createFence();
    auto cmb   = [&device]() {
        if (device.hasAsyncTransfertQueue())
            return device.asyncTransfertQueue().createCommandBuffer();

        return device.graphicsQueue().createCommandBuffer();
    }();

    cmb.begin(true);
    cmb.copyBuffer(*staging_buffer, *m_index_buffer, m_index_buffer->byteCount());
    cmb.end();
    cmb.build();

    cmb.submit({}, {}, core::makeObserver(fence));
    device.waitForFence(fence);
}

////////////////////////////////////////
////////////////////////////////////////
void StaticMesh::computeBoundingBox() const noexcept {
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
