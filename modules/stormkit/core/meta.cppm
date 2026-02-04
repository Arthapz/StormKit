// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core:meta;

export import :meta.algorithms;
export import :meta.concepts;
export import :meta.type_traits;
export import :meta.type_query;
export import :meta.type_manipulation;
export import :meta.priority_tag;

export namespace stormkit { inline namespace core {
    template<class... Ts>
    struct Overloaded: Ts... {
        using Ts::operator()...;
    };

    template<class... Ts>
    Overloaded(Ts...) -> Overloaded<Ts...>;

    namespace meta {
        template<class Type, class... Args>
        consteval auto find_type_index_of() noexcept -> std::size_t {
            static_assert(IsOneOf<Type, Args...>);
            auto i = 0u;
            ((not Is<Type, Args> and ++i) and ...);
            return i;
        }
    } // namespace meta
}} // namespace stormkit::core
