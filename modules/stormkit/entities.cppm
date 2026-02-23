// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/memory_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

#include <stormkit/entities/api.hpp>

export module stormkit.entities;

import std;

import stormkit.core;

namespace stdr = std::ranges;
namespace stdv = std::views;

export namespace stormkit::entities {
    using Entity                         = u32;
    inline constexpr auto INVALID_ENTITY = Entity { 0 };
    class System;

    struct EntityHashFunc {
#ifdef STORMKIT_COMPILER_MSVC
        [[nodiscard]]
        auto operator()(Entity k) const noexcept -> hash64;
#else
        [[nodiscard]]
        static auto operator()(Entity k) noexcept -> hash64;
#endif
    };

    using ComponentType = u32;

    namespace meta {
        template<typename T>
        concept IsComponentType = requires(T&& component) {
            { component.type() } -> core::meta::Is<ComponentType>;
        };
    } // namespace meta

    struct Message {
        u32                 id;
        std::vector<Entity> entities;
    };

    class STORMKIT_ENTITIES_API MessageBus {
      public:
        MessageBus();
        ~MessageBus();

        MessageBus(const MessageBus&)                    = delete;
        auto operator=(const MessageBus&) -> MessageBus& = delete;

        MessageBus(MessageBus&&);
        auto operator=(MessageBus&&) -> MessageBus&;

        auto push(Message&& message) -> void;
        [[nodiscard]]
        auto top() const -> const Message&;
        auto pop() -> void;

        [[nodiscard]]
        auto empty() const noexcept -> bool;

      private:
        std::queue<Message> m_messages;
    };

    class EntityManager;

    class STORMKIT_ENTITIES_API System {
      public:
        using ComponentTypes = std::vector<ComponentType>;
        using Entities       = std::vector<Entity>;

        using PreUpdateClosure  = std::function<void(EntityManager&, const Entities&)>;
        using UpdateClosure     = std::function<void(EntityManager&, fsecond delta, const Entities&)>;
        using PostUpdateClosure = std::function<void(EntityManager&, const Entities&)>;
        using OnMessageReceived = std::function<void(EntityManager&, const Message&, const Entities&)>;

        struct Closures {
            PreUpdateClosure  pre_update = monadic::noop();
            UpdateClosure     update;
            PostUpdateClosure post_update         = monadic::noop();
            OnMessageReceived on_message_received = monadic::noop();
        };

        System(std::string name, ComponentTypes types, Closures&& closures) noexcept;

        System(const System&)                    = delete;
        auto operator=(const System&) -> System& = delete;

        System(System&&) noexcept;
        auto operator=(System&&) noexcept -> System&;

        ~System() noexcept;

        [[nodiscard]]
        auto name() const noexcept -> const std::string&;
        [[nodiscard]]
        auto components_used() const noexcept -> const ComponentTypes&;

      private:
        auto add_entity(Entity e) noexcept -> void;
        auto remove_entity(Entity e) noexcept -> void;

        auto pre_update(EntityManager&) noexcept -> void;
        auto update(EntityManager&, fsecond) noexcept -> void;
        auto post_update(EntityManager&) noexcept -> void;

        auto on_message_received(EntityManager&, const Message&) noexcept -> void;

        std::string m_name;

        ComponentTypes m_types;

        Closures m_closures;

        Entities m_entities;

        friend class EntityManager;
    };

    struct ComponentStore {};

    class STORMKIT_ENTITIES_API EntityManager {
      public:
        static constexpr auto ADDED_ENTITY_MESSAGE_ID   = 1;
        static constexpr auto REMOVED_ENTITY_MESSAGE_ID = 2;

        EntityManager() noexcept;
        ~EntityManager() noexcept;

        EntityManager(const EntityManager&)                    = delete;
        auto operator=(const EntityManager&) -> EntityManager& = delete;

        EntityManager(EntityManager&&) noexcept;
        auto operator=(EntityManager&&) noexcept -> EntityManager&;

        auto make_entity() noexcept -> Entity;
        auto destroy_entity(Entity entity) noexcept -> void;
        auto destroy_all_entities() noexcept -> void;
        auto has_entity(Entity entity) const noexcept -> bool;

        template<meta::IsComponentType T>
        auto add_component(Entity entity, T&& component) noexcept -> T&;

        auto destroy_component(Entity entity, std::string_view name) noexcept -> void;
        auto destroy_component(Entity entity, ComponentType type) noexcept -> void;

        auto has_component(Entity entity, std::string_view name) const noexcept -> bool;
        auto has_component(Entity entity, ComponentType type) const noexcept -> bool;

        auto entities() const noexcept -> const std::vector<Entity>&;

        auto entities_with_component(ComponentType type) const noexcept -> std::vector<Entity>;
        auto entities_with_component(std::string_view name) const noexcept -> std::vector<Entity>;

        template<meta::IsComponentType T, class Self>
        auto get_component(this Self& self, Entity entity, ComponentType type) noexcept -> core::meta::ForwardConst<Self, T&>;
        template<meta::IsComponentType T, class Self>
        auto get_component(this Self& self, Entity entity, std::string_view name) noexcept -> core::meta::ForwardConst<Self, T&>;

        template<meta::IsComponentType T, class Self>
        auto components_of_type(this Self& self, ComponentType type) noexcept
          -> std::vector<Ref<core::meta::ForwardConst<Self, T>>>;
        template<meta::IsComponentType T, class Self>
        auto components_of_type(this Self& self, std::string_view name) noexcept
          -> std::vector<Ref<core::meta::ForwardConst<Self, T>>>;

        auto components_types_of(Entity entity) const noexcept -> std::vector<ComponentType>;

        auto add_system(std::string name, System::ComponentTypes types, System::Closures&& closures) noexcept -> System&;
        auto has_system(std::string_view name) const noexcept -> bool;
        auto remove_system(std::string_view name) noexcept -> void;

        template<class Self>
        auto systems(this Self& self) noexcept -> std::vector<Ref<core::meta::ForwardConst<Self, System>>>;

        template<class Self>
        auto get_system(this Self& self, std::string_view name) noexcept -> core::meta::ForwardConst<Self, System&>;

        auto step(fsecond delta) noexcept -> void;

        auto entity_count() const noexcept -> usize;

        // void commit(Entity e);

      private:
        using ComponentKey = u64;

        struct Store {
            ComponentType                   type;
            usize                           size;
            std::vector<Entity>             entities;
            std::vector<std::byte>          data;
            std::function<void(std::byte*)> delete_func;
        };

        using ComponentStore = std::vector<Store>;

        auto purpose_to_systems(Entity e) noexcept -> void;
        auto remove_from_systems(Entity e) noexcept -> void;
        auto get_needed_entities(System& system) noexcept -> void;

        Entity m_next_valid_entity = 1;

        std::vector<Entity> m_entities;

        std::vector<Entity> m_free_entities;

        HashSet<Entity> m_added_entities;
        HashSet<Entity> m_updated_entities;
        HashSet<Entity> m_removed_entities;

        std::vector<System> m_systems;

        ComponentStore m_components;

        MessageBus m_message_bus;
    };
} // namespace stormkit::entities

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
#ifdef STORMKIT_COMPILER_MSVC
    inline auto EntityHashFunc::operator()(Entity k) const noexcept -> hash64 {
#else
    inline auto EntityHashFunc::operator()(Entity k) noexcept -> hash64 {
#endif
        return as<hash64>(k);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto MessageBus::empty() const noexcept -> bool {
        return std::empty(m_messages);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto System::name() const noexcept -> const std::string& {
        return m_name;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto System::components_used() const noexcept -> const ComponentTypes& {
        return m_types;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsComponentType T>
    auto EntityManager::add_component(Entity entity, T&& component) noexcept -> T& {
        using PureT = core::meta::ToPlainType<T>;
        EXPECTS(has_entity(entity));
        EXPECTS(not has_component(entity, component.type()));

        auto it = stdr::find_if(m_components, [type = component.type()](const auto& pair) noexcept { return pair.type == type; });
        if (it == stdr::cend(m_components))
            it = m_components.emplace(stdr::cend(m_components),
                                      Store { component.type(), sizeof(PureT), {}, {}, [](auto ptr) static noexcept {
                                                 std::bit_cast<PureT*>(ptr)->~PureT();
                                             } });

        ENSURES(it != stdr::cend(m_components));

        auto& [_, size, entities, components, _] = *it;
        ENSURES(size == sizeof(PureT));

        const auto old_size = stdr::size(components);
        components.resize(old_size + sizeof(Entity) + size);
        new (stdr::data(components) + old_size) Entity { entity };
        auto* _component = new (stdr::data(components) + sizeof(Entity) + old_size) PureT { std::forward<T>(component) };

        entities.emplace_back(entity);

        m_updated_entities.emplace(entity);

        return *_component;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::destroy_component(Entity entity, std::string_view name) noexcept -> void {
        destroy_component(entity, hash(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::has_component(Entity entity, std::string_view name) const noexcept -> bool {
        return has_component(entity, hash(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::entities() const noexcept -> const std::vector<Entity>& {
        return m_entities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::entities_with_component(ComponentType type) const noexcept -> std::vector<Entity> {
        // clang-format off
        return entities() 
               | stdv::filter([this, type](auto entity) noexcept { return has_component(entity, type); })
               | stdr::to<std::vector>();
        // clang-format on
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::entities_with_component(std::string_view name) const noexcept -> std::vector<Entity> {
        return entities_with_component(hash(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsComponentType T, class Self>
    auto EntityManager::get_component(this Self& self, Entity entity, ComponentType type) noexcept
      -> core::meta::ForwardConst<Self, T&> {
        EXPECTS(self.has_entity(entity));
        EXPECTS(self.has_component(entity, type));

        auto it = stdr::find_if(self.m_components, [&type](const auto& pair) noexcept { return pair.type == type; });
        ENSURES(it != stdr::cend(self.m_components));

        auto& [_, size, _, components, _] = *it;

        auto component_it = stdr::data(components);
        for (;;) {
            auto e = *std::bit_cast<Entity*>(component_it);
            if (e != entity) {
                component_it += sizeof(Entity) + size;
                continue;
            }

            component_it += sizeof(Entity);

            break;
        }
        return std::forward_like<Self&>(*std::bit_cast<T*>(component_it));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsComponentType T, class Self>
    auto EntityManager::get_component(this Self& self, Entity entity, std::string_view name) noexcept
      -> core::meta::ForwardConst<Self, T&> {
        return self.template get_component<T>(entity, hash(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsComponentType T, class Self>
    auto EntityManager::components_of_type(this Self& self, ComponentType type) noexcept
      -> std::vector<Ref<core::meta::ForwardConst<Self, T>>> {
        // clang-format off
        return self.m_entities 
               | stdv::filter([&self, type](auto entity) noexcept { return self.has_component(entity, type); })
               | stdv::transform([&self, type](auto entity) noexcept { return self.template get_component<T>(entity, type); })
               | stdv::transform(monadic::forward_like<Self&>())
               | stdv::transform(monadic::as_ref())
               | stdr::to<std::vector>();
        // clang-format on
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsComponentType T, class Self>
    auto EntityManager::components_of_type(this Self& self, std::string_view name) noexcept
      -> std::vector<Ref<core::meta::ForwardConst<Self, T>>> {
        return self.template components_of_type<T>(hash(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::components_types_of(Entity entity) const noexcept -> std::vector<ComponentType> {
        EXPECTS(has_entity(entity));

        auto out = std::vector<ComponentType> {};
        for (auto&& [type, _, entities, _, _] : m_components) {
            for (auto e : entities)
                if (e == entity) {
                    out.emplace_back(type);
                    break;
                }
        }
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::add_system(std::string name, System::ComponentTypes types, System::Closures&& closures) noexcept
      -> System& {
        auto& system = m_systems.emplace_back(std::move(name), std::move(types), std::move(closures));

        get_needed_entities(system);

        return system;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::has_system(std::string_view name) const noexcept -> bool {
        return stdr::any_of(m_systems, [name](const auto& system) noexcept { return system.name() == name; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::remove_system(std::string_view name) noexcept -> void {
        auto&& [begin, end] = stdr::remove_if(m_systems, [&name](const auto& system) { return name == system.name(); });
        m_systems.erase(begin, end);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Self>
    auto EntityManager::systems(this Self& self) noexcept -> std::vector<Ref<core::meta::ForwardConst<Self, System>>> {
        constexpr auto as_refer = [] {
            if constexpr (core::meta::IsConst<Self>) return monadic::as_ref();
            else
                return monadic::as_ref_mut();
        }();

        return self.m_systems | stdv::transform(as_refer) | stdr::to<std::vector>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Self>
    auto EntityManager::get_system(this Self& self, std::string_view name) noexcept -> core::meta::ForwardConst<Self, System&> {
        EXPECTS(self.has_system(name));

        auto it = stdr::find_if(self.m_systems, [name](const auto& system) noexcept { return system.name() == name; });
        return std::forward_like<Self&>(*it->get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::entity_count() const noexcept -> usize {
        return std::size(m_entities);
    }
} // namespace stormkit::entities
