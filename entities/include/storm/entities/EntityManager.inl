// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <algorithm>
#include <cassert>
#include <gsl/gsl_util>
#include <memory>
#include <storm/core/Assert.hpp>
#include <storm/entities/System.hpp>
#include <vector>

namespace storm::entities {
    template <typename T> T &EntityManager::addComponent(Entity entity) {
        static_assert(std::is_base_of<Component, T>::value,
                      "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE,
                      "T must have T::type defined");

        STORM_EXPECTS(hasEntity(entity));

        auto component = std::make_unique<T>();

        auto type				   = T::TYPE;
        m_components[entity][type] = std::move(component);

        m_updated_entities.emplace(entity);

        return getComponent<T>(entity);
    }

    template <typename T> void EntityManager::destroyComponent(Entity entity) {
        static_assert(std::is_base_of<Component, T>::value,
                      "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE,
                      "T must have T::type defined");

        STORM_EXPECTS(hasEntity(entity));
        STORM_EXPECTS(hasComponent<T>(entity));

        auto it = std::find_if(std::begin(m_components[entity]),
                               std::end(m_components[entity]), [](auto &i) {
                                   if (i.first == T::TYPE)
                                       return true;

                                   return false;
                               });

        m_components[entity].erase(it);

        m_updated_entities.emplace(entity);
    }

    template <typename T> bool EntityManager::hasComponent(Entity entity) {
        static_assert(std::is_base_of<Component, T>::value,
                      "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE,
                      "T must have T::type defined");

        STORM_EXPECTS(hasEntity(entity));

        auto it = std::find_if(std::begin(m_components[entity]),
                               std::end(m_components[entity]), [](auto &i) {
                                   if (i.first == T::TYPE)
                                       return true;

                                   return false;
                               });

        return (it != std::end(m_components[entity]));
    }

    template <typename T>
    std::vector<Entity> EntityManager::entitiesWithComponent() {
        static_assert(std::is_base_of<Component, T>::value,
                      "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE,
                      "T must have T::type defined");

        auto entities = std::vector<Entity>{};

        std::for_each(std::begin(m_components), std::end(m_components),
                      [&entities](const auto &pair) {
                          const auto &map = pair.second;
                          auto it =
                              std::find_if(std::begin(map), std::end(map),
                                           [](const auto &component) {
                                               if (component.first == T::TYPE)
                                                   return true;

                                               return false;
                                           });

                          if (it != std::end(map))
                              entities.push_back(pair.first);
                      });

        return entities;
    }

    template <typename T> T &EntityManager::getComponent(Entity entity) {
        static_assert(std::is_base_of<Component, T>::value,
                      "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE,
                      "T must have T::type defined");

        STORM_EXPECTS(hasComponent<T>(entity));
        STORM_EXPECTS(hasEntity(entity));

        auto type = T::TYPE;
        return *dynamic_cast<T *>(m_components[entity][type].get());
    }

    template <typename T>
    std::vector<std::reference_wrapper<T>> EntityManager::componentsOfType() {
        auto vec = std::vector<std::reference_wrapper<T>>{};

        for (auto i : m_entities) {
            if (hasComponent<T>(i)) {
                vec.emplace_back(getComponent<T>(i));
            }
        }

        return vec;
    }

    template <typename T, typename... Args>
    T &EntityManager::addSystem(Args &&... args) {
        auto system =
            std::make_unique<T>(*this, std::forward<Args &&>(args)...);
        auto &ref = static_cast<T &>(*system);

        getNeededEntities(ref);
        m_systems.emplace(std::move(system));

        return ref;
    }
} // namespace storm::entities
