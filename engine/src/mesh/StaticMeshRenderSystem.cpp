/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>
#include <storm/entities/MessageBus.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/TransformComponent.hpp>

#include <storm/engine/mesh/StaticMeshRenderSystem.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>
#include <storm/engine/mesh/StaticMeshComponent.hpp>

#include <storm/engine/material/Material.hpp>
#include <storm/engine/material/MaterialComponent.hpp>

#include "../core/TransformComponentCache.hpp"

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
StaticMeshRenderSystem::StaticMeshRenderSystem(entities::EntityManager &manager, Engine &engine)
    : entities::System { manager, 0, { StaticMeshComponent::TYPE, MaterialComponent::TYPE } },
      m_engine { &engine } {
    m_default_transform = std::make_unique<Transform>(*m_engine);

    const auto layout = [this]() {
        auto layout = render::DescriptorSetLayout { m_engine->device() };
        layout.addBinding(
            { 0, render::DescriptorType::Uniform_Buffer_Dynamic, render::ShaderStage::Vertex, 1 });
        layout.bake();

        return layout;
    }();

    const auto descriptors =
        std::array { render::Descriptor { m_default_transform->transformDescriptor() } };

    const auto &pool = m_engine->descriptorPool();

    m_default_transform_descriptor_set = pool.allocateDescriptorSetPtr(layout);
    m_default_transform_descriptor_set->update(descriptors);
}

/////////////////////////////////////
/////////////////////////////////////
StaticMeshRenderSystem::~StaticMeshRenderSystem() = default;

/////////////////////////////////////
/////////////////////////////////////
StaticMeshRenderSystem::StaticMeshRenderSystem(StaticMeshRenderSystem &&) = default;

/////////////////////////////////////
/////////////////////////////////////
StaticMeshRenderSystem &StaticMeshRenderSystem::operator=(StaticMeshRenderSystem &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void StaticMeshRenderSystem::update(core::UInt64 delta) noexcept {
}

/////////////////////////////////////
/////////////////////////////////////
void StaticMeshRenderSystem::render(render::CommandBuffer &cmb,
                                    std::string_view pass_name,
                                    const render::RenderPass &render_pass,
                                    std::vector<render::DescriptorSetCRef> descriptor_sets,
                                    std::vector<core::UOffset> dynamic_offsets) {
    for (auto entity : m_entities) {
        auto per_entity_descriptor_sets = descriptor_sets;
        auto per_entity_dynamic_offsets = dynamic_offsets;

        const auto &mesh_component = m_manager->getComponent<StaticMeshComponent>(entity);
        auto &material_component   = m_manager->getComponent<MaterialComponent>(entity);

        STORM_EXPECTS(mesh_component.mesh != nullptr);
        STORM_EXPECTS(!std::empty(material_component.materials));

        const auto &passes = mesh_component.passes_to_render_in;
        const auto is_rendered_in_pass =
            std::find_if(std::cbegin(passes), std::cend(passes), [pass_name](const auto &str) {
                return str == pass_name;
            }) != std::cend(passes);

        if (!is_rendered_in_pass) continue;
        if (m_manager->hasComponent<TransformComponent>(entity)) {
            auto &transform_component = m_manager->getComponent<TransformComponent>(entity);
            STORM_EXPECTS(transform_component.transform != nullptr);

            transform_component.transform->ensureUpdated();

            per_entity_descriptor_sets.emplace_back(*transform_component.cache->descriptor_set);
            per_entity_dynamic_offsets.emplace_back(
                transform_component.transform->transformOffset());
        } else {
            per_entity_descriptor_sets.emplace_back(*m_default_transform_descriptor_set);
            per_entity_dynamic_offsets.emplace_back(0u);
        }

        const auto &vertex_buffer = mesh_component.mesh->vertexBuffer();

        cmb.bindVertexBuffers({ vertex_buffer }, { 0 });

        if (mesh_component.mesh->hasIndices()) {
            const auto &index_buffer = mesh_component.mesh->indexBuffer();
            cmb.bindIndexBuffer(index_buffer, 0, mesh_component.mesh->useLargeIndices());
        }

        auto last_state_hash  = core::Hash64 { 0u };
        auto current_pipeline = render::GraphicsPipelineConstObserverPtr {};
        for (const auto &submesh : mesh_component.mesh->subMeshes()) {
            STORM_EXPECTS(submesh.materialID() < std::size(material_component.materials));

            auto per_material_descriptor_sets = per_entity_descriptor_sets;
            auto per_material_dynamic_offsets = per_entity_dynamic_offsets;

            auto &material = material_component.materials[submesh.materialID()];
            material.ensureIsUpdated();

            per_material_descriptor_sets.emplace_back(material.descriptorSet());

            const auto &pipeline_state = material.pipelineState();

            const auto hash = std::hash<render::GraphicsPipelineState> {}(pipeline_state);
            if (hash != last_state_hash) {
                auto &pipeline_cache = m_engine->pipelineCache();

                const auto &pipeline = pipeline_cache.getPipeline(pipeline_state, render_pass);
                cmb.bindGraphicsPipeline(pipeline);

                last_state_hash  = hash;
                current_pipeline = core::makeConstObserver(pipeline);
            }

            cmb.pushConstants(*current_pipeline,
                              render::ShaderStage::Fragment,
                              material.pushConstantsData());

            if (!std::empty(per_material_descriptor_sets))
                cmb.bindDescriptorSets(*current_pipeline,
                                       std::move(per_material_descriptor_sets),
                                       std::move(per_material_dynamic_offsets));

            if (submesh.isIndexed()) {
                const auto index_count = submesh.indexCount();

                cmb.drawIndexed(index_count, 1u, submesh.firstIndex());
            } else {
                const auto vertex_count = submesh.vertexCount();

                cmb.draw(vertex_count);
            }
        }
    }
}

/////////////////////////////////////
/////////////////////////////////////
void StaticMeshRenderSystem::onMessageReceived(const entities::Message &message) {
    const auto &layout = m_engine->pipelineBuilder().transformLayout();

    const auto &pool = m_engine->descriptorPool();

    if (message.id == entities::EntityManager::ADDED_ENTITY_MESSAGE_ID) {
        for (const auto entity : message.entities)
            if (m_manager->hasComponent<TransformComponent>(entity)) {
                auto &transform_component = m_manager->getComponent<TransformComponent>(entity);

                if (transform_component.cache == nullptr) {
                    transform_component.cache = std::make_unique<TransformComponentCache>();

                    const auto descriptors = std::array { render::Descriptor {
                        transform_component.transform->transformDescriptor() } };

                    transform_component.cache->descriptor_set =
                        pool.allocateDescriptorSetPtr(layout);
                    transform_component.cache->descriptor_set->update(descriptors);
                }
            }
    }
}
