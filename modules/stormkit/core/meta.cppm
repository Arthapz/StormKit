// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core.meta;

import std;

import stormkit.core.types;

export import stormkit.core.meta.algorithms;
export import stormkit.core.meta.concepts;
export import stormkit.core.meta.type_traits;
export import stormkit.core.meta.type_query;
export import stormkit.core.meta.type_manipulation;
export import stormkit.core.meta.priority_tag;
export import stormkit.core.meta.tag_invoke;

export namespace stormkit { inline namespace core {
    template<class... Ts>
    struct Overloaded: Ts... {
        using Ts::operator()...;
    };

    template<class... Ts>
    Overloaded(Ts...) -> Overloaded<Ts...>;

    namespace meta {
        template<class Type, class... Args>
        consteval auto find_type_index_of() noexcept -> usize {
            static_assert(IsAnyOf<Type, Args...>);
            auto i = 0u;
            ((not Is<Type, Args> and ++i) and ...);
            return i;
        }
    } // namespace meta
}} // namespace stormkit::core
