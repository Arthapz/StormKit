// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QSGTextureProvider>

#include <storm/engine/Fwd.hpp>

#include <storm/render/Fwd.hpp>
#include <storm/render/core/Surface.hpp>

QT_BEGIN_NAMESPACE
class QQuickItem;
class QQuickWindow;
QT_END_NAMESPACE

class StormKitTextureNode: public QSGTextureProvider, public QSGSimpleTextureNode {
    Q_OBJECT
  public:
    explicit StormKitTextureNode(QQuickItem *item, storm::engine::Engine &engine);
    ~StormKitTextureNode() override;

    QSGTexture *texture() const override { return QSGSimpleTextureNode::texture(); }

    void sync();
  private slots:
    void render();

  private:
    QSize m_size;

    storm::engine::EnginePtr m_engine;

    storm::render::HardwareBufferOwnedPtr m_buffer;

    QQuickWindow *m_window;
};
