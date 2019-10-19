// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <unordered_set>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/entities/Component.hpp>
#include <storm/entities/Entity.hpp>
#include <storm/entities/Fwd.hpp>

namespace storm::entities {
	class EntityManager;
	class STORM_PUBLIC System : public storm::core::NonCopyable {
	  public:
		using ComponentTypes = std::unordered_set<Component::Type>;

		explicit System(EntityManager &manager, std::uint32_t priority,
						ComponentTypes &&types);
		explicit System(EntityManager &manager, std::uint32_t priority,
						const ComponentTypes &types);

		System(System &&);
		System &operator=(System &&);

		virtual ~System();

		virtual void preUpdate();
		virtual void update(std::uint64_t delta) = 0;
		virtual void postUpdate();

		inline std::uint32_t priority() const noexcept {
			return m_priority;
		}
		inline const ComponentTypes &componentsUsed() const noexcept {
			return m_types;
		}

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

		EntityManagerRef m_manager;
		std::unordered_set<Entity> m_entities;

        friend class EntityManager;

	  private:
		std::uint32_t m_priority;
		ComponentTypes m_types;
	};
} // namespace storm::entities
