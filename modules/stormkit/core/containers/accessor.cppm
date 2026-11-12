// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.containers.accessor;

import std;

import stormkit.core.types;
import stormkit.core.typesafe.safecasts;
import stormkit.core.meta.type_manipulation;
import stormkit.core.meta.concepts;

export namespace stormkit { inline namespace core {
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    struct STORMKIT_VIEW element_accessor {
        using value_type = T;

        static constexpr auto index = Index;

        Storage_type storage;

        using element_type = meta::forward_ref_to<Storage_type, meta::forward_const_to<Storage_type, T>>;

        constexpr auto value(this auto&& self) noexcept -> element_type;

        constexpr operator element_type(this auto&& self) noexcept;

        constexpr auto operator&() noexcept -> meta::remove_refs_of<element_type>*
            requires(meta::lvalue_ref<element_type>);

        template<typename U>
        constexpr auto operator=(U&& val) noexcept -> element_accessor&
            requires(meta::assignable_from<element_type, U>);

        constexpr auto operator*(this auto&& self) noexcept -> decltype(auto)
            requires(meta::has_deref_operator<element_type>);
        constexpr auto operator->() noexcept -> meta::remove_refs_of<element_type>*
            requires(meta::has_deref_call_operator<element_type>);

        template<typename U>
        constexpr auto operator==(const U& other) const noexcept -> bool
            requires(meta::has_equality_operator<T, U>);

        template<typename U>
        constexpr auto operator<=>(const U& other) const noexcept -> decltype(value() <=> other)
            requires(meta::has_three_way_comparaison_operator<T, U>);
    };

    namespace meta {
        template<typename T>
        concept is_element_accessor = specialization_of_nttp_ttv<T, element_accessor>;

        namespace plain {
            template<typename T>
            concept is_element_accessor = meta::is_element_accessor<meta::to_plain_type<T>>;
        }

        namespace decayed {
            template<typename T>
            concept is_element_accessor = meta::is_element_accessor<meta::to_decayed_type<T>>;
        }
    } // namespace meta

    template<meta::is_element_accessor First, typename Second>
        requires(stormkit::core::is_fn<stormkit::core::equal>::template tag_invokable<typename First::value_type, Second>)
    [[nodiscard]]
    constexpr auto tag_invoke(is_fn<equal>, const First& first, const Second& second) noexcept -> bool;

    template<typename To, meta::plain::is_element_accessor Element>
        requires(stormkit::core::as_fn<To>::template tag_invokable<typename Element::value_type>)
    [[nodiscard]]
    constexpr auto tag_invoke(as_fn<To>, const Element& element, source_location_arg = std::source_location::current()) noexcept
      -> To;

    template<typename T, typename Storage_type, template<class, class> typename... Bound_accessors>
        requires(meta::reference<Storage_type>)
    struct STORMKIT_VIEW accessor_view final: public Bound_accessors<T, Storage_type>... {
        constexpr explicit accessor_view(Storage_type storage) noexcept;

        template<typename Self>
        [[nodiscard]]
        constexpr auto operator->(this Self&& self) noexcept -> decltype(auto);
    };
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    STORMKIT_FORCE_INLINE
    constexpr auto element_accessor<T, Storage_type, Index>::value(this auto&& self) noexcept -> element_type {
        return std::forward<element_type>(self.storage[index]);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    STORMKIT_FORCE_INLINE
    constexpr element_accessor<T, Storage_type, Index>::operator element_type(this auto&& self) noexcept {
        return std::forward<element_type>(self.value());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    STORMKIT_FORCE_INLINE
    constexpr auto element_accessor<T, Storage_type, Index>::operator&() noexcept -> meta::remove_refs_of<element_type>*
        requires(meta::lvalue_ref<element_type>)
    {
        return &value();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    template<typename U>
    STORMKIT_FORCE_INLINE
    constexpr auto element_accessor<T, Storage_type, Index>::operator=(U&& val) noexcept -> element_accessor&
        requires(meta::assignable_from<element_type, U>)
    {
        storage[index] = std::forward<U>(val);
        return *this;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    STORMKIT_FORCE_INLINE
    constexpr auto element_accessor<T, Storage_type, Index>::operator*(this auto&& self) noexcept -> decltype(auto)
        requires(meta::has_deref_operator<element_type>)
    {
        return *self.value();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    STORMKIT_FORCE_INLINE
                constexpr auto element_accessor<T, Storage_type, Index>::operator->() noexcept
                -> meta::remove_refs_of<element_type>*
                    requires(meta::has_deref_call_operator<element_type>)
    {
        return &value();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    template<typename U>
    STORMKIT_FORCE_INLINE
    constexpr auto element_accessor<T, Storage_type, Index>::operator==(const U& other) const noexcept -> bool
        requires(meta::has_equality_operator<T, U>)
    {
        return value() == other;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, usize Index>
        requires(meta::reference<Storage_type>)
    template<typename U>
    STORMKIT_FORCE_INLINE
    constexpr auto element_accessor<T, Storage_type, Index>::operator<=>(const U& other) const noexcept
      -> decltype(value() <=> other)
        requires(meta::has_three_way_comparaison_operator<T, U>)
    {
        return value() <=> other;
    }

    template<meta::is_element_accessor First, typename Second>
        requires(stormkit::core::is_fn<stormkit::core::equal>::template tag_invokable<typename First::value_type, Second>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(is_fn<equal>, const First& first, const Second& second) noexcept -> bool {
        return is(first.value(), second);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename To, meta::plain::is_element_accessor Element>
        requires(stormkit::core::as_fn<To>::template tag_invokable<typename Element::value_type>)
    STORMKIT_FORCE_INLINE
    constexpr auto tag_invoke(as_fn<To>, const Element& element, source_location_arg) noexcept -> To {
        return as<To>(element.value());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, template<class, class> typename... Bound_accessors>
        requires(meta::reference<Storage_type>)
    STORMKIT_FORCE_INLINE
    constexpr accessor_view<T, Storage_type, Bound_accessors...>::accessor_view(Storage_type storage) noexcept
        : Bound_accessors<T, Storage_type> { std::forward<Storage_type>(storage) }... {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage_type, template<class, class> typename... Bound_accessors>
        requires(meta::reference<Storage_type>)
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto accessor_view<T, Storage_type, Bound_accessors...>::operator->(this Self&& self) noexcept -> decltype(auto) {
        return &self;
    }
}} // namespace stormkit::core
