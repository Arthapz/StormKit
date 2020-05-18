#include "App.hpp"

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/EventHandler.hpp>
#include <storm/window/InputHandler.hpp>
#include <storm/window/Window.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/FPSCamera.hpp>
#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/core/ShaderGenerator.hpp>
#include <storm/engine/core/TransformComponent.hpp>
#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/mesh/Model.hpp>
#include <storm/engine/mesh/Primitives.hpp>
#include <storm/engine/mesh/StaticMesh.hpp>
#include <storm/engine/mesh/StaticMeshComponent.hpp>
#include <storm/engine/mesh/StaticMeshRenderSystem.hpp>
#include <storm/engine/mesh/StaticSubMesh.hpp>

#include <storm/engine/material/MaterialComponent.hpp>

#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

using namespace storm;
using storm::log::operator""_module;

template<typename T>
static constexpr auto WINDOW_WIDTH = T { 1280 };
template<typename T>
static constexpr auto WINDOW_HEIGHT = T { 800 };
static constexpr auto WINDOW_TITLE  = "StormKit ModelLoading Example";
static constexpr auto LOG_MODULE    = "ModelLoading"_module;

App::App() {
    log::LogHandler::ilog(LOG_MODULE,
                          "Using StormKit {}.{}.{} {} {}",
                          STORM_MAJOR_VERSION,
                          STORM_MINOR_VERSION,
                          STORM_PATCH_VERSION,
                          STORM_GIT_BRANCH,
                          STORM_GIT_COMMIT_HASH);
    doInitWindow();
    doInitBaseRenderObjects();
}

App::~App() = default;

void App::run([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    auto model_filepath = std::filesystem::path { "./models/Sword.glb" };

    if (argc > 1) { model_filepath = std::filesystem::path { argv[1] }; }

    doInitMeshObjects(std::move(model_filepath));

    namespace Chrono = std::chrono;
    using Clock      = std::chrono::high_resolution_clock;

    auto start_time = Clock::now();

    auto input_handler = window::InputHandler { *m_window };

    input_handler.setMousePositionOnWindow(
        core::Position2i { WINDOW_WIDTH<core::Int32> / 2, WINDOW_HEIGHT<core::Int32> / 2 });

    m_camera->setPosition({ 0.f, 0.f, -4.5f });
    m_camera->setRotation({ -90.f, 0.f, 0.f });

    auto &world = m_scene->entities();
    while (m_window->isOpen()) {
        m_event_handler->update();

        const auto now_timepoint = Clock::now();
        const auto time =
            Chrono::duration<float, Chrono::seconds::period> { now_timepoint - start_time }.count();

        if (m_rotate) {
            auto &transform_component = world.getComponent<engine::TransformComponent>(m_mesh);
            transform_component.transform->setRoll(time * 90.f);
        }
        if (m_show_debug_gui) {
            auto &debug_gui = m_engine->debugGUI();
            debug_gui.update(*m_window);
        }

        if (!m_freeze_camera) {
            auto camera_inputs = engine::FPSCamera::Inputs {};

            if (input_handler.isKeyPressed(window::Key::Z)) camera_inputs.up = true;
            if (input_handler.isKeyPressed(window::Key::S)) camera_inputs.down = true;
            if (input_handler.isKeyPressed(window::Key::Q)) camera_inputs.left = true;
            if (input_handler.isKeyPressed(window::Key::D)) camera_inputs.right = true;

            const auto position = [&camera_inputs, &input_handler]() {
                auto position = input_handler.getMousePositionOnWindow();

                if (position->x < 0 || position->x > WINDOW_WIDTH<core::Int32>) {
                    position->x                = WINDOW_WIDTH<core::Int32> / 2;
                    camera_inputs.mouse_ignore = true;
                }
                if (position->y < 0 || position->y > WINDOW_HEIGHT<core::Int32>) {
                    position->y                = WINDOW_HEIGHT<core::Int32> / 2;
                    camera_inputs.mouse_ignore = true;
                }

                input_handler.setMousePositionOnWindow(position);

                return position;
            }();

            camera_inputs.mouse_updated = true;
            camera_inputs.x_mouse       = static_cast<float>(position->x);
            camera_inputs.y_mouse       = static_cast<float>(position->y);

            m_camera->feedInputs(camera_inputs);
        }

        m_engine->render();
    }

    m_engine->device().waitIdle();
}

void App::doInitWindow() {
    const auto video_settings =
        window::VideoSettings { .size = core::Extentu { .width  = WINDOW_WIDTH<core::UInt32>,
                                                        .height = WINDOW_HEIGHT<core::UInt32> } };
    const auto window_style = window::WindowStyle::Close;

    m_window        = std::make_unique<window::Window>(WINDOW_TITLE, video_settings, window_style);
    m_event_handler = std::make_unique<window::EventHandler>(*m_window);

    m_event_handler->addCallback(window::EventType::Closed,
                                 [this]([[maybe_unused]] const auto &event) { m_window->close(); });
    m_event_handler->addCallback(window::EventType::KeyPressed, [this](const auto &event) {
        if (event.key_event.key == window::Key::Escape) m_window->close();
        else if (event.key_event.key == window::Key::F3)
            enableMSAA();
        else if (event.key_event.key == window::Key::F2)
            m_freeze_camera = !m_freeze_camera;
        else if (event.key_event.key == window::Key::F1) {
            m_show_debug_gui = !m_show_debug_gui;
        } else if (event.key_event.key == window::Key::Space)
            m_rotate = !m_rotate;
        else if (event.key_event.key == window::Key::W) {
            const auto &capabilities = m_engine->device().physicalDevice().capabilities();
            if (!capabilities.features.fill_Mode_non_solid) {
                log::LogHandler::elog("Wireframe is not supported on this GPU");
                return;
            }

            auto &world              = m_scene->entities();
            auto &material_component = world.getComponent<engine::MaterialComponent>(m_mesh);
            for (auto &material : material_component.materials) {
                auto pipeline = material.pipelineState();

                pipeline.rasterization_state.polygon_mode =
                    (pipeline.rasterization_state.polygon_mode == render::PolygonMode::Fill)
                        ? render::PolygonMode::Line // WireFrame
                        : render::PolygonMode::Fill;

                material.setGraphicsPipelineState(pipeline);
            }
        }
    });
}

void App::doInitBaseRenderObjects() {
    m_engine = std::make_unique<engine::Engine>(*m_window);
    m_engine->enableMSAA();

    m_scene  = std::make_unique<engine::Scene>(*m_engine);
    m_camera = std::make_unique<engine::FPSCamera>(
        *m_engine,
        m_engine->surface().extent().convertTo<core::Extentf>());

    m_scene->setCamera(*m_camera);

    m_mesh_render_system = core::makeObserver(
        m_scene->entities().addSystem<engine::StaticMeshRenderSystem>(*m_engine));

    m_engine->setInitFramegraphCallback([this](auto &backbuffer, auto &framegraph) {
        auto output = m_engine->doInitPBRPasses(backbuffer, framegraph);
        if (m_show_debug_gui) m_engine->doInitDebugGUIPasses(output, framegraph);
    });

    m_engine->setScene(*m_scene);
}

void App::doInitMeshObjects(std::filesystem::path model_filepath) {
    auto &world = m_scene->entities();
    m_mesh      = world.makeEntity();
    auto model  = engine::Model { *m_engine };

    auto loaded_mesh = model.loadStaticMeshFromFile(model_filepath, engine::Model::Type::GLB);
    if (!loaded_mesh.has_value()) {
        log::LogHandler::flog(LOG_MODULE, "Failed to load {}", model_filepath.string());
        std::exit(EXIT_FAILURE);
    }

    log::LogHandler::ilog(LOG_MODULE, "{} loaded", model_filepath.string());

    auto [mesh, materials] = std::move(loaded_mesh.value());
    auto &pipeline_builder = m_engine->pipelineBuilder();
    for (auto &material : materials) pipeline_builder.createPBRPipeline(mesh, material, true);

    auto &mesh_component               = world.addComponent<engine::StaticMeshComponent>(m_mesh);
    mesh_component.mesh                = std::make_unique<engine::StaticMesh>(std::move(mesh));
    mesh_component.passes_to_render_in = m_engine->pbrPasseNames();

    auto &material_component     = world.addComponent<engine::MaterialComponent>(m_mesh);
    material_component.materials = std::move(materials);

    const auto &bounding_box = mesh_component.mesh->boundingBox();

    auto &transform_component     = world.addComponent<engine::TransformComponent>(m_mesh);
    transform_component.transform = m_engine->createTransformPtr();

    const auto max_length =
        std::max(bounding_box.extent.width,
                 std::max(bounding_box.extent.height, bounding_box.extent.depth));
    const auto scale = 10.f / max_length;
    transform_component.transform->setScale( // auto scale
        scale,
        scale,
        scale);
    transform_component.transform->moveY(-4.f);
    transform_component.transform->moveZ(-16.f);
    transform_component.transform->rotateYaw(-90.f);
}

void App::enableMSAA() {
    auto &world = m_scene->entities();

    m_engine->toggleMSAA();

    auto &material_component = world.getComponent<engine::MaterialComponent>(m_mesh);

    const auto samples =
        (m_engine->isMSAAEnabled()) ? m_engine->maxSampleCount() : render::SampleCountFlag::C1_BIT;

    for (auto &material : material_component.materials) {
        auto state                                    = material.pipelineState();
        state.multisample_state.rasterization_samples = samples;
        material.setGraphicsPipelineState(std::move(state));
    }
}
