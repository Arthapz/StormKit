// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.parallelism.locked;

import std;

import stormkit.core.meta;
import stormkit.core.types;
import stormkit.core.typesafe;

namespace stormkit { inline namespace core {
    export {
        enum class LockAccessMode : u8 {
            READ_ONLY,
            READ_WRITE,
        };
    }

    namespace meta { namespace details {
        template<typename T>
        struct Lockedvalue_type {
            using type = T;
        };

        template<meta::IsContainerOrPointer T>
        struct Lockedvalue_type<T> {
            using type = meta::ContainedOrpointed_type<T>;
        };

        template<typename ReferenceType, LockAccessMode Mode>
        using AccessClosureInvokeParameter = meta::lazy_conditional<Mode == LockAccessMode::READ_ONLY, const ReferenceType, ReferenceType>;
    }} // namespace meta::details

    namespace details {
        using DefaultMutex = std::mutex;
        template<typename Mutex>
        using DefaultReadOnlyLock = std::unique_lock<Mutex>;
        template<typename Mutex>
        using DefaultReadWriteLock = std::unique_lock<Mutex>;
    } // namespace details

    export {
        template<meta::IsNotRawIndirection T, class Mutex = details::DefaultMutex>
        class STORMKIT_CORE_API Locked {
          public:
            using value_type          = meta::details::Lockedvalue_type<T>::type;
            using ReferenceType      = value_type&;
            using ConstReferenceType = const value_type&;
            using pointer_type        = value_type*;

            /* stl compatible */
            using value_type     = value_type;
            using reference_type = ReferenceType;
            using pointer_type   = pointer_type;

            using MutexType = Mutex;

          private:
            template<template<class> class Lock, LockAccessMode Mode>
            class Access;

          public:
            template<template<class> class Lock>
            using ReadAccess = Access<Lock, LockAccessMode::READ_ONLY>;
            template<template<class> class Lock>
            using WriteAccess = Access<Lock, LockAccessMode::READ_WRITE>;

            Locked() noexcept(noexcept(std::is_nothrow_default_constructible_v<value_type>));

            template<typename... Ts>
            explicit(sizeof...(Ts) == 1)
              Locked(Ts&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<value_type, Ts...>));

            Locked(const Locked&)                    = delete;
            auto operator=(const Locked&) -> Locked& = delete;

            Locked(Locked&&) noexcept;
            auto operator=(Locked&&) noexcept -> Locked&;

            ~Locked() noexcept;

            template<LockAccessMode Mode, template<class> class Lock, typename... LockTs, class Self>
            auto access(this Self& self, LockTs&&... lock_args) noexcept -> Access<Lock, Mode>;

            template<
              LockAccessMode Mode,
              std::invocable<meta::details::AccessClosureInvokeParameter<typename meta::details::Lockedvalue_type<T>::type&, Mode>>
                Closure,
              template<class> class Lock,
              typename... LockTs,
              class Self>
            auto access(this Self& self, Closure&& closure, LockTs&&... lock_args) noexcept
              -> std::invoke_result_t<Closure, meta::details::AccessClosureInvokeParameter<ReferenceType, Mode>>;

            template<template<class> class Lock = details::DefaultReadOnlyLock, typename... LockTs>
            auto read(LockTs&&... lock_args) const noexcept -> ReadAccess<Lock>;

            template<std::invocable<typename Locked<T, Mutex>::ConstReferenceType> Closure,
                     template<class> class Lock = details::DefaultReadOnlyLock,
                     typename... LockTs>
            auto read(Closure&& closure, LockTs&&... lock_args) const noexcept
              -> std::invoke_result_t<Closure, ConstReferenceType>;

            template<template<class> class Lock = details::DefaultReadWriteLock, typename... LockTs>
            auto write(LockTs&&... lock_args) noexcept -> WriteAccess<Lock>;

            template<std::invocable<typename Locked<T, Mutex>::ReferenceType> Closure,
                     template<class> class Lock = details::DefaultReadWriteLock,
                     typename... LockTs>
            auto write(Closure&& closure, LockTs&&... lock_args) noexcept -> std::invoke_result_t<Closure, ReferenceType>;

            template<template<class> class Lock = details::DefaultReadOnlyLock, typename... LockTs>
            auto copy(LockTs&&... lock_args) const noexcept -> value_type;

            template<template<class> class Lock = details::DefaultReadWriteLock, typename... LockTs>
            auto assign(ConstReferenceType value,
                        LockTs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<value_type, ConstReferenceType>))
              -> void;

            template<template<class> class Lock = details::DefaultReadWriteLock, typename... LockTs>
            auto assign(value_type&& value,
                        LockTs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<value_type, value_type&&>)) -> void;

            template<typename Self>
            auto unsafe(this Self& self) noexcept -> meta::forward_const_to<Self, T>&;

            auto mutex() const noexcept -> const MutexType&;

          private:
            template<template<class> class Lock, LockAccessMode Mode>
            class Access {
              public:
                using Accessvalue_type  = std::conditional_t<Mode == LockAccessMode::READ_ONLY, const value_type, value_type>;
                using RefContainerType = std::
                  conditional_t<Mode == LockAccessMode::READ_ONLY, ref<const value_type>, ref<value_type>>;

                template<typename... LockTs>
                Access(ReferenceType value, MutexType& mutex, LockTs&&... args) noexcept;

                template<typename... LockTs>
                Access(ConstReferenceType value, MutexType& mutex, LockTs&&... args) noexcept;

                template<typename... LockTs>
                    requires(not meta::IsContainerOrPointer<T>)
                explicit(sizeof...(LockTs) == 0) Access(const Locked& locked, LockTs&&... args) noexcept
                    requires(Mode == LockAccessMode::READ_ONLY);

                template<typename... LockTs>
                    requires(not meta::IsContainerOrPointer<T>)
                explicit(sizeof...(LockTs) == 0) Access(Locked& locked, LockTs&&... args) noexcept;

                template<typename... LockTs>
                    requires(meta::IsContainerOrPointer<T>)
                explicit(sizeof...(LockTs) == 0) Access(const Locked& locked, LockTs&&... args) noexcept
                    requires(Mode == LockAccessMode::READ_ONLY);

                template<typename... LockTs>
                    requires(meta::IsContainerOrPointer<T>)
                explicit(sizeof...(LockTs) == 0) Access(Locked& locked, LockTs&&... args) noexcept;

                auto operator->() const noexcept -> Accessvalue_type*;
                auto operator*() const noexcept -> Accessvalue_type&;

                mutable Lock<MutexType> lock;

              private:
                RefContainerType m_value;
            };

            mutable Mutex m_mutex;
            T m_value     STORMKIT_GUARDED_BY(m_mutex);
        };

        template<typename T>
        Locked(T) -> Locked<T>;

        template class Locked<int>;
        template class Locked<std::unique_ptr<int>>;
        template class Locked<std::queue<int>>;
    }
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Locked() noexcept(noexcept(std::is_nothrow_default_constructible_v<value_type>)) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Locked(Ts&&... args) noexcept(noexcept(std::is_nothrow_constructible_v<value_type, Ts...>))
        : m_value { std::forward<Ts>(args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    Locked<T, Mutex>::Locked(Locked&& other) noexcept {
        auto _ = other.write();

        m_value = std::move(other.m_value);
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
    template<LockAccessMode Mode, template<class> class Lock, typename... LockTs, class Self>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::access(this Self& self, LockTs&&... lock_args) noexcept -> Access<Lock, Mode> {
        static_assert(not(Mode == LockAccessMode::READ_ONLY and not std::is_const_v<meta::remove_indirections_of<Self>>),
                      "can't get read access on const Locked<T>");
        using AccessType = Access<Lock, Mode>;
        return AccessType { std::forward<Self&>(self), std::forward<LockTs>(lock_args)... };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<LockAccessMode Mode,
             std::invocable<meta::details::AccessClosureInvokeParameter<typename meta::details::Lockedvalue_type<T>::type&, Mode>>
               Closure,
             template<class> class Lock,
             typename... LockTs,
             class Self> STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::access(this Self& self, Closure&& closure, LockTs&&... lock_args) noexcept
      -> std::invoke_result_t<Closure, meta::details::AccessClosureInvokeParameter<ReferenceType, Mode>> {
        static_assert(not(Mode == LockAccessMode::READ_ONLY and not std::is_const_v<meta::remove_indirections_of<Self>>),
                      "can't get read access on const Locked<T>");
        using AccessType = Access<Lock, Mode>;
        auto access_     = AccessType { std::forward<Self&>(self), std::forward<LockTs>(lock_args)... };
        return std::invoke(std::forward<Closure>(closure), *access_);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::read(LockTs&&... lock_args) const noexcept -> ReadAccess<Lock> {
        return access<LockAccessMode::READ_ONLY, Lock>(std::forward<LockTs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<std::invocable<typename Locked<T, Mutex>::ConstReferenceType> Closure,
             template<class> class Lock,
             typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::read(Closure&& closure, LockTs&&... lock_args) const noexcept
      -> std::invoke_result_t<Closure, ConstReferenceType> {
        return access<LockAccessMode::READ_ONLY, Closure, Lock>(std::forward<Closure>(closure),
                                                                std::forward<LockTs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::write(LockTs&&... lock_args) noexcept -> WriteAccess<Lock> {
        return access<LockAccessMode::READ_WRITE, Lock>(std::forward<LockTs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<std::invocable<typename Locked<T, Mutex>::ReferenceType> Closure, template<class> class Lock, typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::write(Closure&& closure, LockTs&&... lock_args) noexcept
      -> std::invoke_result_t<Closure, ReferenceType> {
        return access<LockAccessMode::READ_WRITE, Closure, Lock>(std::forward<Closure>(closure),
                                                                 std::forward<LockTs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::copy(LockTs&&... lock_args) const noexcept -> value_type {
        return *read(std::forward<LockTs>(lock_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::
      assign(ConstReferenceType value,
             LockTs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<value_type, ConstReferenceType>)) -> void {
        *write(std::forward<LockTs>(lock_args)...) = value;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, typename... LockTs>
    STORMKIT_FORCE_INLINE
    auto Locked<T,
                Mutex>::assign(value_type&& value,
                               LockTs&&... lock_args) noexcept(noexcept(std::is_nothrow_assignable_v<value_type, value_type&&>))
      -> void {
        *write(std::forward<LockTs>(lock_args)...) = std::move(value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<typename Self>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::unsafe(this Self& self) noexcept -> meta::forward_const_to<Self, T>& {
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
    template<typename... LockTs>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(ReferenceType value, MutexType& mutex, LockTs&&... lock_args) noexcept
        : lock { mutex, std::forward<LockTs>(lock_args)... }, m_value { as_ref_mut(value) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockTs>
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(ConstReferenceType value, MutexType& mutex, LockTs&&... lock_args) noexcept
        : lock { mutex, std::forward<LockTs>(lock_args)... }, m_value { as_ref(value) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockTs>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(const Locked& locked, LockTs&&... lock_args) noexcept
        requires(Mode == LockAccessMode::READ_ONLY)
        : Access { locked.m_value, locked.m_mutex, std::forward<LockTs>(lock_args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockTs>
        requires(not meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(Locked& locked, LockTs&&... lock_args) noexcept
        : Access { locked.m_value, locked.m_mutex, std::forward<LockTs>(lock_args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockTs>
        requires(meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(const Locked& locked, LockTs&&... lock_args) noexcept
        requires(Mode == LockAccessMode::READ_ONLY)
        : Access { *locked.m_value, locked.m_mutex, std::forward<LockTs>(lock_args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    template<typename... LockTs>
        requires(meta::IsContainerOrPointer<T>)
    STORMKIT_FORCE_INLINE
    Locked<T, Mutex>::Access<Lock, Mode>::Access(Locked& locked, LockTs&&... lock_args) noexcept
        : Access { *locked.m_value, locked.m_mutex, std::forward<LockTs>(lock_args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::Access<Lock, Mode>::operator->() const noexcept -> Accessvalue_type* {
        return m_value.get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsNotRawIndirection T, class Mutex>
    template<template<class> class Lock, LockAccessMode Mode>
    STORMKIT_FORCE_INLINE
    auto Locked<T, Mutex>::Access<Lock, Mode>::operator*() const noexcept -> Accessvalue_type& {
        return *m_value;
    }
}} // namespace stormkit::core
