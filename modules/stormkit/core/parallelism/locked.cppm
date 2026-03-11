// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:parallelism.locked;

import std;

import :meta;
import :typesafe.integer;
import :typesafe.ref;

namespace stormkit { inline namespace core { namespace details {
    using DefaultMutex = std::mutex;
    template<typename Mutex>
    using DefaultReadOnlyLock = std::unique_lock<Mutex>;
    template<typename Mutex>
    using DefaultReadWriteLock = std::unique_lock<Mutex>;
}}} // namespace stormkit::core::details

export namespace stormkit { inline namespace core {
    enum class LockAccessMode : core::u8 {
        READ_ONLY,
        READ_WRITE,
    };

    template<meta::IsNotRawIndirection T, class Mutex = details::DefaultMutex>
    class STORMKIT_CORE_API Locked {
      public:
        using ValueType          = meta::PointedType<T>;
        using ReferenceType      = ValueType&;
        using ConstReferenceType = const ValueType&;
        using PointerType        = ValueType*;

        /* stl compatible */
        using value_type     = ValueType;
        using reference_type = ReferenceType;
        using pointer_type   = PointerType;

        using MutexType = Mutex;

      private:
        template<template<class> class Lock, LockAccessMode Mode>
        class Access;

      public:
        template<template<class> class Lock>
        using ReadAccess = Access<Lock, LockAccessMode::READ_ONLY>;
        template<template<class> class Lock>
        using WriteAccess = Access<Lock, LockAccessMode::READ_WRITE>;

        Locked() noexcept(noexcept(std::is_nothrow_default_constructible_v<ValueType>));

        template<typename... Args>
        explicit(sizeof...(Args) == 1)
          Locked(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<ValueType, Args...>));

        Locked(const Locked&)                    = delete;
        auto operator=(const Locked&) -> Locked& = delete;

        Locked(Locked&&) noexcept;
        auto operator=(Locked&&) noexcept -> Locked&;

        ~Locked() noexcept;

        template<LockAccessMode Mode, template<class> class Lock, typename... LockArgs, class Self>
        auto access(this Self& self, LockArgs&&... lock_args) noexcept -> Access<Lock, Mode>;

        template<template<class> class Lock = details::DefaultReadOnlyLock, typename... LockArgs>
        auto read(LockArgs&&... lock_args) const noexcept -> ReadAccess<Lock>;

        template<template<class> class Lock = details::DefaultReadWriteLock, typename... LockArgs>
        auto write(LockArgs&&... lock_args) noexcept -> WriteAccess<Lock>;

        template<template<class> class Lock = details::DefaultReadOnlyLock, typename... LockArgs>
        auto copy(LockArgs&&... lock_args) const noexcept -> ValueType;

        template<template<class> class Lock = details::DefaultReadWriteLock, typename... LockArgs>
        auto assign(ConstReferenceType value,
                    LockArgs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<ValueType, ConstReferenceType>))
          -> void;

        template<template<class> class Lock = details::DefaultReadWriteLock, typename... LockArgs>
        auto assign(ValueType&& value,
                    LockArgs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<ValueType, ValueType&&>)) -> void;

        template<typename Self>
        auto unsafe(this Self& self) noexcept -> meta::ForwardConst<Self, T>&;

        auto mutex() const noexcept -> const MutexType&;

      private:
        template<template<class> class Lock, LockAccessMode Mode>
        class Access {
          public:
            using AccessValueType  = std::conditional_t<Mode == LockAccessMode::READ_ONLY, const ValueType, ValueType>;
            using RefContainerType = std::conditional_t<Mode == LockAccessMode::READ_ONLY, Ref<const ValueType>, Ref<ValueType>>;

            template<typename... LockArgs>
            Access(ReferenceType value, MutexType& mutex, LockArgs&&... args) noexcept;

            template<typename... LockArgs>
            Access(ConstReferenceType value, MutexType& mutex, LockArgs&&... args) noexcept;

            template<typename... LockArgs>
            explicit(sizeof...(LockArgs) == 0) Access(const Locked& locked, LockArgs&&... args) noexcept
                requires(Mode == LockAccessMode::READ_ONLY);

            template<typename... LockArgs>
            explicit(sizeof...(LockArgs) == 0) Access(Locked& locked, LockArgs&&... args) noexcept;

            auto operator->() const noexcept -> AccessValueType*;
            auto operator*() const noexcept -> AccessValueType&;

            mutable Lock<MutexType> lock;

          private:
            RefContainerType m_value;
        };

        mutable Mutex m_mutex;
        T m_value     STORMKIT_GUARDED_BY(m_mutex);
    };

    template<typename T>
    Locked(T) -> Locked<T>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Locked() noexcept(noexcept(std::is_nothrow_default_constructible_v<ValueType>)) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Locked(Args&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<ValueType, Args...>))
        : m_value { std::forward<Args>(args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    Locked<T, Mutex>::Locked(Locked&& other) noexcept {
        auto from = other.write();

        m_value = std::move(*from);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    auto Locked<T, Mutex>::operator=(Locked&& other) noexcept -> Locked& {
        if (&other == this) [[unlikely]]
            return *this;

        auto from = other.write();
        auto to   = write();

        *to = std::move(*from);

        return *this;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    Locked<T, Mutex>::~Locked() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<LockAccessMode Mode, template<class> class Lock, typename... LockArgs, class Self>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::access(this Self& self, LockArgs&&... lock_args) noexcept -> Access<Lock, Mode> {
        static_assert(not(Mode == LockAccessMode::READ_ONLY and not std::is_const_v<meta::RemoveIndirectionsType<Self>>),
                      "can't get read access on const Locked<T>");
        using AccessType = Access<Lock, Mode>;
        return AccessType { std::forward<Self&>(self), std::forward<LockArgs>(lock_args)... };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockArgs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::read(LockArgs&&... lock_args) const noexcept -> ReadAccess<Lock> {
        return access<LockAccessMode::READ_ONLY, Lock>(std::forward<LockArgs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockArgs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::write(LockArgs&&... lock_args) noexcept -> WriteAccess<Lock> {
        return access<LockAccessMode::READ_WRITE, Lock>(std::forward<LockArgs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockArgs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::copy(LockArgs&&... lock_args) const noexcept -> ValueType {
        return *read(std::forward<LockArgs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockArgs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::assign(ConstReferenceType value,
                                  LockArgs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<ValueType,
                                                                                                          ConstReferenceType>))
      -> void {
        *write(std::forward<LockArgs>(lock_args)...) = value;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockArgs>
    STORMKIT_FORCE_INLINE
    auto Locked<T,
                Mutex>::assign(ValueType&& value,
                               LockArgs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<ValueType, ValueType&&>))
      -> void {
        *write(std::forward<LockArgs>(lock_args)...) = std::move(value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::unsafe(this Self& self) noexcept -> meta::ForwardConst<Self, T>& {
        return std::forward_like<Self&>(self.m_value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::mutex() const noexcept -> const MutexType& {
        return m_mutex;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockArgs>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(ReferenceType value, MutexType& mutex, LockArgs&&... lock_args) noexcept
        : lock { mutex, std::forward<LockArgs>(lock_args)... }, m_value { as_ref_mut(value) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockArgs>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(ConstReferenceType value, MutexType& mutex, LockArgs&&... lock_args) noexcept
        : lock { mutex, std::forward<LockArgs>(lock_args)... }, m_value { as_ref(value) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockArgs>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(const Locked& locked, LockArgs&&... lock_args) noexcept
        requires(Mode == LockAccessMode::READ_ONLY)
        : Access { locked.m_value, locked.m_mutex, std::forward<LockArgs>(lock_args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockArgs>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(Locked& locked, LockArgs&&... lock_args) noexcept
        : Access { locked.m_value, locked.m_mutex, std::forward<LockArgs>(lock_args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::Access<Lock, Mode>::operator->() const noexcept -> AccessValueType* {
        return m_value.get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::Access<Lock, Mode>::operator*() const noexcept -> AccessValueType& {
        return *m_value;
    }
}} // namespace stormkit::core
