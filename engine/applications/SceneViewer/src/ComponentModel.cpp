// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "ComponentModel.hpp"
#include "StormKitScene.hpp"

#include <storm/engine/NameComponent.hpp>
#include <storm/engine/render/MaterialComponent.hpp>
#include <storm/engine/render/MeshComponent.hpp>
#include <storm/engine/render/RenderSystem.hpp>
#include <storm/engine/render/TransformComponent.hpp>

#include <QJsonObject>

using namespace storm;

template<typename... Ts, typename F>
constexpr void for_types(F &&f) noexcept {
    (f.template operator()<Ts>(), ...);
}

ComponentModel::ComponentModel(QObject *parent) : QAbstractListModel { parent } {
}

int ComponentModel::rowCount([[maybe_unused]] const QModelIndex &parent) const {
    return std::size(m_component_names);
}

QHash<int, QByteArray> ComponentModel::roleNames() const {
    auto roles                  = QHash<int, QByteArray> {};
    roles[ComponentModel::Type] = "type";
    roles[ComponentModel::Name] = "name";
    roles[ComponentModel::Data] = "component_data";

    return roles;
}

bool ComponentModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    return true;
}

QVariant ComponentModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};
    if (!m_scene) return {};

    auto &world = m_scene->world();

    if (!world.hasEntity(m_current_entity)) return {};

    switch (role) {
        case ComponentRole::Type: {
            return m_component_names[index.row()];
        }
        case ComponentRole::Name: {
            if (m_component_names[index.row()] ==
                refl::reflect<engine::NameComponent>().name.data) {
                return QString { "NameComponent" };
            }
        }
        case ComponentRole::Data: {
            if (m_component_names[index.row()] ==
                refl::reflect<engine::NameComponent>().name.data) {
                const auto &name_component =
                    world.getComponent<engine::NameComponent>(m_current_entity);
                return QString::fromStdString(name_component.name);
            }
        }
    }

    return {};
}

void ComponentModel::setScene(StormKitScene *scene) {
    m_scene = scene;

    emit sceneChanged(m_scene);
}

void ComponentModel::setCurrentEntity(quint64 entity) {
    beginResetModel();
    m_component_names.clear();
    endResetModel();

    m_current_entity = entity;

    introspect();
}

void ComponentModel::introspect() {
    if (!m_scene) return;

    auto &world = m_scene->world();

    if (!world.hasEntity(m_current_entity)) return;

    auto checkComponent = [&world, this]<typename T>() {
        if (world.hasComponent<T>(m_current_entity)) {
            beginInsertRows({}, rowCount(), rowCount());

            m_component_names.append(QString::fromLatin1(refl::reflect<T>().name.data));

            endInsertRows();
        }
    };

    for_types<engine::NameComponent,
              engine::MaterialComponent,
              engine::MeshComponent,
              engine::TransformComponent>(checkComponent);
}
