#include "StormKitScene.hpp"

/////////// - StormKit::entities - ///////////
#include <storm/entities/MessageBus.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/NameComponent.hpp>

#include <storm/engine/FPSCamera.hpp>
#include <storm/engine/render/MaterialComponent.hpp>
#include <storm/engine/render/MeshComponent.hpp>
#include <storm/engine/render/RenderSystem.hpp>
#include <storm/engine/render/TransformComponent.hpp>

using namespace storm;
using storm::log::operator""_module;

struct Vertex {
    core::Vector3f position;
    core::Vector3f normal;
    core::Vector2f uv;
};

static constexpr auto BINDING_DESCRIPTIONS =
    std::array { render::VertexBindingDescription { .binding = 0, .stride = sizeof(Vertex) } };
static constexpr auto VERTEX_ATTRIBUTE_DESCRIPTIONS =
    std::array { render::VertexInputAttributeDescription { .location = 0,
                                                           .binding  = 0,
                                                           .format   = render::Format::Float3,
                                                           .Int32   = offsetof(Vertex, position) },
                 render::VertexInputAttributeDescription { .location = 1,
                                                           .binding  = 0,
                                                           .format   = render::Format::Float3,
                                                           .Int32   = offsetof(Vertex, normal) },
                 render::VertexInputAttributeDescription { .location = 2,
                                                           .binding  = 0,
                                                           .format   = render::Format::Float2,
                                                           .Int32   = offsetof(Vertex, uv) } };

////////////////////////////////////////
////////////////////////////////////////
StormKitScene::StormKitScene(engine::Engine &engine) : QObject{nullptr}, engine::State { engine } {
    const auto extent  = get<engine::Engine>().surface().extent();
    const auto extentf = extent.convertTo<core::Extentf>();

    m_camera = std::make_unique<engine::FPSCamera>(extentf);
    m_camera->setPosition({ 0.f, 0.f, -0.5f });
    m_camera->setRotation({ 0.f, 0.f, 0.f });

    m_render_system->setupDefaultPhongPasses();

    setCallback(Event::Update, [this](core::Secondf delta) { update(delta); });
}

////////////////////////////////////////
////////////////////////////////////////
StormKitScene::~StormKitScene() = default;

////////////////////////////////////////
////////////////////////////////////////
void StormKitScene::update(core::Secondf delta) {
    if(!m_initialized) {
        auto e               = m_world.makeEntity();
        auto &name_component = m_world.addComponent<engine::NameComponent>(e);
        name_component.name  = "MyMesh";

        auto &mesh_component          = m_world.addComponent<engine::MeshComponent>(e);
        mesh_component.size_of_vertex = sizeof(Vertex);
        mesh_component.vertices.push_back(Vertex { .position = { 0.f, 0.f, 0.f } });
        mesh_component.vertices.push_back(Vertex { .position = { 0.f, 1.f, 0.f } });
        mesh_component.vertices.push_back(Vertex { .position = { 1.f, 1.f, 0.f } });
        mesh_component.vertex_input_state.binding_descriptions         = BINDING_DESCRIPTIONS;
        mesh_component.vertex_input_state.input_attribute_descriptions = VERTEX_ATTRIBUTE_DESCRIPTIONS;

        m_world.addComponent<engine::MaterialComponent>(
            e,
            std::vector<std::string> { engine::RenderSystem::STORMKIT_PHONG_COLORPASS },
            engine::Material::PipelineState {});

        m_world.addComponent<engine::TransformComponent>(e);

        emit newEntity(e);

        m_initialized = true;

    }
    m_world.step(delta);
}
