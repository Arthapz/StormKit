#include "ComponentReflector.hpp"
#include "StormKitScene.hpp"

#include <QMap>
#include <QString>
#include <QStringView>
#include <QVector>

#include <storm/engine/NameComponent.hpp>

ComponentReflector::ComponentReflector(QObject *parent) : QObject { parent } {
}

void ComponentReflector::setScene(StormKitScene *scene) {
    m_scene = scene;

    emit sceneChanged(m_scene);
}

void ComponentReflector::setCurrentEntity(quint32 entity) {
    m_current_entity = entity;

    emit currentEntityChanged(m_current_entity);
}

bool ComponentReflector::hasComponent(ComponentReflector::ComponentType type) const {
    auto &world = m_scene->world();

    return world.hasComponent<storm::engine::NameComponent>(m_current_entity);
}
