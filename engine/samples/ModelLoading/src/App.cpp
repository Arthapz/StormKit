#include "App.hpp"
#include "GLTFScene.hpp"

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/EventHandler.hpp>
#include <storm/window/Window.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

using namespace storm;
using storm::log::operator""_module;

template<typename T>
static constexpr auto WINDOW_WIDTH = T { 1280 };
template<typename T>
static constexpr auto WINDOW_HEIGHT = T { 768 };
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
}

App::~App() = default;

void App::run([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    auto model_filepath = std::filesystem::path { EXAMPLES_DATA_DIR "models/Sword.glb" };

    for (auto i = 1; i < argc; ++i) {
        if (std::filesystem::exists(argv[i])) model_filepath = std::filesystem::path { argv[1] };
        else if (std::string { argv[i] } == std::string { "--fullscreen" })
            m_fullscreen = true;
    }

    doInitWindow();

    namespace Chrono = std::chrono;
    using Clock      = std::chrono::high_resolution_clock;

    auto input_handler = window::InputHandler { *m_window };

    input_handler.setMousePositionOnWindow(
        core::Position2i { WINDOW_WIDTH<core::Int32> / 2, WINDOW_HEIGHT<core::Int32> / 2 });

    m_scene = std::make_unique<GLTFScene>(*m_engine, *m_window, std::move(model_filepath));
    m_engine->setScene(*m_scene);

    auto last_timepoint = Clock::now();
    while (m_window->isOpen()) {
        const auto now_timepoint = Clock::now();
        const auto delta =
            Chrono::duration<float, Chrono::seconds::period> { now_timepoint - last_timepoint }
                .count();
        last_timepoint = now_timepoint;

        m_event_handler->update();

        m_scene->update(delta);

        m_engine->render();
    }

    m_engine->device().waitIdle();
}

void App::doInitWindow() {
    auto video_settings =
        window::VideoSettings { .size = core::Extentu { .width  = WINDOW_WIDTH<core::UInt32>,
                                                        .height = WINDOW_HEIGHT<core::UInt32> } };
    auto window_style = window::WindowStyle::Close;

    if (m_fullscreen) {
        video_settings = window::Window::getDesktopFullscreenSize();
        window_style   = window::WindowStyle::Fullscreen;
    }

    m_window        = std::make_unique<window::Window>(WINDOW_TITLE, video_settings, window_style);
    m_event_handler = std::make_unique<window::EventHandler>(*m_window);

    m_event_handler->addCallback(window::EventType::Closed,
                                 [this]([[maybe_unused]] const auto &event) { m_window->close(); });
    m_event_handler->addCallback(window::EventType::KeyPressed, [this](const auto &event) {
        if (event.key_event.key == window::Key::Escape) m_window->close();
        else if (event.key_event.key == window::Key::Numpad5)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::D);
        else if (event.key_event.key == window::Key::Numpad4)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::G);
        else if (event.key_event.key == window::Key::Numpad3)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::F);
        else if (event.key_event.key == window::Key::Numpad2)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Specular_Contribution);
        else if (event.key_event.key == window::Key::Numpad1)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Diffuse_Contribution);
        else if (event.key_event.key == window::Key::F10)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Emissive_Map);
        else if (event.key_event.key == window::Key::F9)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Ambiant_Occlusion_Map);
        else if (event.key_event.key == window::Key::F8)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Roughness_Map);
        else if (event.key_event.key == window::Key::F7)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Metallic_Map);
        else if (event.key_event.key == window::Key::F6)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Normal_Map);
        else if (event.key_event.key == window::Key::F5)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::Albedo_Map);
        else if (event.key_event.key == window::Key::F4)
            m_scene->setDebugView(engine::PBRMaterialInstance::DebugView::None);
        else if (event.key_event.key == window::Key::F3)
            m_scene->toggleMSAA();
        else if (event.key_event.key == window::Key::F2)
            m_scene->toggleFreezeCamera();
        else if (event.key_event.key == window::Key::F1)
            m_scene->toggleDebugGUI();
        else if (event.key_event.key == window::Key::Space)
            m_scene->toggleMeshRotation();
        else if (event.key_event.key == window::Key::W)
            m_scene->toggleWireframe();
    });

    m_engine = std::make_unique<engine::Engine>(*m_window, "ModelLoading");
}
