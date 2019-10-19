// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Ranges.hpp>

#include <storm/entities/Component.hpp>
#include <storm/entities/Entity.hpp>
#include <storm/entities/Fwd.hpp>
#include <storm/entities/System.hpp>

namespace storm::entities {
    class STORM_PUBLIC EntityManager final: public storm::core::NonCopyable {
      public:
        static constexpr auto ADDED_ENTITY_MESSAGE_ID   = 1;
        static constexpr auto REMOVED_ENTITY_MESSAGE_ID = 2;

        explicit EntityManager();
        ~EntityManager();

        EntityManager(EntityManager &&);
        EntityManager &operator=(EntityManager &&);

        Entity makeEntity();
        void destroyEntity(Entity entity);
        bool hasEntity(Entity entity);
        bool hasComponent(Entity entity);

        template<typename T>
        T &addComponent(Entity entity);

        template<typename T>
        void destroyComponent(Entity entity);

        template<typename T>
        bool hasComponent(Entity entity);

        bool hasComponent(Entity entity, Component::Type type);

        template<typename T>
        std::vector<Entity> entitiesWithComponent();

        template<typename T>
        T &getComponent(Entity entity);

        std::vector<ComponentRef> components(Entity entity);

        template<typename T>
        std::vector<std::reference_wrapper<T>> componentsOfType();

        template<typename T, typename... Args>
        T &addSystem(Args &&... args);

        template<typename T>
        bool hasSystem() const noexcept;

        template<typename T>
        T &getSystem();

        template<typename T>
        const T &getSystem() const;

        void step(core::UInt64 delta);

        inline core::ArraySize numberOfEntities() const noexcept;

        void commit(Entity e);

      private:
        void purposeToSystems(Entity e);
        void removeFromSystems(Entity e);
        void getNeededEntities(System &system);

        Entity m_next_valid_entity = 1;

        std::unordered_set<Entity> m_entities;
        std::set<SystemOwnedPtr, System::Predicate> m_systems;
        std::unordered_map<Entity, std::unordered_map<Component::Type, ComponentOwnedPtr>>
            m_components;

        std::unordered_set<Entity> m_added_entities;
        std::unordered_set<Entity> m_updated_entities;
        std::unordered_set<Entity> m_removed_entities;

        MessageBusOwnedPtr m_message_bus;
    };
} // namespace storm::entities

#include "EntityManager.inl"
