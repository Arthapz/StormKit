#include <storm/render/core/CommandBuffer.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/engine/scene/RenderQueue.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
RenderQueue::RenderQueue(const render::Device &device, std::string name) : m_device { &device } {
    m_debug_marker = fmt::format("RenderQueue - {}", name);
}

////////////////////////////////////////
////////////////////////////////////////
RenderQueue::~RenderQueue() = default;

////////////////////////////////////////
////////////////////////////////////////
RenderQueue::RenderQueue(RenderQueue &&) = default;

////////////////////////////////////////
////////////////////////////////////////
RenderQueue &RenderQueue::operator=(RenderQueue &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void RenderQueue::clear() {
}

////////////////////////////////////////
////////////////////////////////////////
void RenderQueue::addMesh(const StaticMesh &mesh) {
    m_meshes.emplace_back(&mesh);
}

////////////////////////////////////////
////////////////////////////////////////
void RenderQueue::buildCommandBuffer(std::vector<render::DescriptorSetCRef> descriptor_sets,
                                     std::vector<core::UOffset> offsets) {
}

////////////////////////////////////////
////////////////////////////////////////
/*void RenderQueue::render(render::CommandBuffer &cmb,
                         const render::RenderPass &renderpass,
                         const render::DescriptorSet &descriptor_set,
                         std::vector<core::UOffset> offsets) {
    const auto &pipeline = m_pipeline_cache->getPipeline(m_pipeline_state, renderpass);
    cmb.beginDebugRegion(m_debug_marker, core::RGBColorDef::Lime<float>);

    cmb.bindGraphicsPipeline(pipeline);

    for (const auto &[mesh, mesh_descriptor_set] : m_meshes) {
        auto offsets_mesh = offsets;
        offsets_mesh.emplace_back(mesh->currentTransformOffset());

        cmb.bindDescriptorSets(
            pipeline, {descriptor_set, *mesh_descriptor_set}, std::move(offsets_mesh));

        const auto instance_count = mesh->instanceCount();
        for (const auto &submesh : mesh->subMeshes()) {
            cmb.bindVertexBuffers({submesh.vertexBuffer()}, {0});
            if (submesh.isIndexed()) {
                cmb.bindIndexBuffer(submesh.indexBuffer(), 0, submesh.useLargeIndices());
                cmb.drawIndexed(submesh.indexCount(), instance_count);
            } else
                cmb.draw(submesh.vertexCount(), instance_count);
        }
    }
    cmb.endDebugRegion();
}*/
