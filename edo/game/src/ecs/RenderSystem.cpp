#include "RenderSystem.hpp"
#include "CameraComponent.hpp"
#include "CustomShaderDataComponent.hpp"
#include "StaticGeometryComponent.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

#include <storm/graphics/core/Mesh.hpp>
#include <storm/graphics/core/Renderer.hpp>
#include <storm/graphics/framegraph/FrameGraphBuilder.hpp>
#include <storm/graphics/framegraph/FramePass.hpp>

#include <storm/entities/EntityManager.hpp>
#include <storm/entities/MessageBus.hpp>

#ifdef STORM_OS_MACOS
static const auto RESOURCE_DIR =
    std::filesystem::current_path() / "../Resources/";
#else
static const auto RESOURCE_DIR = std::filesystem::current_path();
#endif

struct ColorPassData {
    storm::graphics::FrameGraphTextureResourceObserverPtr backbuffer  = nullptr;
    storm::graphics::FrameGraphTextureResourceObserverPtr depthbuffer = nullptr;
};

////////////////////////////////////////
////////////////////////////////////////
RenderSystem::RenderSystem(storm::entities::EntityManager &manager,
                           const storm::render::Device &device,
                           storm::render::Surface &surface)
    : storm::entities::System{manager, 22, {CameraComponent::TYPE}},
      m_device{&device}, m_surface{&surface} {
    m_renderer =
        std::make_unique<storm::graphics::Renderer>(*m_device, *m_surface);

    initFrameGraph();
}

////////////////////////////////////////
////////////////////////////////////////
RenderSystem::~RenderSystem() = default;

////////////////////////////////////////
////////////////////////////////////////
RenderSystem::RenderSystem(RenderSystem &&) = default;

////////////////////////////////////////
////////////////////////////////////////
RenderSystem &RenderSystem::operator=(RenderSystem &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void RenderSystem::update([[maybe_unused]] std::uint64_t delta) {
    for (auto entity : m_entities) {
        const auto &camera_component =
            m_manager.get().getComponent<CameraComponent>(entity);

        m_renderer->setProjectionMatrix(camera_component.projection_matrix);
        m_renderer->setViewMatrix(camera_component.view_matrix);
    }

    for (auto &[entity, mesh] : m_meshes) {
        if (m_manager.get().hasComponent<TransformComponent>(entity)) {
            const auto &transform_component =
                m_manager.get().getComponent<TransformComponent>(entity);

            mesh->setModelMatrix(transform_component.transform_matrix);
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
void RenderSystem::postUpdate() {
}

void RenderSystem::present() {
    m_renderer->present();
}

void RenderSystem::onMessageReceived(const storm::entities::Message &message) {
    if (message.id == storm::entities::EntityManager::ADDED_ENTITY_MESSAGE_ID) {
        for (auto entity : message.entities) {
            if (m_manager.get().hasComponent<StaticGeometryComponent>(entity)) {
                const auto &geometry_component =
                    m_manager.get().getComponent<StaticGeometryComponent>(
                        entity);

                auto mesh = std::make_unique<storm::graphics::Mesh>(*m_device);
                mesh->setVertexData(geometry_component.vertices,
                                    geometry_component.vertex_count);

                if (geometry_component.index_count > 0 &&
                    geometry_component.indices.has_value())
                    mesh->setIndexData(geometry_component.indices.value(),
                                       geometry_component.index_count);

                if (m_manager.get().hasComponent<TransformComponent>(entity)) {
                    const auto &transform_component =
                        m_manager.get().getComponent<TransformComponent>(
                            entity);

                    mesh->setModelMatrix(transform_component.transform_matrix);
                }

                if (m_manager.get().hasComponent<CustomShaderDataComponent>(
                        entity)) {
                    const auto &custom_shader_data_component =
                        m_manager.get().getComponent<CustomShaderDataComponent>(
                            entity);

                    for (const auto &set : custom_shader_data_component.sets)
                        mesh->addCustomSet(set);
                }

                auto [it, _] = m_meshes.emplace(entity, std::move(mesh));

                if (m_manager.get().hasComponent<TextureComponent>(entity)) {
                    const auto &texture_component =
                        m_manager.get().getComponent<TextureComponent>(entity);

                    auto material				 = storm::graphics::Material{};
                    material.diffuse_map		 = texture_component.texture;
                    material.diffuse_map_sampler = texture_component.sampler;

                    m_renderer->addMesh(*geometry_component.state, material,
                                        *it->second);
                } else
                    m_renderer->addMesh(*geometry_component.state, *it->second);
            }
        }

    } else if (message.id ==
               storm::entities::EntityManager::REMOVED_ENTITY_MESSAGE_ID) {
        // TODO
    }
}

////////////////////////////////////////
////////////////////////////////////////
void RenderSystem::initFrameGraph() {
    auto &framegraph_builder = m_renderer->frameBuilder();
    framegraph_builder.setBackbufferName("backbuffer");

    auto &color_pass = framegraph_builder.addPass<ColorPassData>(
        "ColorPass",
        [&](auto &builder, auto &data) {
            data.backbuffer = builder.create(
                "backbuffer", storm::graphics::FrameGraphTextureDesc{
                                  .extent = m_surface->extent(),
                                  .format = m_surface->pixelFormat()});
        },
        [&](const auto &data, auto &render_pass, auto &framebuffer,
            auto &command_buffer) {
            command_buffer.beginRenderPass(render_pass, framebuffer);

            m_renderer->drawMeshes(command_buffer, render_pass);

            command_buffer.endRenderPass();
        });

    color_pass.setCullImune(true);
}
