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

export {
    namespace stormkit { inline namespace core {
        namespace meta {
            template<class T, class U>
            concept ContainedOrPointerOf = (IsContainedSemantics<T> or IsPointer<T>) and SameAs<U, PointedType<T>>;
        }

        template<typename T>
        using ptr = T*;
        template<typename T>
        using owned_ptr = T*;

        template<typename T, bool Optional = false>
        class Ref;

        template<typename T>
        using OptionalRef = Ref<T, true>;

        template<typename T, bool Optional = false>
        class Ref {
          public:
            using ElementType   = T;
            using ReferenceType = ElementType&;
            using PointerType   = ElementType*;

            // STL compatible
            using element_type = ElementType;
            using pointer      = PointerType;

            constexpr Ref(std::nullopt_t) noexcept
                requires(Optional == true);
            constexpr Ref(std::nullptr_t) noexcept
                requires(Optional == true);
            constexpr Ref() noexcept
                requires(Optional == true);
            constexpr ~Ref() noexcept;

            template<meta::IsCanonical<T> U, bool OptionalU>
                requires(not(meta::IsConst<U> and not meta::IsConst<T>))
            constexpr Ref(const Ref<U, OptionalU>&) noexcept;
            template<meta::IsCanonical<T> U, bool OptionalU>
                requires(not(meta::IsConst<U> and not meta::IsConst<T>))
            constexpr Ref(Ref<U, OptionalU>&&) noexcept;

            constexpr auto operator=(const Ref&) noexcept -> Ref& = delete;

            template<meta::IsCanonical<T> U, bool OptionalU>
                requires(not(meta::IsConst<U> and not meta::IsConst<T>))
            constexpr auto operator=(Ref<U, OptionalU>&& other) noexcept -> decltype(auto);

            [[nodiscard]]
            constexpr auto get() const noexcept STORMKIT_LIFETIMEBOUND -> PointerType;
            [[nodiscard]]
            constexpr auto operator->() const noexcept STORMKIT_LIFETIMEBOUND->PointerType;
            [[nodiscard]]
            constexpr auto operator*() const noexcept STORMKIT_LIFETIMEBOUND->ReferenceType;

            constexpr explicit operator bool() const noexcept;
            [[nodiscard]]
            constexpr auto has_value() const noexcept -> Boolean;

            [[nodiscard]]
            constexpr operator ReferenceType() const noexcept STORMKIT_LIFETIMEBOUND;
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
              -> std::compare_three_way_result_t<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
                requires std::three_way_comparable<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
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
              -> std::compare_three_way_result_t<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
                requires(std::three_way_comparable<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
                         and Optional == true);

            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator==(const Ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator<(const Ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator<=(const Ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator>(const Ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
            [[nodiscard]]
            constexpr auto operator>=(const Ref<U, OptionalU>&) const noexcept -> bool;
            template<typename U, bool OptionalU>
                requires std::three_way_comparable<typename Ref<T, Optional>::PointerType,
                                                   typename Ref<U, OptionalU>::PointerType>
            [[nodiscard]]
            constexpr auto operator<=>(const Ref<U>& other) const noexcept
              -> std::compare_three_way_result_t<typename Ref<T, Optional>::PointerType, typename Ref<U, OptionalU>::PointerType>;

            [[nodiscard]]
            constexpr operator std::reference_wrapper<T>() const noexcept;

          private:
            constexpr Ref(ReferenceType value STORMKIT_LIFETIMEBOUND) noexcept;
            constexpr Ref(PointerType value STORMKIT_LIFETIMEBOUND) noexcept;

            friend class Ref<T, not Optional>;
            friend class Ref<meta::If<meta::IsConst<T>, T, const T>, not Optional>;

            PointerType m_value;

            template<bool RAW, typename U>
            friend constexpr auto as_ref(const U&) noexcept -> Ref<const meta::PointedType<U>>;

            template<bool RAW, typename U>
                requires meta::IsNotConst<meta::PointedType<U>>
            friend constexpr auto as_ref_mut(U&) noexcept -> Ref<meta::PointedType<U>>;

            template<bool RAW, typename U>
            friend constexpr auto as_ref_like(U&) noexcept -> Ref<meta::AddConstIf<meta::IsConst<U>, meta::PointedType<U>>>;

            template<bool RAW, typename U>
            friend constexpr auto as_opt_ref(const U&) noexcept -> OptionalRef<const meta::PointedType<U>>;

            template<bool RAW, typename U>
                requires meta::IsNotConst<meta::PointedType<U>>
            friend constexpr auto as_opt_ref_mut(U&) noexcept -> OptionalRef<const meta::PointedType<U>>;

            template<bool RAW, typename U>
            friend constexpr auto as_opt_ref_like(U&) noexcept
              -> OptionalRef<meta::AddConstIf<meta::IsConst<U>, meta::PointedType<U>>>;
        };

        template<bool RAW = false, typename T>
        [[nodiscard]]
        constexpr auto as_ref(const T& value STORMKIT_LIFETIMEBOUND) noexcept -> Ref<const meta::PointedType<T>>;

        template<bool RAW = false, typename T>
            requires meta::IsNotConst<meta::PointedType<T>>
        [[nodiscard]]
        constexpr auto as_ref_mut(T& value STORMKIT_LIFETIMEBOUND) noexcept -> Ref<meta::PointedType<T>>;

        template<bool RAW = false, typename T>
        [[nodiscard]]
        constexpr auto as_ref_like(T& value STORMKIT_LIFETIMEBOUND) noexcept
          -> Ref<meta::AddConstIf<meta::IsConst<T>, meta::PointedType<T>>>;

        template<bool RAW = false, typename T>
        [[nodiscard]]
        constexpr auto as_opt_ref(const T& value STORMKIT_LIFETIMEBOUND) noexcept -> OptionalRef<const meta::PointedType<T>>;

        template<bool RAW = false, typename T>
            requires meta::IsNotConst<meta::PointedType<T>>
        [[nodiscard]]
        constexpr auto as_opt_ref_mut(T& value STORMKIT_LIFETIMEBOUND) noexcept -> OptionalRef<const meta::PointedType<T>>;

        template<bool RAW = false, typename T>
        [[nodiscard]]
        constexpr auto as_opt_ref_like(T& value STORMKIT_LIFETIMEBOUND) noexcept
          -> OptionalRef<meta::AddConstIf<meta::IsConst<T>, meta::PointedType<T>>>;

        template<typename T>
        [[nodiscard]]
        constexpr auto unref(const Ref<T>& value STORMKIT_LIFETIMEBOUND) noexcept -> const T&;

        template<meta::IsNotConst T>
        [[nodiscard]]
        constexpr auto unref_mut(const Ref<T>& value STORMKIT_LIFETIMEBOUND) noexcept -> T&;

        template<template<typename, std::size_t> typename Out = std::array, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = std::array, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, bool RAW = false, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_refs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, bool RAW = false, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = std::array, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_opt_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_opt_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = std::array, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_opt_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, bool RAW = false, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_opt_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, bool RAW = false, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_opt_refs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, bool RAW = false, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_opt_refs(T& range) noexcept -> decltype(auto);

        template<meta::HashType Ret = hash32, typename T>
        constexpr auto hasher(const Ref<T>& value) noexcept -> Ret;

        template<typename T, bool Optional, typename FormatContext>
        auto format_as(const Ref<T, Optional>& value, FormatContext& ctx) noexcept -> decltype(ctx.out());
    }} // namespace stormkit::core

    template<typename T>
    struct std::pointer_traits<stormkit::Ref<T>> {
        using pointer         = typename stormkit::Ref<T>::PointerType;
        using element_type    = typename stormkit::Ref<T>::ElementType;
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
    constexpr Ref<T, Optional>::Ref() noexcept
        requires(Optional == true)
        : m_value { nullptr } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::Ref(std::nullopt_t) noexcept
        requires(Optional == true)
        : m_value { nullptr } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::Ref(std::nullptr_t) noexcept
        requires(Optional == true)
        : m_value { nullptr } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::Ref(ReferenceType value) noexcept
        : m_value { &value } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::Ref(PointerType value) noexcept
        : m_value { value } {
        EXPECTS(m_value != nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::~Ref() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<meta::IsCanonical<T> U, bool OptionalU>
        requires(not(meta::IsConst<U> and not meta::IsConst<T>))
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::Ref(const Ref<U, OptionalU>& other) noexcept
        : m_value { other.m_value } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<meta::IsCanonical<T> U, bool OptionalU>
        requires(not(meta::IsConst<U> and not meta::IsConst<T>))
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::Ref(Ref<U, OptionalU>&& other) noexcept
        : m_value { std::exchange(other.m_value, nullptr) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<meta::IsCanonical<T> U, bool OptionalU>
        requires(not(meta::IsConst<U> and not meta::IsConst<T>))
    constexpr auto Ref<T, Optional>::operator=(Ref<U, OptionalU>&& other) noexcept -> decltype(auto) {
        if constexpr (meta::IsStrict<T, U> and Optional == OptionalU)
            if (&other == this) return *this;

        m_value = std::exchange(other.m_value, nullptr);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::get() const noexcept -> PointerType {
        return m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator->() const noexcept -> PointerType {
        EXPECTS(m_value != nullptr);
        return get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator*() const noexcept -> ReferenceType {
        EXPECTS(m_value != nullptr);
        return *get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::operator bool() const noexcept {
        return m_value != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::has_value() const noexcept -> Boolean {
        return operator bool();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::operator ReferenceType() const noexcept {
        EXPECTS(m_value != nullptr);
        return *m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::operator PointerType() const noexcept {
        return m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator==(std::nullptr_t) const noexcept -> bool {
        return !m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<(std::nullptr_t) const noexcept -> bool {
        return std::less<Ref<T, Optional>::pointer> {}(m_value, nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<=(std::nullptr_t) const noexcept -> bool {
        return !(nullptr < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator>(std::nullptr_t) const noexcept -> bool {
        return nullptr < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator>=(std::nullptr_t) const noexcept -> bool {
        return !(*this < nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<=>(std::nullptr_t) const noexcept
      -> std::compare_three_way_result_t<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
        requires std::three_way_comparable<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>

    {
        return std::compare_three_way {}(m_value, static_cast<typename Ref<T, Optional>::pointer>(nullptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator==(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return !m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return std::less<Ref<T, Optional>::pointer> {}(m_value, std::nullopt);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<=(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return !(std::nullopt < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator>(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return std::nullopt < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator>=(std::nullopt_t) const noexcept -> bool
        requires(Optional == true)
    {
        return !(*this < std::nullopt);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<=>(std::nullopt_t) const noexcept
      -> std::compare_three_way_result_t<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
        requires(std::three_way_comparable<typename Ref<T, Optional>::PointerType, typename Ref<T, Optional>::PointerType>
                 and Optional == true)
    {
        return std::compare_three_way {}(m_value, static_cast<typename Ref<T, Optional>::pointer>(std::nullopt));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator==(const Ref<U, OptionalU>& other) const noexcept -> bool {
        return m_value == other.m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<(const Ref<U, OptionalU>& other) const noexcept -> bool {
        return std::less<
          std::common_type_t<Ref<T, Optional>::PointerType, typename Ref<U, OptionalU>::PointerType>> {}(m_value, other.m_value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<=(const Ref<U, OptionalU>& other) const noexcept -> bool {
        return !(other < *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator>(const Ref<U, OptionalU>& other) const noexcept -> bool {
        return other < *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator>=(const Ref<U, OptionalU>& other) const noexcept -> bool {
        return !(*this < other);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    template<typename U, bool OptionalU>
        requires std::three_way_comparable<typename Ref<T, Optional>::PointerType, typename Ref<U, OptionalU>::PointerType>
    STORMKIT_FORCE_INLINE
    constexpr auto Ref<T, Optional>::operator<=>(const Ref<U>& other) const noexcept
      -> std::compare_three_way_result_t<typename Ref<T, Optional>::PointerType, typename Ref<U, OptionalU>::PointerType> {
        return m_value <=> other.m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional>
    STORMKIT_FORCE_INLINE
    constexpr Ref<T, Optional>::operator std::reference_wrapper<T>() const noexcept {
        if constexpr (meta::IsConst<T>) return std::cref(*m_value);
        else
            return std::ref(*m_value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<bool RAW, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(const T& value) noexcept -> Ref<const meta::PointedType<T>> {
        using OutRef = Ref<const meta::PointedType<T>>;

        if constexpr (not RAW and meta::IsPointer<T>) {
            EXPECTS(value != nullptr);
            return OutRef { std::addressof(*value) };
        } else if constexpr (not RAW and meta::IsContainedSemantics<T>) {
            EXPECTS(value.operator bool());
            return OutRef { &(value.operator*()) };
        } else {
            return OutRef { std::addressof(value) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<bool RAW, typename T>
        requires meta::IsNotConst<meta::PointedType<T>> 
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T& value) noexcept -> Ref<meta::PointedType<T>> {
        using OutRef = Ref<meta::PointedType<T>>;

        if constexpr (not RAW and meta::IsPointer<T>) {
            EXPECTS(value != nullptr);
            return OutRef { std::addressof(*value) };
        } else if constexpr (not RAW and meta::IsContainedSemantics<T>) {
            EXPECTS(value.operator bool());
            return OutRef { &(value.operator*()) };
        } else {
            return OutRef { std::addressof(value) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<bool RAW, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T& value) noexcept -> Ref<meta::AddConstIf<meta::IsConst<T>, meta::PointedType<T>>> {
        using OutRef = Ref<meta::AddConstIf<meta::IsConst<T>, meta::PointedType<T>>>;

        if constexpr (not RAW and meta::IsPointer<T>) {
            EXPECTS(value != nullptr);
            return OutRef { std::addressof(*value) };
        } else if constexpr (not RAW and meta::IsContainedSemantics<T>) {
            EXPECTS(value.operator bool());
            return OutRef { &(value.operator*()) };
        } else {
            return OutRef { std::addressof(value) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<bool RAW, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref(const T& value) noexcept -> OptionalRef<const meta::PointedType<T>> {
        using OutRef = OptionalRef<const meta::PointedType<T>>;

        if constexpr (not RAW and meta::IsPointer<T>) {
            return OutRef { std::addressof(*value) };
        } else if constexpr (not RAW and meta::IsContainedSemantics<T>) {
            return OutRef { &(value.operator*()) };
        } else {
            return OutRef { std::addressof(value) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<bool RAW, typename T>
        requires meta::IsNotConst<meta::PointedType<T>> 
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref_mut(T& value) noexcept -> OptionalRef<meta::PointedType<T>> {
        using OutRef = OptionalRef<meta::PointedType<T>>;

        if constexpr (not RAW and meta::IsPointer<T>) {
            return OutRef { std::addressof(*value) };
        } else if constexpr (not RAW and meta::IsContainedSemantics<T>) {
            return OutRef { &(value.operator*()) };
        } else {
            return OutRef { std::addressof(value) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<bool RAW, typename T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref_like(T& value) noexcept -> OptionalRef<meta::AddConstIf<meta::IsConst<T>, meta::PointedType<T>>> {
        using OutRef = OptionalRef<meta::AddConstIf<meta::IsConst<T>, meta::PointedType<T>>>;
        if constexpr (not RAW and meta::IsPointer<T>) {
            return OutRef { std::addressof(*value) };
        } else if constexpr (not RAW and meta::IsContainedSemantics<T>) {
            return OutRef { &(value.operator*()) };
        } else {
            return OutRef { std::addressof(value) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto unref(const Ref<T>& value) noexcept -> const T& {
        return *value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsNotConst T>
        STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto unref_mut(const Ref<T>& value) noexcept -> T& {
        return *value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { as_ref<RAW>(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_ref<RAW>(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_ref_mut<RAW>(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_ref_mut<RAW>(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, bool RAW, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(const T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_ref<RAW>(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, bool RAW, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_ref_mut<RAW>(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { as_opt_ref<RAW>(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_opt_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_opt_ref<RAW>(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_opt_ref_mut<RAW>(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, bool RAW, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_opt_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_opt_ref_mut<RAW>(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, bool RAW, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_opt_refs(const T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_opt_ref<RAW>(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, bool RAW, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_opt_refs(T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_opt_ref_mut<RAW>(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const Ref<T>& value) noexcept -> Ret {
        return hash<Ret>(value.get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, bool Optional, typename FormatContext>
    inline auto format_as(const Ref<T, Optional>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        if constexpr (Optional) {
            if (value == nullptr) return std::format_to(ctx.out(), "[ref value: null]");
            else
                return std::format_to(ctx.out(), "[ref value: {}]", *value);
        } else
            return std::format_to(ctx.out(), "[ref value: {}]", *value);
    }
}} // namespace stormkit::core
