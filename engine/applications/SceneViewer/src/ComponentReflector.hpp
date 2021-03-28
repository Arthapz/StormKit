#pragma once

#include <QMap>
#include <QObject>
#include <QQuickItem>
#include <QVariant>

#include "StormKitScene.hpp"

#include <storm/entities/Component.hpp>

class ComponentReflector: public QObject {
    Q_OBJECT

    Q_PROPERTY(StormKitScene *scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(quint32 current_entity READ currentEntity WRITE setCurrentEntity NOTIFY currentEntityChanged)
    Q_ENUMS(ComponentType)

  public:
    enum class ComponentType { Name, Transform, Render, Material };

    explicit ComponentReflector(QObject *parent = nullptr);

    void setScene(StormKitScene *scene);
    StormKitScene *scene() noexcept { return m_scene; }
    const StormKitScene *scene() const noexcept { return m_scene; }

    quint32 currentEntity() const noexcept { return m_current_entity; }
    void setCurrentEntity(quint32 entity);

    Q_INVOKABLE bool hasComponent(ComponentType type) const;

  signals:
    void sceneChanged(StormKitScene *);
    void componentFrameChanged(QQuickItem *);
    void currentEntityChanged(quint32);

  private:
    StormKitScene *m_scene = nullptr;

    quint32 m_current_entity = std::numeric_limits<quint32>::max();
};

Q_DECLARE_METATYPE(ComponentReflector::ComponentType)
