// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core.singleton;

import std;

export namespace stormkit { inline namespace core {
    template<class T>
    class Singleton {
      public:
        template<class... Ts>
        static auto instance(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T>) -> T&;

        Singleton(Singleton&&)      = delete;
        Singleton(const Singleton&) = delete;

        auto operator=(Singleton&&) -> Singleton&      = delete;
        auto operator=(const Singleton&) -> Singleton& = delete;

      protected:
        Singleton() noexcept  = default;
        ~Singleton() noexcept = default;

      private:
        static auto once_flag() noexcept -> std::once_flag&;

        static inline std::unique_ptr<T> m_instance = nullptr;
    };
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    template<class... Ts>
    auto Singleton<T>::instance(Ts&&... args) noexcept(std::is_nothrow_constructible_v<T>) -> T& {
        auto lambdas = [](Ts&&... args) mutable { m_instance = std::make_unique<T>(std::forward<Ts>(args)...); };

        std::call_once(once_flag(), lambdas, std::forward<Ts>(args)...);

        return *m_instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto Singleton<T>::once_flag() noexcept -> std::once_flag& {
        static auto once_flag = std::once_flag {};
        return once_flag;
    }
}} // namespace stormkit::core
