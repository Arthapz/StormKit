// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.deferinit;

import std;

import stormkit.core.meta;
import stormkit.core.contract;
import stormkit.core.types;

export namespace stormkit { inline namespace core {
    template<typename T>
    using DeferInitDefaultStorage = array<std::byte, sizeof(T)>;

    template<typename T, typename Storage = DeferInitDefaultStorage<T>>
    class [[nodiscard]] DeferInit {
      public:
        using ValueType     = T;
        using PointerType   = ValueType*;
        using ReferenceType = ValueType&;

        // STL compatible
        using value_type = ValueType;
        using pointer    = PointerType;

        constexpr DeferInit();
        constexpr ~DeferInit();

        DeferInit(const DeferInit& other)                    = delete;
        auto operator=(const DeferInit& other) -> DeferInit& = delete;

        constexpr DeferInit(DeferInit&& other) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>));
        constexpr auto operator=(DeferInit&& other) noexcept(noexcept(std::is_nothrow_move_assignable_v<T>)) -> DeferInit&;

        template<typename... Args>
        constexpr auto construct(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<T, Args...>)) -> void;

        template<typename... Args>
        constexpr auto construct_with_narrowing(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<T, Args...>))
          -> void;

        constexpr auto operator=(T&& value) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>)) -> void;

        constexpr auto value(this auto&& self) noexcept -> decltype(auto);
        constexpr auto operator->(this auto& self) noexcept -> decltype(auto);
        constexpr auto operator*(this auto&& self) noexcept -> decltype(auto);

        constexpr operator T&() noexcept;
        constexpr operator const T&() const noexcept;

        constexpr explicit operator bool() const noexcept;
        constexpr auto     has_value() const noexcept -> bool;

      private:
        constexpr auto reset() noexcept -> void;

        alignas(T) Storage m_data;
        PointerType m_pointer = nullptr;
    };
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr DeferInit<T, Storage>::DeferInit() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr DeferInit<T, Storage>::~DeferInit() {
        reset();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr DeferInit<T, Storage>::DeferInit(DeferInit&& other) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>)) {
        reset();

        if (other.has_value()) [[likely]] {
            m_pointer = new (std::data(m_data)) T { std::move(other.value()) };

            other.reset();
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::operator=(DeferInit&& other) noexcept(noexcept(std::is_nothrow_move_assignable_v<T>))
      -> DeferInit& {
        if (&other == this) [[unlikely]]
            return *this;

        reset();

        if (other.has_value()) [[likely]] {
            m_pointer = new (std::data(m_data)) T { std::move(other.value()) };

            other.reset();
        }

        return *this;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T,
                             Storage>::construct(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<T, Args...>))
      -> void {
        reset();

        m_pointer = new (std::data(m_data)) T { std::forward<Args>(args)... };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::
      construct_with_narrowing(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<T, Args...>)) -> void {
        reset();

        m_pointer = new (std::data(m_data)) T(std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::operator=(T&& value) noexcept(noexcept(std::is_nothrow_move_constructible_v<T>))
      -> void {
        reset();

        m_pointer = new (std::data(m_data)) T { std::move(value) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr DeferInit<T, Storage>::operator bool() const noexcept {
        return has_value();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::has_value() const noexcept -> bool {
        return m_pointer != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::reset() noexcept -> void {
        if (m_pointer) [[likely]] {
            value().~T();
            m_pointer = nullptr;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::value(this auto&& self) noexcept -> decltype(auto) {
        expects(self.has_value(), "Underlying object is not has_value");

        return std::forward_like<decltype(self)>(*self.m_pointer);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::operator->(this auto& self) noexcept -> decltype(auto) {
        expects(self.has_value(), "Underlying object is not has_value");

        return std::forward_like<decltype(self)>(self.m_pointer);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr auto DeferInit<T, Storage>::operator*(this auto&& self) noexcept -> decltype(auto) {
        return std::forward_like<decltype(self)>(self.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr DeferInit<T, Storage>::operator T&() noexcept {
        return value();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Storage>
    STORMKIT_FORCE_INLINE
    constexpr DeferInit<T, Storage>::operator const T&() const noexcept {
        return value();
    }
}} // namespace stormkit::core

static_assert(stormkit::meta::IsContainer<stormkit::DeferInit<int>>);
