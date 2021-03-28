// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QObject>

/////////// - StormKit::window - ///////////
#include <storm/window/InputHandler.hpp>

/////////// - StormKit::core - ///////////
#include <storm/render/core/Enums.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/Scene.hpp>

class StormKitScene: public QObject, public storm::engine::State {
    Q_OBJECT
  public:
    explicit StormKitScene(storm::engine::Engine &engine);
    ~StormKitScene() override;

  signals:
    void newEntity(quint32);

  private:
    void update(storm::core::Secondf delta);

    bool m_initialized = false;
};

