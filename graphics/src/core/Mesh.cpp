#include <storm/core/TypeTraits.hpp>

#include <storm/graphics/core/Mesh.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/sync/Fence.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
Mesh::Mesh(const render::Device &device) : m_device{&device} {
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

////////////////////////////////////////
////////////////////////////////////////
void Mesh::setVertexData(VertexArrayProxy vertex_data,
						 std::uint32_t vertex_count) {
	STORM_EXPECTS(vertex_count != 0u);

	m_vertex_count = vertex_count;

	auto staging_buffer = render::HardwareBufferOwnedPtr{};

	m_vertex_buffer_size = std::visit(
		[this, &staging_buffer](auto &vertex_array) {
			using VertexArrayType =
				std::remove_reference_t<decltype(vertex_array)>;

			const auto buffer_size =
				ValueTypeSize<VertexArrayType> * std::size(vertex_array);

			m_vertex_buffer = m_device->createVertexBuffer(
				buffer_size, render::MemoryProperty::Device_Local, true);
			staging_buffer = m_device->createStagingBuffer(buffer_size);
			staging_buffer->upload<ValueType<VertexArrayType>>(vertex_array);

			return buffer_size;
		},
		vertex_data);

	auto fence = m_device->createFence();
	auto cmb   = m_device->transfertQueue().createCommandBuffer();

	cmb->begin(true);
	cmb->copyBuffer(*staging_buffer, *m_vertex_buffer, m_vertex_buffer_size);
	cmb->end();
	cmb->build();

	m_device->transfertQueue().submit(core::makeConstObserverArray(cmb), {}, {},
									  core::makeObserver(fence));
	m_device->waitForFence(*fence);
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::setIndexData(IndexArrayProxy index_data, std::uint32_t index_count) {
    STORM_EXPECTS(index_count != 0u);

    m_index_count = index_count;

    auto staging_buffer = render::HardwareBufferOwnedPtr{};

    m_index_buffer_size = std::visit(
        [this, &staging_buffer](auto &index_array) {
            using IndexArrayType =
                std::remove_reference_t<decltype(index_array)>;

            if constexpr (sizeof(IndexArrayType) == sizeof(std::uint32_t))
                m_use_large_indices = true;

            const auto buffer_size =
                ValueTypeSize<IndexArrayType> * std::size(index_array);

            m_index_buffer = m_device->createIndexBuffer(
                buffer_size, render::MemoryProperty::Device_Local, true);
            staging_buffer = m_device->createStagingBuffer(buffer_size);
            staging_buffer->upload<ValueType<IndexArrayType>>(index_array);

            return buffer_size;
        },
        index_data);

    auto fence = m_device->createFence();
    auto cmb   = m_device->transfertQueue().createCommandBuffer();

    cmb->begin(true);
    cmb->copyBuffer(*staging_buffer, *m_index_buffer, m_index_buffer_size);
    cmb->end();
    cmb->build();

    m_device->transfertQueue().submit(core::makeConstObserverArray(cmb), {}, {},
                                      core::makeObserver(fence));
    m_device->waitForFence(*fence);
}

////////////////////////////////////////
////////////////////////////////////////
void Mesh::draw(render::CommandBuffer &cmb) const {
	STORM_EXPECTS(m_vertex_buffer != nullptr);

    const auto use_index_buffer = m_index_buffer != nullptr;

	cmb.bindVertexBuffers({*m_vertex_buffer}, {0});
    if (!use_index_buffer)
		cmb.draw(m_vertex_count);
	else {
		cmb.bindIndexBuffer(*m_index_buffer, 0, m_use_large_indices);
		cmb.drawIndexed(m_index_count);
	}
}
