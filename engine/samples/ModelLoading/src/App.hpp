#pragma once

/////////// - StormKit::core - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/App.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

class GLTFScene;
class App final: public storm::core::App {
  public:
    App();
    ~App() override;

    void run(int argc, char **argv) override;

  private:
    void doInitWindow();

    storm::window::WindowOwnedPtr m_window;
    storm::window::EventHandlerOwnedPtr m_event_handler;
    storm::engine::EngineOwnedPtr m_engine;

    std::unique_ptr<GLTFScene> m_scene;
};
