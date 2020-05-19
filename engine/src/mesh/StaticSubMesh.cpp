/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/RingHardwareBuffer.hpp>

#include <storm/engine/mesh/StaticSubMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh::StaticSubMesh(Engine &engine,
                             core::UInt32 vertex_count,
                             core::UInt32 first_index,
                             core::UInt32 index_count) noexcept
    : Bindable { [&engine](Bindable::DescriptorSetVariant &layout) {
          auto l = engine.device().createDescriptorSetLayoutPtr();
          l->addBinding({ 0,
                          render::DescriptorType::Uniform_Buffer_Dynamic,
                          render::ShaderStage::Vertex,
                          1 });
          l->bake();

          layout = std::move(l);
      } },
      m_first_index { first_index }, m_index_count { index_count }, m_vertex_count {
          vertex_count
      } {
    const auto &device = engine.device();
    const auto &pool   = engine.descriptorPool();

    m_submesh_buffer = std::make_unique<RingHardwareBuffer>(1,
                                                            device,
                                                            render::HardwareBufferUsage::Uniform,
                                                            sizeof(core::Matrixf));

    const auto descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_submesh_buffer->buffer()),
                                   .range   = sizeof(core::Matrixf),
                                   .offset  = 0u };
    const auto descriptors =
        render::DescriptorStaticArray<1> { render::Descriptor { std::move(descriptor) } };

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());

    descriptorSet().update(descriptors);
}

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh::~StaticSubMesh() = default;

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh::StaticSubMesh(StaticSubMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh &StaticSubMesh::operator=(StaticSubMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void StaticSubMesh::flush() {
    if (!m_dirty) return;

    m_submesh_buffer->next();
    m_submesh_buffer->upload<core::Matrixf>({ &m_matrix, 1 });

    m_offset = m_submesh_buffer->currentOffset();

    m_dirty = false;
}
