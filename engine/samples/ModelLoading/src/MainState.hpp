// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/State.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/InputHandler.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Enums.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>
#include <storm/engine/State.hpp>

class MainState final: public storm::engine::State {
  public:
    explicit MainState(storm::core::StateManager &owner,
                       storm::engine::Engine &engine,
                       const storm::window::Window &window);
    ~MainState() override;

    MainState(MainState &&) noexcept;
    MainState &operator=(MainState &&) noexcept;

    void pause() noexcept override {}
    void resume() noexcept override {}
    void update(storm::core::Secondf delta) override;

  private:
    storm::window::InputHandler m_input_handler;
    storm::engine::RenderSystemPtr m_render_system;
    storm::engine::FPSCameraOwnedPtr m_camera;
};
