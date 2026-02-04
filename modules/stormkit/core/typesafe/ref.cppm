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
        template<typename T>
        using ptr = T*;

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

            template<meta::IsRawPointerOrLValueReference U>
            friend constexpr auto as_ref(U&& value) noexcept -> decltype(auto);

            template<meta::IsRawPointerOrLValueReference U>
            friend constexpr auto as_ref_mut(U&& value) noexcept -> decltype(auto);

            template<meta::IsRawPointerOrLValueReference U>
            friend constexpr auto as_ref_like(U&& value) noexcept -> decltype(auto);

            template<meta::IsRawPointerOrLValueReference U>
            friend constexpr auto as_opt_ref(U&& value) noexcept -> decltype(auto);

            template<meta::IsRawPointerOrLValueReference U>
            friend constexpr auto as_opt_ref_mut(U&& value) noexcept -> decltype(auto);

            template<meta::IsRawPointerOrLValueReference U>
            friend constexpr auto as_opt_ref_like(U&& value) noexcept -> decltype(auto);
        };

        template<typename T>
        using OptionalRef = Ref<T, true>;

        template<meta::IsRawPointerOrLValueReference T>
        [[nodiscard]]
        constexpr auto as_ref(T&& value STORMKIT_LIFETIMEBOUND) noexcept -> decltype(auto);

        template<meta::IsRawPointerOrLValueReference T>
        [[nodiscard]]
        constexpr auto as_ref_mut(T&& value STORMKIT_LIFETIMEBOUND) noexcept -> decltype(auto);

        template<meta::IsRawPointerOrLValueReference T>
        [[nodiscard]]
        constexpr auto as_ref_like(T&& value STORMKIT_LIFETIMEBOUND) noexcept -> decltype(auto);

        template<meta::IsRawPointerOrLValueReference T>
        [[nodiscard]]
        constexpr auto as_opt_ref(T&& value STORMKIT_LIFETIMEBOUND) noexcept -> decltype(auto);

        template<meta::IsRawPointerOrLValueReference T>
        [[nodiscard]]
        constexpr auto as_opt_ref_mut(T&& value STORMKIT_LIFETIMEBOUND) noexcept -> decltype(auto);

        template<meta::IsRawPointerOrLValueReference T>
        [[nodiscard]]
        constexpr auto as_opt_ref_like(T&& value STORMKIT_LIFETIMEBOUND) noexcept -> decltype(auto);

        template<typename T>
        [[nodiscard]]
        constexpr auto unref(const Ref<T>& value STORMKIT_LIFETIMEBOUND) noexcept -> const T&;

        template<meta::IsNotConst T>
        [[nodiscard]]
        constexpr auto unref_mut(const Ref<T>& value STORMKIT_LIFETIMEBOUND) noexcept -> T&;

        template<template<typename, std::size_t> typename Out = std::array, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = std::array, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_refs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto);
        template<template<typename, std::size_t> typename Out = std::array, typename... Args>
            requires(not std::ranges::range<Args> and ...)

        [[nodiscard]]
        constexpr auto as_opt_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_opt_refs(Args&&... args) noexcept -> decltype(auto);

        template<template<typename, std::size_t> typename Out = std::array, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto as_opt_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> typename Out = std::vector, typename... Args>
            requires(not std::ranges::range<Args> and ...)
        [[nodiscard]]
        constexpr auto to_opt_ref_muts(Args&&... args) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, std::ranges::range T>
            requires(std::ranges::range<Out<typename T::value_type>>)
        [[nodiscard]]
        constexpr auto to_opt_refs(const T& range) noexcept -> decltype(auto);

        template<template<typename...> class Out = std::vector, std::ranges::range T>
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
        if constexpr (meta::IsStrict<T, U>)
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
    template<meta::IsRawPointerOrLValueReference T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref(T&& value) noexcept -> decltype(auto) {
        using TValue = meta::CanonicalType<T>;
        if constexpr (meta::IsPointer<TValue>) {
            EXPECTS(value != nullptr);
            return Ref<const meta::ElementType<TValue>> { std::to_address(value) };
        } else if constexpr (meta::IsContainedSemantics<TValue>) {
            EXPECTS(value.operator bool());
            return Ref<const meta::UnderlyingType<TValue>> { &(value.operator*()) };
        } else {
            return Ref<const TValue> { &value };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    // template<typename T>
    template<meta::IsRawPointerOrLValueReference T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_mut(T&& value) noexcept -> decltype(auto) {
        using TValue                    = std::remove_reference_t<T>;
        static constexpr auto error_msg = "as_ref_mut can't take a reference of a const object"sv;
        if constexpr (meta::IsPointer<TValue>) {
            using PointedType = meta::ElementType<TValue>;
            static_assert(meta::IsNotConst<PointedType>, error_msg);
            EXPECTS(value != nullptr);
            return Ref<meta::ElementType<TValue>> { std::to_address(value) };
        } else if constexpr (meta::IsContainedSemantics<TValue>) {
            using UnderlyingType = meta::UnderlyingType<TValue>;
            static_assert(meta::IsNotConst<UnderlyingType>, error_msg);
            EXPECTS(value.operator bool());
            return Ref<UnderlyingType> { &(value.operator*()) };
        } else {
            static_assert(meta::IsNotConst<TValue>, error_msg);
            return Ref<TValue> { &value };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    // template<typename T>
    template<meta::IsRawPointerOrLValueReference T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_like(T&& value) noexcept -> decltype(auto) {
        using TValue = meta::CanonicalType<T>;
        if constexpr (meta::IsPointer<TValue>) {
            EXPECTS(value != nullptr);
            return Ref<meta::ElementType<TValue>> { std::to_address(value) };
        } else if constexpr (meta::IsContainedSemantics<T>) {
            EXPECTS(value.operator bool());
            return Ref<meta::UnderlyingType<TValue>> { &(value.operator*()) };
        } else {
            return Ref<TValue> { &value };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    // template<typename T>
    template<meta::IsRawPointerOrLValueReference T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref(T&& value) noexcept -> decltype(auto) {
        using TValue = meta::CanonicalType<T>;
        if constexpr (meta::IsPointer<TValue>) {
            return OptionalRef<const meta::ElementType<TValue>> { std::to_address(value) };
        } else if constexpr (meta::IsContainedSemantics<TValue>) {
            return OptionalRef<const meta::UnderlyingType<TValue>> { &(value.operator*()) };
        } else {
            return OptionalRef<const TValue> { &value };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    // template<typename T>
    template<meta::IsRawPointerOrLValueReference T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref_mut(T&& value) noexcept -> decltype(auto) {
        using TValue                    = std::remove_reference_t<T>;
        static constexpr auto error_msg = "as_ref_mut can't take a reference of a const object"sv;
        if constexpr (meta::IsPointer<TValue>) {
            using PointedType = meta::ElementType<TValue>;
            static_assert(meta::IsNotConst<PointedType>, error_msg);
            return OptionalRef<meta::ElementType<TValue>> { std::to_address(value) };
        } else if constexpr (meta::IsContainedSemantics<TValue>) {
            using UnderlyingType = meta::UnderlyingType<TValue>;
            static_assert(meta::IsNotConst<UnderlyingType>, error_msg);
            return OptionalRef<UnderlyingType> { &(value.operator*()) };
        } else {
            static_assert(meta::IsNotConst<TValue>, error_msg);
            return OptionalRef<TValue> { &value };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    // template<typename T>
    template<meta::IsRawPointerOrLValueReference T>
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref_like(T&& value) noexcept -> decltype(auto) {
        using TValue = meta::CanonicalType<T>;
        if constexpr (meta::IsPointer<TValue>) {
            return OptionalRef<meta::ElementType<TValue>> { std::to_address(value) };
        } else if constexpr (meta::IsContainedSemantics<T>) {
            return OptionalRef<meta::UnderlyingType<TValue>> { &(value.get()) };
        } else {
            return OptionalRef<TValue> { &value };
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
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { as_ref(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_ref(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_ref_mut(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_ref_mut(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_refs(const T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_ref(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_refs(T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_ref_mut(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { as_opt_ref(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_opt_refs(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_opt_ref(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto as_opt_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_opt_ref_mut(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> typename Out, typename... Args>
        requires(not std::ranges::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    constexpr auto to_opt_ref_muts(Args&&... args) noexcept -> decltype(auto) {
        return Out { { as_opt_ref_mut(std::forward<Args>(args))... } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_opt_refs(const T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_opt_ref(std::forward<U>(val));
                 })
               | std::ranges::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, std::ranges::range T>
        requires(std::ranges::range<Out<typename T::value_type>>)
    STORMKIT_FORCE_INLINE
    constexpr auto to_mut_opt_refs(T& range) noexcept -> decltype(auto) {
        return range
               | std::views::transform([]<class U>(U&& val) static noexcept -> decltype(auto) {
                     return as_opt_ref_mut(std::forward<U>(val));
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
