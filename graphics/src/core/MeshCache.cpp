#include <storm/graphics/core/Mesh.hpp>
#include <storm/graphics/core/MeshCache.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
MeshCache::MeshCache(std::uint32_t image_count, const render::Device &device,
                     const render::DescriptorSetLayout &layout)
    : m_image_count{image_count}, m_device{&device},
      m_descriptor_set_layout(&layout) {

    m_graphics_queue = core::makeConstObserver(&m_device->graphicsQueue());

    m_descriptor_pool = m_device->createDescriptorPool(
        *m_descriptor_set_layout,
        {{render::DescriptorType::UniformBuffer, MAX_MESH_SETS}},
        MAX_MESH_SETS);
}

////////////////////////////////////////
////////////////////////////////////////
MeshCache::~MeshCache() = default;

////////////////////////////////////////
////////////////////////////////////////
MeshCache::MeshCache(MeshCache &&) = default;

////////////////////////////////////////
////////////////////////////////////////
MeshCache &MeshCache::operator=(MeshCache &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void MeshCache::addMesh(const Mesh &mesh) {
    static constexpr auto MODEL_DATA_SIZE = sizeof(Mesh::ModelData);

    auto mesh_data = MeshData{};
    mesh_data.model_set =
        std::move(m_descriptor_pool->allocateDescriptorSets(1)[0]);
    mesh_data.model_buffer = m_device->createUniformBuffer(
        MODEL_DATA_SIZE, render::MemoryProperty::Device_Local, true);
    mesh_data.model_transfert_cmbs =
        m_graphics_queue->createCommandBuffers(m_image_count);
    mesh_data.model_staging_buffers.reserve(m_image_count);

    const auto model_descriptor =
		storm::render::BufferDescriptor{.binding = 0,
										.buffer  = core::makeConstObserver(mesh_data.model_buffer),
                                        .range   = MODEL_DATA_SIZE};

    mesh_data.model_set->update({model_descriptor});

    for (auto i = 0u; i < m_image_count; ++i) {
        auto buffer = m_device->createStagingBuffer(MODEL_DATA_SIZE);
        buffer->upload<Mesh::ModelData>({&mesh.m_model_data, 1});

        auto &staging_buffer =
            mesh_data.model_staging_buffers.emplace_back(std::move(buffer));
        auto &cmb = mesh_data.model_transfert_cmbs[i];

        cmb->begin();
        cmb->copyBuffer(*staging_buffer, *mesh_data.model_buffer,
                        MODEL_DATA_SIZE);
        cmb->end();
        cmb->build();

        mesh_data.model_transfert_semaphores.emplace_back(
            m_device->createSemaphore());
    }

    m_meshes.emplace(core::makeObserver(&mesh), std::move(mesh_data));
}

////////////////////////////////////////
////////////////////////////////////////
std::vector<render::SemaphoreConstObserverPtr> MeshCache::drawMeshes(
    render::CommandBuffer &cmb, const render::GraphicsPipeline &pipeline,
    const render::DescriptorSet &camera_set,
    const render::DescriptorSet &material_set, std::uint32_t frame_id) const {
    auto semaphores = std::vector<render::SemaphoreConstObserverPtr>{};
    semaphores.reserve(std::size(m_meshes));

    for (const auto &[mesh, mesh_data] : m_meshes) {
        if (mesh->m_model_data_need_update) {
            updateCameraMeshData(*mesh, mesh_data, frame_id);

            semaphores.emplace_back(core::makeConstObserver(
                mesh_data.model_transfert_semaphores[frame_id]));

            mesh->m_model_data_need_update = false;
        }

        auto sets =
            std::vector{std::cref(camera_set), std::cref(*mesh_data.model_set),
                        std::cref(material_set)};
        for (const auto &custom_set : mesh->customSets())
            sets.emplace_back(*custom_set);

        cmb.bindDescriptorSets(pipeline, std::move(sets));

        mesh->draw(cmb);
    }

    return semaphores;
}

void MeshCache::updateCameraMeshData(const Mesh &mesh,
                                     const MeshData &mesh_data,
                                     std::uint32_t frame_id) const {
    auto &staging   = *mesh_data.model_staging_buffers[frame_id];
    auto &semaphore = mesh_data.model_transfert_semaphores[frame_id];
    auto &cmb		= mesh_data.model_transfert_cmbs[frame_id];

    staging.upload<Mesh::ModelData>({&mesh.m_model_data, 1});

    m_graphics_queue->submit(core::makeConstObserverArray(cmb), {},
                             core::makeConstObserverArray(semaphore));
}
