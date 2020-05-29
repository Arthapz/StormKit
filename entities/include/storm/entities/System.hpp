// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>

#include <storm/core/HashMap.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/entities/Component.hpp>
#include <storm/entities/Entity.hpp>
#include <storm/entities/Fwd.hpp>

namespace storm::entities {
    class EntityManager;
    class STORM_PUBLIC System: public storm::core::NonCopyable {
      public:
        using ComponentTypes = storm::core::HashSet<Component::Type>;

        explicit System(EntityManager &manager, core::UInt32 priority, ComponentTypes &&types);
        explicit System(EntityManager &manager, core::UInt32 priority, const ComponentTypes &types);

        System(System &&);
        System &operator=(System &&);

        virtual ~System();

        virtual void preUpdate();
        virtual void update(core::UInt64 delta) = 0;
        virtual void postUpdate();

        inline core::UInt32 priority() const noexcept { return m_priority; }
        inline const ComponentTypes &componentsUsed() const noexcept { return m_types; }

        void addEntity(Entity e);
        void removeEntity(Entity e);

        struct Predicate {
            inline bool operator()(const SystemOwnedPtr &s1,
                                   const SystemOwnedPtr &s2) const noexcept {
                return s1->priority() < s2->priority();
            }
        };

      protected:
        virtual void onMessageReceived(const Message &message) = 0;

        EntityManagerObserverPtr m_manager;
        storm::core::HashSet<Entity> m_entities;

        friend class EntityManager;

      private:
        core::UInt32 m_priority;
        ComponentTypes m_types;
    };
} // namespace storm::entities
