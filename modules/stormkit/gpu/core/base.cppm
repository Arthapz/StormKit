// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.core:base;

import std;

import stormkit.core;

import :structs;
import :vulkan;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    template<typename T>
    class Owned;

    namespace meta {
        template<typename T>
        struct ObjectInfo;

        template<typename T>
        concept HasRequiresInfo = requires(ObjectInfo<T> value) { value; };

        template<typename T>
        concept CreateAllocateDisabled = HasRequiresInfo<T> and requires() {
            { ObjectInfo<T>::DISABLE_CREATE_ALLOCATE } -> cmeta::IsBooleanTestable;
        } and ObjectInfo<T>::DISABLE_CREATE_ALLOCATE;

        template<typename T>
        concept IsOwnedByOther = HasRequiresInfo<T> and requires(T) { typename ObjectInfo<T>::OwnedBy; };

        template<typename T>
        concept IsOwned = HasRequiresInfo<T>;
    } // namespace meta

    template<typename T>
    class View;

    namespace meta {
        template<typename T>
        concept IsView = not IsOwned<T> and requires(const T& value) {
            typename T::ElementType;
            typename T::ViewType;
            { value.native_handle() } -> cmeta::Is<typename T::ElementType>;
        };

        template<typename T>
        concept IsOwnedOrView = IsOwned<T> or IsView<T>;

        template<typename T, typename... Args>
        concept DoInitReturnExpected = requires(T& foo, Args&&... args) {
            { foo.do_init(T::PRIVATE, std::forward<Args>(args)...) } -> cmeta::SameAs<Expected<void>>;
        };
        template<typename T, typename... Args>
        concept DoInitReturnVoid = requires(T& foo, Args&&... args) {
            { foo.do_init(T::PRIVATE, std::forward<Args>(args)...) } -> cmeta::SameAs<void>;
        };
    } // namespace meta

    STORMKIT_GPU_API
    auto initialize_backend() -> Expected<void>;

    template<typename T>
    class View {
      public:
        using ObjectInfo  = typename meta::ObjectInfo<T>;
        using ElementType = ObjectInfo::ElementType;
        using ViewType    = ObjectInfo::ViewType;

        View(const T& of) noexcept;
        template<cmeta::ContainedOrPointerOf<T> U>
        View(const U& of) noexcept;
        ~View() noexcept;

        View(const View&) noexcept;
        auto operator=(const View&) noexcept -> View&;

        View(View&&) noexcept;
        auto operator=(View&&) noexcept -> View&;

        [[nodiscard]]
        auto native_handle() const noexcept -> ElementType;

        operator ElementType() const noexcept;

      protected:
        ElementType m_vk_handle;
    };

    template<typename T>
    class Owned {
      public:
        using ObjectInfo  = typename meta::ObjectInfo<T>;
        using ElementType = ObjectInfo::ElementType;
        using DeleterType = ObjectInfo::DeleterType;
        using ViewType    = ObjectInfo::ViewType;
        ~Owned() noexcept;

        Owned(const Owned&)                             = delete;
        auto operator=(const Owned&) noexcept -> Owned& = delete;

        Owned(Owned&&) noexcept;
        auto operator=(Owned&&) noexcept -> Owned&;

        template<typename... Args>
        [[nodiscard]]
        static auto create(Args&&... args) noexcept -> decltype(auto)
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename... Args>
        [[nodiscard]]
        static auto allocate(Args&&... args) noexcept -> decltype(auto)
            requires(not meta::CreateAllocateDisabled<T>);

        [[nodiscard]]
        auto native_handle() const noexcept -> ElementType;

        operator ElementType() const noexcept;

      protected:
        static constexpr struct PrivateTag {
        } PRIVATE;

        explicit Owned(DeleterType&& deleter_ptr) noexcept;

        ElementType m_vk_handle;
        DeleterType m_deleter_ptr;
    };

    template<meta::IsView T>
    auto to_view(T value) noexcept -> T;

    template<meta::IsOwned T>
    auto to_view(const T& value) noexcept -> typename T::ViewType;

    template<meta::IsOwnedOrView T, cmeta::ContainedOrPointerOf<T> U>
    auto to_view(const U& value) noexcept -> typename T::ViewType;

    template<template<typename, std::size_t> class Out = std::array, typename... Args>
    auto to_views(const Args&... args) noexcept -> decltype(auto);

    template<template<typename...> class Out = std::vector, typename... Args>
    auto to_views(const Args&... args) noexcept -> decltype(auto);
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline Owned<T>::Owned(DeleterType&& deleter_ptr) noexcept
        : m_deleter_ptr { std::move(deleter_ptr) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline Owned<T>::~Owned() noexcept {
        if constexpr (cmeta::SameAs<DeleterType, void (*)(ElementType, const VkAllocationCallbacks*)>) {
            if (m_deleter_ptr != nullptr and m_vk_handle != VK_NULL_HANDLE) vk::call(m_deleter_ptr, m_vk_handle, nullptr);
            m_vk_handle = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline Owned<T>::Owned(Owned&& other) noexcept
        : m_vk_handle { std::exchange(other.m_vk_handle, VK_NULL_HANDLE) }, m_deleter_ptr { std::move(other.m_deleter_ptr) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto Owned<T>::operator=(Owned&& other) noexcept -> Owned& {
        if (&other == this) [[unlikely]]
            return *this;

        m_vk_handle   = std::exchange(other.m_vk_handle, VK_NULL_HANDLE);
        m_deleter_ptr = std::move(other.m_deleter_ptr);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto Owned<T>::native_handle() const noexcept -> ElementType {
        EXPECTS(m_vk_handle != VK_NULL_HANDLE);
        return m_vk_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline Owned<T>::operator ElementType() const noexcept {
        return native_handle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename... Args>
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::create(Args&&... args) noexcept -> decltype(auto)
        requires(not meta::CreateAllocateDisabled<T>)
    {
        if constexpr (meta::IsOwnedByOther<T>) {
            auto out = T { PRIVATE, std::forward<Args...[0]>(args...[0]) };
            return []<typename... Args2>(auto&& out, auto&&, Args2&&... args2) static noexcept -> decltype(auto) {
                if constexpr (meta::DoInitReturnExpected<T, Args2...>) {
                    auto out_expected = Expected<T> { std::in_place, std::move(out) };
                    if (auto result = out.do_init(PRIVATE, std::forward<Args2>(args2)...); not result)
                        out_expected = std::unexpected { std::move(result).error() };

                    return out_expected;
                } else if constexpr (meta::DoInitReturnVoid<T, Args2...>) {
                    out.do_init(PRIVATE, std::forward<Args2>(args2)...);
                    return out;
                }
            }(std::move(out), std::forward<Args>(args)...);
        } else {
            auto out = T { PRIVATE };
            if constexpr (meta::DoInitReturnExpected<T, Args...>) {
                auto out_expected = Expected<T> { std::in_place, std::move(out) };
                if (auto result = out.do_init(PRIVATE, std::forward<Args>(args)...); not result)
                    out_expected = std::unexpected { std::move(result).error() };

                return out_expected;
            } else if constexpr (meta::DoInitReturnVoid<T, Args...>) {
                out.do_init(PRIVATE, std::forward<Args>(args)...);
                return out;
            }
        }

        std::unreachable();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename... Args>
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::allocate(Args&&... args) noexcept -> decltype(auto)
        requires(not meta::CreateAllocateDisabled<T>)
    {
        if constexpr (meta::IsOwnedByOther<T>) {
            auto out = core::allocate_unsafe<T>(PRIVATE, std::forward<Args...[0]>(args...[0]));
            return []<typename... Args2>(auto&& out, auto&&, Args2&&... args2) static noexcept -> Expected<Heap<T>> {
                if constexpr (meta::DoInitReturnExpected<T, Args2...>) {
                    auto out_expected = Expected<Heap<T>> { std::in_place, std::move(out) };
                    if (auto result = out->do_init(PRIVATE, std::forward<Args2>(args2)...); not result)
                        out_expected = std::unexpected { std::move(result).error() };

                    return out_expected;
                } else if constexpr (meta::DoInitReturnVoid<T, Args2...>) {
                    out->do_init(PRIVATE, std::forward<Args2>(args2)...);
                    return out;
                }
            }(std::move(out), std::forward<Args>(args)...);
        } else {
            auto out = core::allocate_unsafe(PRIVATE);
            if constexpr (meta::DoInitReturnExpected<T, Args...>) {
                auto out_expected = Expected<Heap<T>> { std::in_place, std::move(out) };
                if (auto result = out.do_init(PRIVATE, std::forward<Args>(args)...); not result)
                    out_expected = std::unexpected { std::move(result).error() };

                return out_expected;
            } else if constexpr (meta::DoInitReturnVoid<T, Args...>) {
                out->do_init(PRIVATE, std::forward<Args>(args)...);
                return out;
            }
        }

        std::unreachable();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline View<T>::View(const T& of) noexcept
        : m_vk_handle { of.native_handle() } {
        ENSURES(m_vk_handle != VK_NULL_HANDLE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<cmeta::ContainedOrPointerOf<T> U>
    STORMKIT_FORCE_INLINE
    inline View<T>::View(const U& object) noexcept
        : m_vk_handle { object->native_handle() } {
        ENSURES(m_vk_handle != VK_NULL_HANDLE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline View<T>::~View() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline View<T>::View(const View&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto View<T>::operator=(const View&) noexcept -> View& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline View<T>::View(View&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto View<T>::operator=(View&&) noexcept -> View& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto View<T>::native_handle() const noexcept -> ElementType {
        EXPECTS(m_vk_handle != VK_NULL_HANDLE);
        return m_vk_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline View<T>::operator ElementType() const noexcept {
        return native_handle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsView T>
    STORMKIT_FORCE_INLINE
    inline auto to_view(T&& value) noexcept -> T {
        return std::forward<T>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsOwned T>
    STORMKIT_FORCE_INLINE
    inline auto to_view(const T& value) noexcept -> typename T::ViewType {
        return typename T::ViewType { value };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsOwnedOrView T, cmeta::ContainedOrPointerOf<T> U>
    STORMKIT_FORCE_INLINE
    inline auto to_view(const U& value) noexcept -> typename T::ViewType {
        return to_view(*value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> class Out = std::array, typename... Args>
    STORMKIT_FORCE_INLINE
    inline auto to_views(Args&&... args) noexcept -> decltype(auto) {
        return Out { to_view(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out = std::vector, typename... Args>
    STORMKIT_FORCE_INLINE
    inline auto to_views(const Args&... args) noexcept -> decltype(auto) {
        return Out { to_view(std::forward<Args>(args))... };
    }
} // namespace stormkit::gpu
