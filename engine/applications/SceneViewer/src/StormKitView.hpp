// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QtQuick/QQuickItem>

#include <storm/engine/Engine.hpp>

#include <storm/render/Fwd.hpp>
#include <storm/render/core/Surface.hpp>

#include "EntityModel.hpp"
#include "StormKitScene.hpp"

class StormKitTextureNode;
class EntityModel;
class StormKitView: public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(bool initialized MEMBER m_initialized NOTIFY initialized)
    Q_PROPERTY(EntityModel *entities READ entities CONSTANT)
    Q_PROPERTY(StormKitScene *scene READ scene CONSTANT)

    QML_ELEMENT
  public:
    explicit StormKitView(QQuickItem *parent = nullptr);

    EntityModel *entities() noexcept { return &m_model; }
    StormKitScene *scene() noexcept { return m_scene.get(); }
    bool isInitialized() const noexcept { return m_initialized; }

  signals:
    void timeChanged();
    void entitiesUpdated();
    void initialized();

  protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChange(const QRectF &, const QRectF &) override;

  private slots:
    void invalidateSceneGraph();

  private:
    void initialize();
    void releaseResources() override;

    EntityModel m_model;

    StormKitTextureNode *m_texture_node = nullptr;

    storm::engine::EngineOwnedPtr m_engine;
    std::unique_ptr<StormKitScene> m_scene;

    bool m_initialized = false;
};
