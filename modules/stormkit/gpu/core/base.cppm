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

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    template<typename T>
    class Owned;

    namespace meta {
        template<typename T>
        struct ObjectInfo;

        template<typename T>
        concept HasRequiresInfo = requires(ObjectInfo<cmeta::CanonicalType<T>> value) { value; };

        template<typename T>
        concept CreateAllocateDisabled = HasRequiresInfo<T> and requires() {
            { ObjectInfo<cmeta::CanonicalType<T>>::DISABLE_CREATE_ALLOCATE } -> cmeta::IsBooleanTestable;
        } and ObjectInfo<cmeta::CanonicalType<T>>::DISABLE_CREATE_ALLOCATE;

        template<typename T>
        concept IsOwnedByOther = HasRequiresInfo<T> and requires(T) { typename ObjectInfo<cmeta::CanonicalType<T>>::OwnedBy; };

        template<typename T>
        concept IsOwned = HasRequiresInfo<T>
                          and std::derived_from<cmeta::CanonicalType<T>, Owned<typename ObjectInfo<cmeta::CanonicalType<T>>::Of>>;
    } // namespace meta

    template<typename T>
    class View;

    namespace meta {
        template<typename T>
        concept IsView = not IsOwned<cmeta::CanonicalType<T>> and requires(T value) {
            typename cmeta::CanonicalType<T>::ElementType;
            typename cmeta::CanonicalType<T>::ViewType;
            { value.native_handle() } -> cmeta::Is<typename cmeta::CanonicalType<T>::ElementType>;
        };

        template<typename T>
        concept IsOwnedOrView = IsOwned<T> or IsView<T>;

        template<typename T, typename... Args>
        concept DoInitReturnExpected = requires(T& foo, Args&&... args) {
            { foo.do_init(cmeta::CanonicalType<T>::PRIVATE, std::forward<Args>(args)...) } -> cmeta::SameAs<Expected<void>>;
        };
        template<typename T, typename... Args>
        concept DoInitReturnVoid = requires(T& foo, Args&&... args) {
            { foo.do_init(cmeta::CanonicalType<T>::PRIVATE, std::forward<Args>(args)...) } -> cmeta::SameAs<void>;
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
        template<cmeta::IsContainerOrPointerOf<T> U>
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

        template<typename Owner, typename... Args>
            requires(meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        [[nodiscard]]
        static auto create(Owner&& owner, Args&&... args) noexcept -> Expected<T>
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename Owner, typename... Args>
            requires(meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        [[nodiscard]]
        static auto create(Owner&& owner, Args&&... args) noexcept -> T
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename... Args>
            requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        [[nodiscard]]
        static auto create(Args&&... args) noexcept -> Expected<T>
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename... Args>
            requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        [[nodiscard]]
        static auto create(Args&&... args) noexcept -> T
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename Owner, typename... Args>
            requires(meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        [[nodiscard]]
        static auto allocate(Owner&& owner, Args&&... args) noexcept -> Expected<Heap<T>>
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename Owner, typename... Args>
            requires(meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        [[nodiscard]]
        static auto allocate(Owner&& owner, Args&&... args) noexcept -> Heap<T>
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename... Args>
            requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        [[nodiscard]]
        static auto allocate(Args&&... args) noexcept -> Expected<Heap<T>>
            requires(not meta::CreateAllocateDisabled<T>);

        template<typename... Args>
            requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        [[nodiscard]]
        static auto allocate(Args&&... args) noexcept -> Heap<T>
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
    auto as_view(T&& value) noexcept -> T;

    template<meta::IsOwned T>
    auto as_view(const T& value) noexcept -> typename meta::ObjectInfo<cmeta::CanonicalType<T>>::ViewType;

    template<cmeta::IsPointer T>
    auto as_view(const T& value) noexcept -> typename meta::ObjectInfo<cmeta::CanonicalType<cmeta::PointedType<T>>>::ViewType;

    template<cmeta::IsContainer T>
    auto as_view(const T& value) noexcept -> typename meta::ObjectInfo<cmeta::CanonicalType<cmeta::ContainedType<T>>>::ViewType;

    template<template<typename, std::size_t> class Out = std::array, typename... Args>
        requires(not stdr::range<Args> and ...)
    auto as_views(Args&&... args) noexcept -> decltype(auto);

    template<template<typename...> class Out = std::vector, typename... Args>
        requires(not stdr::range<Args> and ...)
    auto to_views(Args&&... args) noexcept -> decltype(auto);

    template<template<typename...> class Out = std::vector, stdr::range Range>
    auto to_views(const Range& range) noexcept -> decltype(auto);

    template<meta::IsOwned T, typename FormatContext>
    auto format_as(const T& object, FormatContext& ctx) noexcept -> decltype(ctx.out());
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
        : m_vk_handle { VK_NULL_HANDLE }, m_deleter_ptr { std::move(deleter_ptr) } {
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
    inline Owned<T>::Owned(Owned<T>&& other) noexcept
        : m_vk_handle { std::exchange(other.m_vk_handle, VK_NULL_HANDLE) },
          m_deleter_ptr { std::exchange(other.m_deleter_ptr, {}) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto Owned<T>::operator=(Owned&& other) noexcept -> Owned& {
        if (&other == this) [[unlikely]]
            return *this;

        m_vk_handle   = std::exchange(other.m_vk_handle, VK_NULL_HANDLE);
        m_deleter_ptr = std::exchange(other.m_deleter_ptr, {});

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
    template<typename Owner, typename... Args>
        requires(meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::create(Owner&& owner, Args&&... args) noexcept -> Expected<T>
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = T { PRIVATE, std::forward<Owner>(owner) };
        Try(out.do_init(PRIVATE, std::forward<Args>(args)...));
        Return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename Owner, typename... Args>
        requires(meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::create(Owner&& owner, Args&&... args) noexcept -> T
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = T { PRIVATE, std::forward<Owner>(owner) };
        out.do_init(PRIVATE, std::forward<Args>(args)...);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename... Args>
        requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::create(Args&&... args) noexcept -> Expected<T>
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = T { PRIVATE };
        Try(out.do_init(PRIVATE, std::forward<Args>(args)...));
        Return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename... Args>
        requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::create(Args&&... args) noexcept -> T
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = T { PRIVATE };
        out.do_init(PRIVATE, std::forward<Args>(args)...);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename Owner, typename... Args>
        requires(meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::allocate(Owner&& owner, Args&&... args) noexcept -> Expected<Heap<T>>
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = core::allocate_unsafe<T>(PRIVATE, std::forward<Owner>(owner));
        Try(out->do_init(PRIVATE, std::forward<Args>(args)...));
        Return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename Owner, typename... Args>
        requires(meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::allocate(Owner&& owner, Args&&... args) noexcept -> Heap<T>
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = core::allocate_unsafe<T>(PRIVATE, std::forward<Owner>(owner));
        out->do_init(PRIVATE, std::forward<Args>(args)...);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename... Args>
        requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnExpected<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::allocate(Args&&... args) noexcept -> Expected<Heap<T>>
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = core::allocate_unsafe<T>(PRIVATE);
        Try(out->do_init(PRIVATE, std::forward<Args>(args)...));
        Return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename... Args>
        requires(not meta::IsOwnedByOther<T> and meta::DoInitReturnVoid<T, Args...>)
        STORMKIT_FORCE_INLINE
    inline auto Owned<T>::allocate(Args&&... args) noexcept -> Heap<T>
        requires(not meta::CreateAllocateDisabled<T>)
    {
        auto out = core::allocate_unsafe<T>(PRIVATE);
        out->do_init(PRIVATE, std::forward<Args>(args)...);
        return out;
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
    template<cmeta::IsContainerOrPointerOf<T> U>
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
    inline auto as_view(T&& value) noexcept -> T {
        return std::forward<T>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsOwned T>
    STORMKIT_FORCE_INLINE
    inline auto as_view(const T& value) noexcept -> typename meta::ObjectInfo<cmeta::CanonicalType<T>>::ViewType {
        return typename meta::ObjectInfo<T>::ViewType { value };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<cmeta::IsPointer T>
    STORMKIT_FORCE_INLINE
    inline auto as_view(const T& value) noexcept ->
      typename meta::ObjectInfo<cmeta::CanonicalType<cmeta::PointedType<T>>>::ViewType {
        return as_view(unref(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<cmeta::IsContainer T>
    STORMKIT_FORCE_INLINE
    inline auto as_view(const T& value) noexcept ->
      typename meta::ObjectInfo<cmeta::CanonicalType<cmeta::ContainedType<T>>>::ViewType {
        return as_view(value.value());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename, std::size_t> class Out, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    inline auto as_views(Args&&... args) noexcept -> decltype(auto) {
        return Out { gpu::as_view(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out = std::vector, typename... Args>
        requires(not stdr::range<Args> and ...)
    STORMKIT_FORCE_INLINE
    inline auto to_views(Args&&... args) noexcept -> decltype(auto) {
        return Out { gpu::as_view(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<template<typename...> class Out, stdr::range Range>
    STORMKIT_FORCE_INLINE
    inline auto to_views(const Range& range) noexcept -> decltype(auto) {
        return range
               | stdv::transform([]<typename T>(T&& val) static noexcept { return gpu::as_view(std::forward<T>(val)); })
               | stdr::to<Out>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::IsOwned T, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const T& object, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return format_as(as_view(object), ctx);
    }
} // namespace stormkit::gpu
