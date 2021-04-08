// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QAbstractListModel>

#include <storm/core/Memory.hpp>

#include <storm/entities/Entity.hpp>
#include <storm/entities/EntityManager.hpp>

class Entity {
  public:
    Entity(const storm::entities::EntityManager &world, quint32 entity);

    quint64 ID() const noexcept { return m_entity; }

    const QString &name() const noexcept { return m_name; }

  private:
    storm::entities::EntityManagerConstPtr m_world;
    quint32 m_entity;
    QString m_name;
};

class EntityModel: public QAbstractListModel {
    Q_OBJECT
  public:
    enum EntityRole { Name = Qt::UserRole + 1, ID };

    explicit EntityModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex {}) const override {
        return m_entities.size();
    }
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setWorld(const storm::entities::EntityManager &world) {
        m_world = storm::core::makeConstObserver(world);
    }

  public slots:
    void addEntity(quint32 entity);

  private:
    storm::entities::EntityManagerConstPtr m_world;
    QVector<Entity> m_entities;
};
