// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:containers.raii_capsule;

import std;

import :meta;

import :utils.tags;
import :functional.monadic;

export namespace stormkit { inline namespace core {
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE = T {}>
    class RAIICapsule {
      public:
        using ValueType     = T;
        using ReferenceType = ValueType&;

        using value_type = ValueType;
        using reference  = ReferenceType;

        template<typename... Args>
        static constexpr auto create(Args&&... args) noexcept -> RAIICapsule
            requires meta::Is<std::invoke_result_t<decltype(Constructor), Args...>, ValueType>;
        template<typename... Args>
        static constexpr auto create(Args&&... args) noexcept -> decltype(auto)
            requires meta::IsSpecializationOf<std::invoke_result_t<decltype(Constructor), Args...>, std::expected>;

        static constexpr auto take(ValueType&& value) noexcept -> RAIICapsule;
        static constexpr auto empty() noexcept -> RAIICapsule;

        constexpr ~RAIICapsule() noexcept;

        constexpr RAIICapsule(RAIICapsule& other)                    = delete;
        constexpr auto operator=(RAIICapsule& other) -> RAIICapsule& = delete;

        constexpr RAIICapsule(RAIICapsule&& other) noexcept;
        constexpr auto operator=(RAIICapsule&& other) noexcept -> RAIICapsule&;

        constexpr      operator ValueType() const noexcept;
        constexpr auto handle() noexcept -> ReferenceType;
        constexpr auto handle() const noexcept -> ValueType;
        constexpr auto release() noexcept -> ValueType;
        constexpr auto reset(T handle = RELEASE_VALUE) noexcept -> void;

      private:
        constexpr RAIICapsule() noexcept;
        constexpr RAIICapsule(ValueType handle) noexcept;

        constexpr auto destroy() noexcept -> void;

        ValueType m_handle = RELEASE_VALUE;
    };
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////
namespace stormkit { inline namespace core {

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    template<typename... Args>
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::create(Args&&... args) noexcept -> RAIICapsule
        requires meta::Is<std::invoke_result_t<decltype(Constructor), Args...>, ValueType>
    {
        return RAIICapsule { Constructor(std::forward<Args>(args)...) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    template<typename... Args>
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::create(Args&&... args) noexcept -> decltype(auto)
        requires meta::IsSpecializationOf<std::invoke_result_t<decltype(Constructor), Args...>, std::expected>
    {
        return Constructor(std::forward<Args>(args)...).transform(monadic::init<RAIICapsule>());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::take(ValueType&& value) noexcept -> RAIICapsule {
        return RAIICapsule { std::move(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::empty() noexcept -> RAIICapsule {
        return RAIICapsule { RELEASE_VALUE };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::~RAIICapsule() noexcept {
        destroy();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::RAIICapsule(RAIICapsule&& other) noexcept {
        m_handle = other.release();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::operator=(RAIICapsule&& other) noexcept
      -> RAIICapsule& {
        m_handle = other.release();
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::operator ValueType() const noexcept {
        return m_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::handle() noexcept -> ReferenceType {
        return m_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::handle() const noexcept -> ValueType {
        return m_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_PURE STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::release() noexcept -> ValueType {
        auto tmp = std::exchange(m_handle, RELEASE_VALUE);
        return tmp;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::reset(ValueType handle) noexcept -> void {
        destroy();
        m_handle = handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr auto RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::destroy() noexcept -> void {
        if (m_handle != RELEASE_VALUE) { Deleter(release()); }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::RAIICapsule(ValueType handle) noexcept {
        m_handle = handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, auto Constructor, auto Deleter, typename Tag, auto RELEASE_VALUE>
    STORMKIT_FORCE_INLINE
    constexpr RAIICapsule<T, Constructor, Deleter, Tag, RELEASE_VALUE>::RAIICapsule() noexcept = default;
}} // namespace stormkit::core
