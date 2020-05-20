// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::window - ///////////
#include <storm/window/InputHandler.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/material/PBRMaterialInstance.hpp>

class GLTFScene final: public storm::engine::Scene {
  public:
    explicit GLTFScene(storm::engine::Engine &engine,
                       const storm::window::Window &window,
                       std::filesystem::path model_filepath);
    ~GLTFScene() override;

    GLTFScene(GLTFScene &&);
    GLTFScene &operator=(GLTFScene &&);

    inline void toggleFreezeCamera() noexcept { m_freeze_camera = !m_freeze_camera; }
    inline void toggleMeshRotation() noexcept { m_rotate_mesh = !m_rotate_mesh; }
    void toggleWireframe();
    void toggleMSAA() noexcept;
    inline void toggleDebugGUI() noexcept { m_show_debug_gui = !m_show_debug_gui; }
    void setDebugView(storm::engine::PBRMaterialInstance::DebugView debug_index);

    void update(float time);

  protected:
    void doRenderScene(storm::engine::FrameGraph &framegraph,
                       storm::engine::FramePassTextureID backbuffer,
                       std::vector<storm::engine::BindableBaseConstObserverPtr> bindables,
                       storm::render::GraphicsPipelineState &state) override;

  private:
    storm::window::WindowConstObserverPtr m_window;

    storm::engine::FPSCameraOwnedPtr m_camera;

    storm::window::InputHandler m_input_handler;

    storm::engine::v2::ModelOwnedPtr m_model;
    storm::engine::MeshArray m_meshes;

    bool m_freeze_camera  = false;
    bool m_rotate_mesh    = true;
    bool m_wireframe      = false;
    bool m_show_debug_gui = false;
};
