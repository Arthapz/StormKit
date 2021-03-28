// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QAbstractListModel>

#include "StormKitScene.hpp"

class ComponentModel: public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(StormKitScene *scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(quint32 current_entity MEMBER m_current_entity WRITE setCurrentEntity)
  public:
    enum ComponentRole { Type = Qt::UserRole + 1, Name, Data };

    explicit ComponentModel(QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex {}) const final;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const final;
    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role) final;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const final;

    [[nodiscard]] StormKitScene *scene() noexcept { return m_scene; }
    [[nodiscard]] const StormKitScene *scene() const noexcept { return m_scene; }
    void setScene(StormKitScene *scene);

    void setCurrentEntity(quint64 entity);

  signals:
    void sceneChanged(StormKitScene *);

  private:
    void introspect();

    StormKitScene *m_scene = nullptr;

    quint32 m_current_entity = std::numeric_limits<quint32>::max();

    QVector<QString> m_component_names;
};
