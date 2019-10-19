#pragma once

/////////// - StormKit::core - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/App.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::window - ///////////
#include <storm/entities/Entity.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

class App: public storm::core::App {
  public:
    App();
    ~App() override;

    void run(int argc, char **argv) override;

  private:
    void doInitWindow();
    void doInitBaseRenderObjects();
    void doInitMeshObjects(std::filesystem::path model_filepath);
    void doInitFramePasses(storm::engine::FramePassTextureID &backbuffer,
                           storm::engine::FrameGraph &frame_graph);
    void enableMSAA();

    storm::window::WindowOwnedPtr m_window;
    storm::window::EventHandlerOwnedPtr m_event_handler;
    storm::engine::EngineOwnedPtr m_engine;

    storm::engine::SceneOwnedPtr m_scene;
    storm::engine::FPSCameraOwnedPtr m_camera;
    storm::engine::StaticMeshRenderSystemObserverPtr m_mesh_render_system;

    storm::render::ShaderOwnedPtr m_vertex_shader;
    storm::render::ShaderOwnedPtr m_fragment_shader;

    bool m_rotate = true;
    storm::entities::Entity m_mesh;

    bool m_freeze_camera  = false;
    bool m_show_debug_gui = false;
    bool m_wireframe      = false;
};
