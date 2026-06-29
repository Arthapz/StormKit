// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/contract_macro.hpp>

export module stormkit.core:typesafe.ref;

import std;

import :meta;

import :utils.contract;

import :typesafe.boolean;
import :typesafe.integer;

import :hash;

namespace stdr = std::ranges;
namespace stdv = std::views;

export {
    namespace stormkit { inline namespace core {
        template<typename T>
        using ptr = T*;
        template<typename T>
        using owned_ptr = T*;

        template<typename T, bool Optional>
        class ref;

        template<typename T>
        using optref = ref<T, true>;

        inline constexpr struct Raw {
        } RAW;

        template<typename T, bool Optional = false>
        class ref {
          public:
            using ElementType   = T;
            using referenceType = ElementType&;
            using PointerType   = ElementType*;

            // STL compatible
            using element_type = ElementType;
            using pointer      = PointerType;

            constexpr ref(std::nullopt_t) noexcept
                requires(Optional == true);
            constexpr ref(std::nullptr_t) noexcept
                requires(Optional == true);
            constexpr ref() noexcept
                requires(Optional == true);
            constexpr ~ref() noexcept;

            template<meta::IsCanonical<T> U, bool OptionalU>
                requires(not(meta::IsConst<U> and not meta::IsConst<T>))
            constexpr ref(const ref<U, OptionalU>&) noexcept;
            template<meta::IsCanonical<T> U, bool OptionalU>
                requires(not(meta::IsConst<U> and not meta::IsConst<T>))
            constexpr ref(ref<U, OptionalU>&&) noexcept;

            constexpr auto operator=(const ref&) noexcept -> ref& = delete;

            template<meta::IsCanonical<T> U, bool OptionalU>
                requires(not(meta::IsConst<U> and not meta::IsConst<T>))
            constexpr auto operator=(ref<U, OptionalU>&& other) noexcept -> decltype(auto);

            [[nodiscard]]
            constexpr auto get() const noexcept STORMKIT_LIFETIMEBOUND -> PointerType;
            [[nodiscard]]
            constexpr auto operator->() const noexcept STORMKIT_LIFETIMEBOUND->PointerType;
            [[nodiscard]]
            constexpr auto operator*() const noexcept STORMKIT_LIFETIMEBOUND->referenceType;

            constexpr explicit operator bool() const noexcept;
            [[nodiscard]]
            constexpr auto has_value() const noexcept -> Boolean;

            [[nodiscard]]
            constexpr operator referenceType() const noexcept STORMKIT_LIFETIMEBOUND;
            [[nodiscard]]
            constexpr operator PointerType() const noexcept STORMKIT_LIFETIMEBOUND;

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
            constexpr auto operator<=>(std::nullptr_t) const noexcept
              -> std::compare_three_way_result_t<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
                requires std::three_way_comparable<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
            ;

            [[nodiscard]]
            constexpr auto operator==(std::nullopt_t) const noexcept -> bool
                requires(Optional == true);
            [[nodiscard]]
            constexpr auto operator<(std::nullopt_t) const noexcept -> bool
                requires(Optional == true);
            [[nodiscard]]
            constexpr auto operator<=(std::nullopt_t) const noexcept -> bool
                requires(Optional == true);
            [[nodiscard]]
            constexpr auto operator>(std::nullopt_t) const noexcept -> bool
                requires(Optional == true);
            [[nodiscard]]
            constexpr auto operator>=(std::nullopt_t) const noexcept -> bool
                requires(Optional == true);
            [[nodiscard]]
            constexpr auto operator<=>(std::nullopt_t) const noexcept
              -> std::compare_three_way_result_t<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
                requires(std::three_way_comparable<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
                         and Optional == true);

            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator==(const ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator<(const ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator<=(const ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator>(const ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator>=(const ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
                requires std::three_way_comparable<typename ref<T, Optional>::PointerType,
                                                   typename ref<U, OptionalU>::PointerType>
            [[nodiscard]]
            constexpr auto operator<=>(const ref<U>& other) const noexcept
              -> std::compare_three_way_result_t<typename ref<T, Optional>::PointerType, typename ref<U, OptionalU>::PointerType>;

            [[nodiscard]]
            constexpr operator std::reference_wrapper<T>() const noexcept;

          private:
            constexpr ref(referenceType value STORMKIT_LIFETIMEBOUND) noexcept;
            constexpr ref(PointerType value STORMKIT_LIFETIMEBOUND) noexcept;

            friend class ref<T, not Optional>;
            friend class ref<meta::If<meta::IsConst<T>, T, const T>, not Optional>;

            PointerType m_value;

            template<typename U>
            friend constexpr auto as_ref_raw(const U&) noexcept -> ref<const U>;

            template<typename U>
                requires(not meta::IsConst<U>)
            friend constexpr auto as_ref_mut_raw(U&) noexcept -> ref<U>;

            template<typename U>
            friend constexpr auto as_ref_like_raw(U&) noexcept -> ref<U>;

            template<typename U>
            friend constexpr auto as_optref_raw(const U&) noexcept -> optref<const U>;

            template<typename U>
                requires(not meta::IsConst<U>)
            friend constexpr auto as_optref_mut_raw(U&) noexcept -> optref<U>;

            template<typename U>
            friend constexpr auto as_optref_like_raw(U&) noexcept -> optref<U>;
        };

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_raw(const T& value) noexcept -> ref<const T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_ref(const T& value) noexcept -> ref<const T>;

        template<meta::IsPointer T>
        [[nodiscard]]
        constexpr auto as_ref(const T& value) noexcept -> ref<const meta::PointedType<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_ref(const T& value) noexcept -> ref<const meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_mut_raw(const T& value) noexcept -> ref<const T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T> and not meta::IsConst<T>)
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value) noexcept -> ref<T>;

        template<meta::IsPointer T>
            requires(not meta::IsConst<meta::PointedType<T>>)
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::PointedType<T>>;

        template<meta::IsContainer T>
            requires(not meta::IsConst<meta::ContainedType<T>>)
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_mut_like(const T& value) noexcept -> ref<const T>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_ref_like_raw(T& value) noexcept -> ref<T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_ref_like(T& value) noexcept -> ref<T>;

        template<meta::IsPointer T>
        [[nodiscard]]
        constexpr auto as_ref_like(T& value) noexcept -> ref<meta::PointedType<T>>;

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

        template<meta::IsPointer T>
        [[nodiscard]]
        constexpr auto as_optref(const T& value) noexcept -> optref<const meta::PointedType<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_optref(const T& value) noexcept -> optref<const meta::ContainedType<T>>;

        template<typename T>
            requires(not meta::IsConst<T>)
        [[nodiscard]]
        constexpr auto as_optref_mut_raw(T& value) noexcept -> optref<T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T> and not meta::IsConst<T>)
        [[nodiscard]]
        constexpr auto as_optref_mut(T& value) noexcept -> optref<T>;

        template<meta::IsPointer T>
            requires(not meta::IsConst<meta::PointedType<T>>)
        [[nodiscard]]
        constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::PointedType<T>>;

        template<meta::IsContainer T>
            requires(not meta::IsConst<meta::ContainedType<T>>)
        [[nodiscard]]
        constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::ContainedType<T>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto as_optref_like_raw(T& value) noexcept -> optref<T>;

        template<typename T>
            requires(not meta::IsContainerOrPointer<T>)
        [[nodiscard]]
        constexpr auto as_optref_like(T& value) noexcept -> optref<T>;

        template<meta::IsPointer T>
        [[nodiscard]]
        constexpr auto as_optref_like(T& value) noexcept -> optref<meta::PointedType<T>>;

        template<meta::IsContainer T>
        [[nodiscard]]
        constexpr auto as_optref_like(T& value) noexcept -> optref<meta::ContainedType<T>>;

        template<meta::IsPointer T>
        [[nodiscard]]
        constexpr auto unref(const T& value) noexcept -> const meta::PointedType<T>&;

        template<meta::IsPointer T>
            requires(not meta::IsConst<meta::PointedType<T>>)
        [[nodiscard]]
        constexpr auto unref_mut(T& value) noexcept -> meta::PointedType<T>&;

        template<template<typename, std::size_t> typename Out = array, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = array, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_refs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = array, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_optrefs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_optrefs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = array, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_optref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = dynarray, typename... Args>
            requires(not stdr::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_optref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_optrefs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = dynarray, stdr::range T>
            requires(stdr::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_optrefs(T& range) noexcept -> decltype(auto);

        template<meta::HashType Ret = hash32, typename T>
        constexpr auto hasher(const ref<T>& value) noexcept -> Ret;

        template<typename T, bool Optional, typename FormatContext>
        auto format_as(const ref<T, Optional>& value, FormatContext& ctx) noexcept -> decltype(ctx.out());
    }} // namespace stormkit::core

    template<typename T>
    struct std::pointer_traits<stormkit::ref<T>> {
        using pointer         = typename stormkit::ref<T>::PointerType;
        using element_type    = typename stormkit::ref<T>::ElementType;
        using difference_type = std::ptrdiff_t;
    };
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref() noexcept
        requires(Optional == true)
        : m_value { nullptr } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref(std::nullopt_t) noexcept
        requires(Optional == true)
        : m_value { nullptr } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref(std::nullptr_t) noexcept
        requires(Optional == true)
        : m_value { nullptr } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref(referenceType value) noexcept
        : m_value { &value } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref(PointerType value) noexcept
        : m_value { value } {
        EXPECTS(m_value != nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::~ref() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<meta::IsCanonical<T> U, bool OptionalU>
        requires(not(meta::IsConst<U> and not meta::IsConst<T>))
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref(const ref<U, OptionalU>& other) noexcept
        : m_value { other.m_value } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<meta::IsCanonical<T> U, bool OptionalU>
        requires(not(meta::IsConst<U> and not meta::IsConst<T>))
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::ref(ref<U, OptionalU>&& other) noexcept
        : m_value { std::exchange(other.m_value, nullptr) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<meta::IsCanonical<T> U, bool OptionalU>
        requires(not(meta::IsConst<U> and not meta::IsConst<T>))
    constexpr auto ref<T, Optional>::operator=(ref<U, OptionalU>&& other) noexcept -> decltype(auto) {
        if constexpr (meta::SameAs<T, U> and Optional == OptionalU)
            if (&other == this) return *this;

        m_value = std::exchange(other.m_value, nullptr);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::get() const noexcept -> PointerType {
        return m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator->() const noexcept -> PointerType {
        EXPECTS(m_value != nullptr);
        return get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator*() const noexcept -> referenceType {
        EXPECTS(m_value != nullptr);
        return *get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::operator bool() const noexcept {
        return m_value != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::has_value() const noexcept -> Boolean {
        return operator bool();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::operator referenceType() const noexcept {
        EXPECTS(m_value != nullptr);
        return *m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::operator PointerType() const noexcept {
        return m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator==(std::nullptr_t) const noexcept -> bool {
        return !m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<(std::nullptr_t) const noexcept -> bool {
        return std::less<ref<T, Optional>::pointer> {}(m_value, nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<=(std::nullptr_t) const noexcept -> bool {
        return !(nullptr < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator>(std::nullptr_t) const noexcept -> bool {
        return nullptr < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator>=(std::nullptr_t) const noexcept -> bool {
        return !(*this < nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<=>(std::nullptr_t) const noexcept
      -> std::compare_three_way_result_t<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
        requires std::three_way_comparable<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>

    {
        return std::compare_three_way {}(m_value, static_cast<typename ref<T, Optional>::pointer>(nullptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator==(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return !m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return std::less<ref<T, Optional>::pointer> {}(m_value, std::nullopt);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<=(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return !(std::nullopt < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator>(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return std::nullopt < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator>=(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return !(*this < std::nullopt);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<=>(std::nullopt_t) const noexcept
      -> std::compare_three_way_result_t<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
        requires(std::three_way_comparable<typename ref<T, Optional>::PointerType, typename ref<T, Optional>::PointerType>
                 and Optional == true)
    {
        return std::compare_three_way {}(m_value, static_cast<typename ref<T, Optional>::pointer>(std::nullopt));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator==(const ref<U, OptionalU>& other) const noexcept -> bool {
        return m_value == other.m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<(const ref<U, OptionalU>& other) const noexcept -> bool {
        return std::less<
          std::common_type_t<ref<T, Optional>::PointerType, typename ref<U, OptionalU>::PointerType>> {}(m_value, other.m_value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<=(const ref<U, OptionalU>& other) const noexcept -> bool {
        return !(other < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator>(const ref<U, OptionalU>& other) const noexcept -> bool {
        return other < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator>=(const ref<U, OptionalU>& other) const noexcept -> bool {
        return !(*this < other);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
        requires std::three_way_comparable<typename ref<T, Optional>::PointerType, typename ref<U, OptionalU>::PointerType>
    STORMKIT_FORCE_INLINE
    constexpr auto ref<T, Optional>::operator<=>(const ref<U>& other) const noexcept
      -> std::compare_three_way_result_t<typename ref<T, Optional>::PointerType, typename ref<U, OptionalU>::PointerType> {
        return m_value <=> other.m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr ref<T, Optional>::operator std::reference_wrapper<T>() const noexcept {
        if constexpr (meta::IsConst<T>) return std::cref(*m_value);
        else
            return std::ref(*m_value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_raw(const T& value) noexcept -> ref<const T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(const T& value) noexcept -> ref<const T> {
        return as_ref_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(const T& value) noexcept -> ref<const meta::PointedType<T>> {
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
    template<typename T>
        requires(not meta::IsConst<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut_raw(T& value) noexcept -> ref<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not meta::IsContainerOrPointer<T> and not meta::IsConst<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> ref<T> {
        return as_ref_mut_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
        requires(not meta::IsConst<meta::PointedType<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::PointedType<T>> {
        EXPECTS(value != nullptr);
        return as_ref_mut(unref_mut(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
        requires(not meta::IsConst<meta::ContainedType<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> ref<meta::ContainedType<T>> {
        if constexpr (requires { value.has_value(); }) EXPECTS(value.has_value());
        return as_ref_mut(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like_raw(T& value) noexcept -> ref<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T& value, Raw) noexcept -> ref<T> {
        return as_ref_like_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T& value) noexcept -> ref<meta::PointedType<T>> {
        EXPECTS(value != nullptr);
        if (meta::IsConst<meta::PointedType<T>>) return as_ref_like(unref(value));
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
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_raw(const T& value) noexcept -> optref<const T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref(const T& value) noexcept -> optref<const T> {
        return as_optref_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref(const T& value) noexcept -> optref<const meta::PointedType<T>> {
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
    template<typename T>
        requires(not meta::IsConst<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut_raw(T& value) noexcept -> optref<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not meta::IsContainerOrPointer<T> and not meta::IsConst<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut(T& value) noexcept -> optref<T> {
        return as_optref_mut(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
        requires(not meta::IsConst<meta::PointedType<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::PointedType<T>> {
        EXPECTS(value != nullptr);
        return as_optref_mut(unref_mut(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsContainer T>
        requires(not meta::IsConst<meta::ContainedType<T>>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_mut(T& value) noexcept -> optref<meta::ContainedType<T>> {
        if constexpr (requires { value.has_value(); }) EXPECTS(value.has_value());
        return as_optref_mut(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like_raw(T& value) noexcept -> optref<T> {
        return { std::addressof(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like(T& value) noexcept -> optref<T> {
        return as_optref_like_raw(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_like(T& value) noexcept -> optref<meta::PointedType<T>> {
        EXPECTS(value != nullptr);
        if (meta::IsConst<meta::PointedType<T>>) return as_optref_like(unref(value));
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
    template<meta::IsPointer T>
        STORMKIT_FORCE_INLINE
    constexpr auto unref(const T& value) noexcept -> const meta::PointedType<T>& {
        return *value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsPointer T>
        requires(not meta::IsConst<meta::PointedType<T>>)
        STORMKIT_FORCE_INLINE
    constexpr auto unref_mut(T& value) noexcept -> meta::PointedType<T>& {
        return *value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_refs(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<ref<const ValueType>, sizeof...(args)> { as_ref(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<ref<const ValueType>> { as_ref(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<ref<ValueType>, sizeof...(args)> { as_ref_mut(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<ref<ValueType>> { as_ref_mut(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(const T& range) noexcept -> decltype(auto) {
        using ValueType = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) { return as_ref(std::forward<U>(val)); })
               | stdr::to<Out<ref<const ValueType>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto) {
        using ValueType = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_ref_mut(std::forward<U>(val));
                 })
               | stdr::to<Out<ref<ValueType>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optrefs(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<optref<const ValueType>, sizeof...(args)> { as_optref(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_optrefs(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<optref<const ValueType>> { optas_ref(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_optref_muts(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<optref<ValueType>, sizeof...(args)> { as_optref_mut(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_optref_muts(Args&&... args) noexcept -> decltype(auto) {
        using ValueType = std::common_type_t<meta::ContainedOrPointedOrTType<meta::RemoveIndirectionsType<Args>>...>;
        return Out<optref<ValueType>> { as_optref_mut(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_optrefs(const T& range) noexcept -> decltype(auto) {
        using ValueType = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_optref(std::forward<U>(val));
                 })
               | stdr::to<Out<ref<const ValueType>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range T>
        requires(stdr::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_optrefs(T& range) noexcept -> decltype(auto) {
        using ValueType = stdr::range_value_t<T>;
        return range
               | stdv::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_optref_mut(std::forward<U>(val));
                 })
               | stdr::to<Out<ref<ValueType>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const ref<T>& value) noexcept -> Ret {
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
    }
}} // namespace stormkit::core
