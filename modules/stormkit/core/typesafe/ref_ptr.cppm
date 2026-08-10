// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/contract_macro.hpp>

export module stormkit.core.typesafe.ref_ptr;

import std;

import stormkit.core.meta;
import stormkit.core.contract;
import stormkit.core.types;

namespace stdr = std::ranges;
namespace stdv = std::views;

export {
    namespace stormkit { inline namespace core {
        template<typename T>
        using owned_raw_ptr = T*;

        template<meta::negate<meta::raw_indirection> T>
        class STORMKIT_VIEW ref_ptr {
          public:
            using element_type  = T;
            using pointer       = T*;
            using const_pointer = const T*;

            constexpr ref_ptr(pointer pointed STORMKIT_LIFETIMEBOUND) noexcept;
            constexpr ~ref_ptr() noexcept;

            template<meta::is<element_type> U>
            constexpr ref_ptr(const ref_ptr<U>&) noexcept;
            template<meta::is<element_type> U>
            constexpr ref_ptr(ref_ptr<U>&&) noexcept;

            template<meta::is<element_type> U>
            constexpr auto operator=(ref_ptr<U> other) noexcept -> ref_ptr&;

            template<typename Self>
            [[nodiscard]]
            constexpr auto operator->(this Self& self) noexcept -> meta::forward_const_to<Self, element_type>*;

            template<typename Self>
            [[nodiscard]]
            constexpr auto operator*(this Self& self) noexcept -> meta::forward_const_to<Self, element_type>&;

            template<typename Self>
            [[nodiscard]]
            constexpr auto get(this Self& self) noexcept -> meta::forward_const_to<Self, element_type>*;

            [[nodiscard]]
            constexpr operator pointer() noexcept;
            [[nodiscard]]
            constexpr operator const_pointer() const noexcept;
            [[nodiscard]]
            constexpr operator std::reference_wrapper<element_type>() const noexcept;
            [[nodiscard]]
            constexpr explicit operator bool() const noexcept;

            [[nodiscard]]
            constexpr auto operator==(std::nullptr_t) const noexcept -> bool;
            [[nodiscard]]
            constexpr auto operator<(std::nullptr_t) const noexcept -> bool;
            [[nodiscard]]
            constexpr auto operator<=(std::nullptr_t) const noexcept -> bool;
            [[nodiscard]]
            constexpr auto operator>(std::nullptr_t) const noexcept -> bool;
            [[nodiscard]]
            constexpr auto operator>=(std::nullptr_t) const noexcept -> bool;
            [[nodiscard]]
            constexpr auto operator<=>(std::nullptr_t) const noexcept -> std::compare_three_way_result_t<pointer, pointer>
                requires std::three_way_comparable<pointer, pointer>;

            template<meta::negate<meta::raw_indirection> U>
            [[nodiscard]]
            constexpr auto operator==(const ref_ptr<U>&) const noexcept -> bool;
            template<meta::negate<meta::raw_indirection> U>
            [[nodiscard]]
            constexpr auto operator<(const ref_ptr<U>&) const noexcept -> bool;
            template<meta::negate<meta::raw_indirection> U>
            [[nodiscard]]
            constexpr auto operator<=(const ref_ptr<U>&) const noexcept -> bool;
            template<meta::negate<meta::raw_indirection> U>
            [[nodiscard]]
            constexpr auto operator>(const ref_ptr<U>&) const noexcept -> bool;
            template<meta::negate<meta::raw_indirection> U>
            [[nodiscard]]
            constexpr auto operator>=(const ref_ptr<U>&) const noexcept -> bool;
            template<meta::negate<meta::raw_indirection> U>
            [[nodiscard]]
            constexpr auto operator<=>(const ref_ptr<U>& other) const noexcept
              -> std::compare_three_way_result_t<pointer, meta::pointer_type<ref_ptr<U>>>
                requires std::three_way_comparable<pointer, meta::pointer_type<ref_ptr<U>>>;

          private:
            pointer m_pointed;
        };

        /*
        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_raw(const T& value) noexcept -> ref<const T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_ref(const T& value) noexcept -> ref<const T>;

        template<meta::pointer T>
        [[nodiscard]]
        constexpr auto as_ref(const T& value) noexcept -> ref<const meta::pointed_type<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_ref(const T& value) noexcept -> ref<const meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_mut_raw(const T& value) noexcept -> ref<const T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T> and not meta::const_type<T>)
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value) noexcept -> ref_ptr<T>;

        template<meta::pointer T>
            requires(not meta::const_type<meta::pointed_type<T>>)
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::pointed_type<T>>;

        template<meta::IsContainer T>
            requires(not meta::const_type<meta::ContainedType<T>>)
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_mut_like(const T& value) noexcept -> ref<const T>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_like_raw(T& value) noexcept -> ref_ptr<T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_ref_like(T& value) noexcept -> ref_ptr<T>;

        template<meta::pointer T>
        [[nodiscard]]
        constexpr auto as_ref_like(T& value) noexcept -> ref<meta::pointed_type<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_ref_like(T& value) noexcept -> ref<meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_optref_raw(const T& value) noexcept -> optref<const T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_optref(const T& value) noexcept -> optref<const T>;

        template<meta::pointer T>
        [[nodiscard]]
        constexpr auto as_optref(const T& value) noexcept -> optref<const meta::pointed_type<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_optref(const T& value) noexcept -> optref<const meta::ContainedType<T>>;

        template<typename T>
            requires(not meta::const_type<T>)
        [[nodiscard]]
        constexpr auto as_optref_mut_raw(T& value) noexcept -> optref_ptr<T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T> and not meta::const_type<T>)
        [[nodiscard]]
        constexpr auto as_optref_mut(T& value) noexcept -> optref_ptr<T>;

        template<meta::pointer T>
            requires(not meta::const_type<meta::pointed_type<T>>)
        [[nodiscard]]
        constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::pointed_type<T>>;

        template<meta::IsContainer T>
            requires(not meta::const_type<meta::ContainedType<T>>)
        [[nodiscard]]
        constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_optref_like_raw(T& value) noexcept -> optref_ptr<T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_optref_like(T& value) noexcept -> optref_ptr<T>;

        template<meta::pointer T>
        [[nodiscard]]
        constexpr auto as_optref_like(T& value) noexcept -> optref<meta::pointed_type<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_optref_like(T& value) noexcept -> optref<meta::ContainedType<T>>;

        template<meta::pointer T>
        [[nodiscard]]
        constexpr auto unref(const T& value) noexcept -> const meta::pointed_type<T>&;

        template<meta::pointer T>
            requires(not meta::const_type<meta::pointed_type<T>>)
        [[nodiscard]]
        constexpr auto unref_mut(T& value) noexcept -> meta::pointed_type<T>&;

        template<template<typename, usize> typename Out = array, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto as_refs(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto to_refs(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename, usize> typename Out = array, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto as_ref_muts(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto to_ref_muts(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_refs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto);

        template<template<typename, usize> typename Out = array, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto as_optrefs(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto to_optrefs(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename, usize> typename Out = array, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto as_optref_muts(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Ts>
            requires(not stdr::range<Ts> and ...)
        [[nodiscard]]
        constexpr auto to_optref_muts(Ts&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_optrefs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_optrefs(T& range) noexcept -> decltype(auto);

        template<meta::hash_type Ret = hash32, typename T>
        constexpr auto hasher(const ref_ptr<T>& value) noexcept -> Ret;

        template<typename T, bool Optional, typename FormatContext>
        auto format_as(const ref<T, Optional>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()); */
    }} // namespace stormkit::core

    template<typename T>
    struct std::pointer_traits<stormkit::core::ref_ptr<T>> {
        using pointer         = typename stormkit::core::ref_ptr<T>::pointer;
        using element_type    = typename stormkit::core::ref_ptr<T>::element_type;
        using difference_type = std::ptrdiff_t;
    };
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::ref_ptr(pointer pointed STORMKIT_LIFETIMEBOUND) noexcept
        : m_pointed { pointed } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::~ref_ptr() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::is<meta::element_type<ref_ptr<T>>> U>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::ref_ptr(const ref_ptr<U>& other) noexcept
        : m_pointed { as<pointer>(other.m_pointed) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::is<meta::element_type<ref_ptr<T>>> U>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::ref_ptr(ref_ptr<U>&& other) noexcept
        : m_pointed { as<pointer>(other.m_pointed) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::is<meta::element_type<ref_ptr<T>>> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator=(ref_ptr<U> other) noexcept -> ref_ptr& {
        if (&other == this) [[unlikely]]
            return *this;

        m_pointed = as<pointer>(other.pointed);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator->(this Self& self) noexcept -> meta::forward_const_to<Self, element_type>* {
        return as<meta::forward_const_to<Self, element_type>*>(self.m_pointed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    template<typename Self>
    constexpr auto ref_ptr<T>::operator*(this Self& self) noexcept -> meta::forward_const_to<Self, element_type>& {
        return *self.m_pointed;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    template<typename Self>
    constexpr auto ref_ptr<T>::get(this Self& self) noexcept -> meta::forward_const_to<Self, element_type>* {
        return as<meta::forward_const_to<Self, element_type>*>(self.m_pointed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::operator pointer() noexcept {
        return m_pointed;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::operator const_pointer() const noexcept {
        return as<const_pointer>(m_pointed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr ref_ptr<T>::operator std::reference_wrapper<element_type>() const noexcept {
        if constexpr (meta::const_type<T>) return std::cref(*m_pointed);
        else
            return std::ref(*m_pointed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr ref_ptr<T>::operator bool() const noexcept {
        return true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto ref_ptr<T>::operator==(std::nullptr_t) const noexcept -> bool {
        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator<(std::nullptr_t) const noexcept -> bool {
        return std::less<pointer> {}(m_pointed, nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator<=(std::nullptr_t) const noexcept -> bool {
        return !(nullptr < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator>(std::nullptr_t) const noexcept -> bool {
        return nullptr < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator>=(std::nullptr_t) const noexcept -> bool {
        return !(*this < nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator<=>(std::nullptr_t) const noexcept -> std::compare_three_way_result_t<pointer, pointer>
        requires std::three_way_comparable<pointer, pointer>
    {
        return std::compare_three_way {}(m_pointed, static_cast<pointer>(nullptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::negate<meta::raw_indirection> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator==(const ref_ptr<U>& other) const noexcept -> bool {
        return m_pointed == other.m_pointed;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::negate<meta::raw_indirection> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator<(const ref_ptr<U>& other) const noexcept -> bool {
        return std::less<std::common_type_t<pointer, meta::pointer_type<ref_ptr<U>>>> {}(m_pointed, other.m_pointed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::negate<meta::raw_indirection> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator<=(const ref_ptr<U>& other) const noexcept -> bool {
        return !(other < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::negate<meta::raw_indirection> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator>(const ref_ptr<U>& other) const noexcept -> bool {
        return other < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::negate<meta::raw_indirection> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator>=(const ref_ptr<U>& other) const noexcept -> bool {
        return !(*this < other);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    template<meta::negate<meta::raw_indirection> U>
    STORMKIT_FORCE_INLINE
    constexpr auto ref_ptr<T>::operator<=>(const ref_ptr<U>& other) const noexcept
      -> std::compare_three_way_result_t<pointer, meta::pointer_type<ref_ptr<U>>>
        requires std::three_way_comparable<pointer, meta::pointer_type<ref_ptr<U>>>
    {
        return m_pointed <=> other.m_pointed;
    }

    /*
    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_raw(const T& value) noexcept -> ref<const T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(const T& value) noexcept -> ref<const T> {
        return as_ref_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(const T& value) noexcept -> ref<const meta::pointed_type<T>> {
        EXPECTS(value != nullptr);
        return as_ref(unref(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(const T& value) noexcept -> ref<const meta::ContainedType<T>> {
        EXPECTS(value.operator bool());
        return as_ref(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::const_type<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut_raw(T& value) noexcept -> ref_ptr<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::IsContainerOrPointer<T> and not meta::const_type<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> ref_ptr<T> {
        return as_ref_mut_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
        requires(not meta::const_type<meta::pointed_type<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::pointed_type<T>> {
        EXPECTS(value != nullptr);
        return as_ref_mut(unref_mut(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
        requires(not meta::const_type<meta::ContainedType<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::ContainedType<T>> {
        if constexpr (requires { value.has_value(); }) EXPECTS(value.has_value());
        return as_ref_mut(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like_raw(T& value) noexcept -> ref_ptr<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T& value, Raw) noexcept -> ref_ptr<T> {
        return as_ref_like_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T& value) noexcept -> ref<meta::pointed_type<T>> {
        EXPECTS(value != nullptr);
        if (meta::const_type<meta::pointed_type<T>>) return as_ref_like(unref(value));
        else
            return as_ref_like(unref_mut(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T& value) noexcept -> ref<meta::ContainedType<T>> {
        if constexpr (requires { value.has_value(); }) EXPECTS(value.has_value());
        return as_ref_like(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_raw(const T& value) noexcept -> optref<const T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref(const T& value) noexcept -> optref<const T> {
        return as_optref_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref(const T& value) noexcept -> optref<const meta::pointed_type<T>> {
        EXPECTS(value != nullptr);
        return as_optref(unref(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref(const T& value) noexcept -> optref<const meta::ContainedType<T>> {
        EXPECTS(value.operator bool());
        return as_optref(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::const_type<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut_raw(T& value) noexcept -> optref_ptr<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::IsContainerOrPointer<T> and not meta::const_type<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut(T& value) noexcept -> optref_ptr<T> {
        return as_optref_mut(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
        requires(not meta::const_type<meta::pointed_type<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::pointed_type<T>> {
        EXPECTS(value != nullptr);
        return as_optref_mut(unref_mut(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
        requires(not meta::const_type<meta::ContainedType<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::ContainedType<T>> {
        if constexpr (requires { value.has_value(); }) EXPECTS(value.has_value());
        return as_optref_mut(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like_raw(T& value) noexcept -> optref_ptr<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::negate<meta::raw_indirection> T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like(T& value) noexcept -> optref_ptr<T> {
        return as_optref_like_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like(T& value) noexcept -> optref<meta::pointed_type<T>> {
        EXPECTS(value != nullptr);
        if (meta::const_type<meta::pointed_type<T>>) return as_optref_like(unref(value));
        else
            return as_optref_like(unref_mut(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like(T& value) noexcept -> optref<meta::ContainedType<T>> {
        if constexpr (requires { value.has_value(); }) EXPECTS(value.has_value());
        return as_optref_like(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
        STORMKIT_FORCE_INLINE
    constexpr auto unref(const T& value) noexcept -> const meta::pointed_type<T>& {
        return *value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::pointer T>
        requires(not meta::const_type<meta::pointed_type<T>>)
        STORMKIT_FORCE_INLINE
    constexpr auto unref_mut(T& value) noexcept -> meta::pointed_type<T>& {
        return *value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, usize> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_refs(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<ref<const value_type>, sizeof...(args)> { as_ref(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<ref<const value_type>> { as_ref(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, usize> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_muts(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<ref<value_type>, sizeof...(args)> { as_ref_mut(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_ref_muts(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<ref<value_type>> { as_ref_mut(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(const T& range) noexcept -> decltype(auto) {
        using value_type = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) { return as_ref(std::forward<U>(val)); })
               | stdr::to<Out<ref<const value_type>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto) {
        using value_type = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_ref_mut(std::forward<U>(val));
                 })
               | stdr::to<Out<ref<value_type>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, usize> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optrefs(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<optref<const value_type>, sizeof...(args)> { as_optref(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_optrefs(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<optref<const value_type>> { optas_ref(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, usize> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_muts(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<optref<value_type>, sizeof...(args)> { as_optref_mut(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Ts>
        requires(not stdr::range<Ts> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_optref_muts(Ts&&... args) noexcept -> decltype(auto) {
        using value_type = std::common_type_t<meta::ContainedOrPointedOrTType<meta::remove_indirections_of<Ts>>...>;
        return Out<optref<value_type>> { as_optref_mut(std::forward<Ts>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_optrefs(const T& range) noexcept -> decltype(auto) {
        using value_type = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_optref(std::forward<U>(val));
                 })
               | stdr::to<Out<ref<const value_type>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_optrefs(T& range) noexcept -> decltype(auto) {
        using value_type = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_optref_mut(std::forward<U>(val));
                 })
               | stdr::to<Out<ref<value_type>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::hash_type Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const ref_ptr<T>& value) noexcept -> Ret {
        return hash<Ret>(value.get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional, typename FormatContext>
    inline auto format_as(const ref<T, Optional>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        if constexpr (Optional) {
            if (value == nullptr) return std::format_to(ctx.out(), "[ref value: null]");
            else
                return std::format_to(ctx.out(), "[ref value: {}]", *value);
        } else
            return std::format_to(ctx.out(), "[ref value: {}]", *value);
    }*/
}} // namespace stormkit::core
