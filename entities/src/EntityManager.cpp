#include <storm/entities/EntityManager.hpp>
#include <storm/entities/MessageBus.hpp>

#include <storm/core/Ranges.hpp>

using namespace storm::entities;

/////////////////////////////////////
/////////////////////////////////////
EntityManager::EntityManager() {
    m_message_bus = std::make_unique<MessageBus>();
}

/////////////////////////////////////
/////////////////////////////////////
EntityManager::EntityManager(EntityManager &&) = default;

/////////////////////////////////////
/////////////////////////////////////
EntityManager &EntityManager::operator=(EntityManager &&) = default;

/////////////////////////////////////
/////////////////////////////////////
EntityManager::~EntityManager() = default;

/////////////////////////////////////
/////////////////////////////////////
Entity EntityManager::makeEntity() {
    const auto entity = m_next_valid_entity++;

    m_added_entities.emplace(entity);
    m_message_bus->push(Message { ADDED_ENTITY_MESSAGE_ID, { entity } });

    return entity;
}

/////////////////////////////////////
/////////////////////////////////////
void EntityManager::destroyEntity(Entity entity) {
    STORM_EXPECTS(entity != INVALID_ENTITY);

    if (hasEntity(entity)) {
        m_removed_entities.emplace(entity);
        m_message_bus->push(Message { REMOVED_ENTITY_MESSAGE_ID, { entity } });
    }
}

/////////////////////////////////////
/////////////////////////////////////
bool EntityManager::hasEntity(Entity entity) {
    STORM_EXPECTS(entity != INVALID_ENTITY);

    auto it  = core::ranges::find(m_entities, entity);
    auto it2 = core::ranges::find(m_added_entities, entity);

    return it != std::cend(m_entities) || it2 != std::cend(m_added_entities);
}

/////////////////////////////////////
/////////////////////////////////////
bool EntityManager::hasComponent(Entity entity) {
    STORM_EXPECTS(entity != INVALID_ENTITY);

    auto it = m_components.find(entity);

    if (it == core::ranges::end(m_components)) return false;

    return true;
}

/////////////////////////////////////
/////////////////////////////////////
bool EntityManager::hasComponent(Entity entity, Component::Type type) {
    STORM_EXPECTS(entity != INVALID_ENTITY && type != Component::INVALID_TYPE);

    auto it = core::ranges::find_if(m_components[entity], [type](auto &i) {
        if (i.first == type) return true;

        return false;
    });

    return (it != core::ranges::end(m_components[entity]));
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<ComponentRef> EntityManager::components(Entity entity) {
    STORM_EXPECTS(entity != INVALID_ENTITY);

    if (!hasEntity(entity) || !hasComponent(entity)) return {};

    auto vec = std::vector<ComponentRef> {};
    vec.reserve(m_components[entity].size());
    for (auto &i : m_components[entity]) { vec.emplace_back(*i.second); }

    return vec;
}

/////////////////////////////////////
/////////////////////////////////////
void EntityManager::step(uint64_t delta) {
    for (auto entity : m_added_entities) { m_entities.emplace(entity); }
    m_added_entities.clear();

    for (auto entity : m_updated_entities) { purposeToSystems(entity); }
    m_updated_entities.clear();

    for (auto entity : m_removed_entities) {
        auto it = core::ranges::find(m_entities, entity);

        auto itMap = m_components.find(entity);
        if (hasComponent(entity)) m_components.erase(itMap);

        m_entities.erase(it);

        removeFromSystems(entity);
    }
    m_removed_entities.clear();

    while (!m_message_bus->empty()) {
        for (auto &system : m_systems) system->onMessageReceived(m_message_bus->top());
        m_message_bus->pop();
    }

    for (auto &system : m_systems) system->preUpdate();
    for (auto &system : m_systems) system->update(delta);
    for (auto &system : m_systems) system->postUpdate();
}

/////////////////////////////////////
/////////////////////////////////////
void EntityManager::purposeToSystems(Entity e) {
    STORM_EXPECTS(e != INVALID_ENTITY);

    for (auto &s : m_systems) {
        auto is_reliable = true;
        for (auto t : s->componentsUsed()) {
            if (!hasComponent(e, t)) {
                is_reliable = false;
                break;
            }
        }

        if (is_reliable) s->addEntity(e);
    }
}

/////////////////////////////////////
/////////////////////////////////////
void EntityManager::removeFromSystems(Entity e) {
    STORM_EXPECTS(e != INVALID_ENTITY);

    for (auto &s : m_systems) { s->removeEntity(e); }
}

/////////////////////////////////////
/////////////////////////////////////
void EntityManager::getNeededEntities(System &system) {
    for (auto e : m_entities) {
        auto is_reliable = true;
        for (auto t : system.componentsUsed()) {
            if (!hasComponent(e, t)) {
                is_reliable = false;
                break;
            }
        }

        if (is_reliable) system.addEntity(e);
    }
}
