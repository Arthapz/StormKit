// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "EntityModel.hpp"

#include <storm/engine/render/MaterialComponent.hpp>
#include <storm/engine/render/MeshComponent.hpp>
#include <storm/engine/render/RenderSystem.hpp>
#include <storm/engine/render/TransformComponent.hpp>
#include <storm/engine/NameComponent.hpp>


using namespace storm;

Entity::Entity(const storm::entities::EntityManager &world, quint32 entity)
    : m_world{&world}, m_entity{entity} {
    if(m_world->hasComponent<engine::NameComponent>(entity)) {
        const auto &name_component = m_world->getComponent<engine::NameComponent>(entity);

        m_name = QString::fromStdString(name_component.name);
    }
}

EntityModel::EntityModel(QObject *parent)
    : QAbstractListModel{parent} {
}

QHash<int, QByteArray> EntityModel::roleNames() const {
    auto roles = QHash<int, QByteArray>{};
    roles[EntityModel::EntityRole::Name] = "name";
    roles[EntityModel::EntityRole::ID] = "id";

    return roles;
}

QVariant EntityModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) return {};

    if(role == EntityRole::Name) return m_entities[index.row()].name();
    else if(role == EntityRole::ID) return m_entities[index.row()].ID();

    return {};
}

void EntityModel::addEntity(quint32 entity) {
    Q_ASSERT(m_world);

    beginInsertRows(QModelIndex{}, rowCount(), rowCount());

    m_entities.push_back(Entity{*m_world, entity});

    endInsertRows();
}
