#include "MainState.hpp"
#include "Log.hpp"

/////////// - StormKit::entities - ///////////
#include <storm/entities/MessageBus.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>
#include <storm/engine/NameComponent.hpp>

#include <storm/engine/render/TransformComponent.hpp>
#include <storm/engine/render/MaterialComponent.hpp>
#include <storm/engine/render/DrawableComponent.hpp>

#include <storm/engine/render/3D/FPSCamera.hpp>
#include <storm/engine/render/3D/PbrRenderSystem.hpp>
#include <storm/engine/render/3D/PbrMesh.hpp>

using namespace storm;

static constexpr auto ROTATE_ANGLE = 5.f;

class RotationSystem: public entities::System {
  public:
    explicit RotationSystem(entities::EntityManager &manager)
        : System { manager, 1, { engine::TransformComponent::TYPE } } {}
    ~RotationSystem() override = default;

    RotationSystem(RotationSystem &&) noexcept = default;
    RotationSystem &operator=(RotationSystem &&) noexcept = default;

    void update(core::Secondf delta) override {
        for (auto e : m_entities) {
            auto &transform_component = m_manager->getComponent<engine::TransformComponent>(e);

            //transform_component.transform.rotateRoll(ROTATE_ANGLE * delta.count());
        }
    }

  protected:
    void onMessageReceived(const entities::Message &message) override {}
};

////////////////////////////////////////
////////////////////////////////////////
MainState::MainState(core::StateManager &owner,
                     engine::Engine &engine,
                     const window::Window &window)
    : State { owner, engine }, m_input_handler { window } {
    m_input_handler.setMousePositionOnWindow(
        core::Position2i { window.size().width / 2, window.size().height / 2 });

    const auto extent = State::engine().surface().extent();

    m_render_system = &m_world.addSystem<engine::PbrRenderSystem>(State::engine());
    m_world.addSystem<RotationSystem>();

    m_camera = engine::FPSCamera::allocateOwned(extent);
    m_camera->setPosition({ 0.f, 0.f, -3.f });

    m_render_system->setCamera(*m_camera);

    auto vertices = engine::VertexArray {};
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 0.f, 0.f, 0.f }, .uv = {0.f, 0.f} });
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 1.f, 0.f, 0.f }, .uv = {1.f, 0.f} });
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 1.f, 1.f, 0.f }, .uv = {1.f, 1.f} });
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 0.f, 1.f, 0.f }, .uv = {0.f, 1.f} });

    vertices.push_back(engine::PbrMesh::Vertex { .position = { 0.f, 0.f, 1.f }, .uv = {0.f, 0.f} });
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 1.f, 0.f, 1.f }, .uv = {1.f, 0.f} });
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 1.f, 1.f, 1.f }, .uv = {1.f, 1.f} });
    vertices.push_back(engine::PbrMesh::Vertex { .position = { 0.f, 1.f, 1.f }, .uv = {0.f, 1.f} });

    auto indices = engine::LargeIndexArray { 0, 1, 3, 3, 1, 2,
                                             1, 5, 2, 2, 5, 6,
                                             5, 4, 6, 6, 4, 7,
                                             4, 0, 7, 7, 0, 3,
                                             3, 2, 7, 7, 2, 6,
                                             4, 5, 0, 0, 5, 1 };

    auto mesh = engine::PbrMesh::allocateOwned();
    mesh->setVertices(std::move(vertices));
    mesh->setIndices(std::move(indices));

    auto e               = m_world.makeEntity();
    auto &name_component = m_world.addComponent<engine::NameComponent>(e);
    name_component.name  = "MyMesh";

    auto &drawable_component = m_world.addComponent<engine::DrawableComponent>(e);
    drawable_component.drawable = std::move(mesh);

    auto &transform_component = m_world.addComponent<engine::TransformComponent>(e);

}

////////////////////////////////////////
////////////////////////////////////////
MainState::~MainState() = default;

////////////////////////////////////////
////////////////////////////////////////
MainState::MainState(MainState &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
MainState &MainState::operator=(MainState &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
void MainState::update(core::Secondf delta) {
    auto inputs = engine::FPSCamera::Inputs{
        .up    = m_input_handler.isKeyPressed(window::Key::Z),
        .down  = m_input_handler.isKeyPressed(window::Key::S),
        .right = m_input_handler.isKeyPressed(window::Key::D),
        .left  = m_input_handler.isKeyPressed(window::Key::Q),
    };

    const auto extent = State::engine().surface().extent();
    const auto position = [&inputs, &extent, this]() {
        auto position = m_input_handler.getMousePositionOnWindow();

        if (position->x <= 5 || position->x > (extent.width - 5)) {
            position->x         = extent.width / 2;
            inputs.mouse_ignore = true;
        }
        if (position->y <= 5 || position->y > (extent.height - 5)) {
            position->y         = extent.height / 2;
            inputs.mouse_ignore = true;
        }

        m_input_handler.setMousePositionOnWindow(position);

        return position;
    }();

    inputs.mouse_updated = true;
    inputs.x_mouse       = static_cast<float>(position->x);
    inputs.y_mouse       = static_cast<float>(position->y);

    m_camera->feedInputs(inputs);
    m_camera->update(delta);

    auto &frame = engine().beginFrame();

    m_world.step(delta);
    m_render_system->render(frame);

    engine().endFrame();
}
