// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:typesafe.strong_type;

import std;

import :meta;
import :string.constexpr_string;
import :hash;

export {
    namespace stormkit { inline namespace core {
        struct ArithmeticTag {
            template<typename T>
            using Type = meta::UnderlyingType<meta::ToPlainType<T>>;

            template<typename Self>
            constexpr auto operator+(this Self&& self, meta::PlainIs<Type<Self>> auto&& other) noexcept
              -> meta::ToPlainType<Self> {
                return meta::ToPlainType<Self> { std::forward<Self>(self).get() + std::forward<decltype(other)>(other) };
            }

            template<typename Self>
            constexpr auto operator+=(this Self& self, meta::PlainIs<Type<Self>> auto&& other) noexcept -> Self& {
                self.get() += std::forward<decltype(other)>(other);
                return self;
            }

            template<typename Self>
            constexpr auto operator-(this Self&& self, meta::PlainIs<Type<Self>> auto&& other) noexcept
              -> meta::ToPlainType<Self> {
                return meta::ToPlainType<Self> { std::forward<Self>(self).get() - std::forward<decltype(other)>(other) };
            }

            template<typename Self>
            constexpr auto operator-=(this Self& self, meta::PlainIs<Type<Self>> auto&& other) noexcept -> Self& {
                self.get() -= std::forward<decltype(other)>(other);
                return self;
            }

            template<typename Self>
            constexpr auto operator*(this Self&& self, meta::PlainIs<Type<Self>> auto&& other) noexcept
              -> meta::ToPlainType<Self> {
                return meta::ToPlainType<Self> { std::forward<Self>(self).get() * std::forward<decltype(other)>(other) };
            }

            template<typename Self>
            constexpr auto operator*=(this Self& self, meta::PlainIs<Type<Self>> auto&& other) noexcept -> Self& {
                self.get() *= std::forward<decltype(other)>(other);
                return self;
            }

            template<typename Self>
            constexpr auto operator/(this Self&& self, meta::PlainIs<Type<Self>> auto&& other) noexcept
              -> meta::ToPlainType<Self> {
                return meta::ToPlainType<Self> { std::forward<Self>(self).get() / std::forward<decltype(other)>(other) };
            }

            template<typename Self>
            constexpr auto operator/=(this Self& self, meta::PlainIs<Type<Self>> auto&& other) noexcept -> Self& {
                self.get() /= std::forward<decltype(other)>(other);
                return self;
            }
        };

        struct ImplicitConvertionTag {};

        template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
        class StrongType;

        namespace meta {
            template<typename T>
            concept IsStrongType = IsSpecializationOfNTTP_TTVTs<T, StrongType>;

            template<typename T, typename Capability>
            concept HasCapability = DerivedFrom<T, Capability>;

            template<typename... Capabilities>
            concept HasArithmeticCapability = AnyOf<ArithmeticTag, Capabilities...>;

            template<typename... Capabilities>
            concept HasImplicitConvertionCapability = AnyOf<ImplicitConvertionTag, Capabilities...>;
        } // namespace meta

        template<typename T>
            requires(meta::IsStrongType<meta::ToPlainType<T>>)
        constexpr auto operator+(meta::PlainIs<ArithmeticTag::Type<T>> auto&& first, T&& second) noexcept
          -> meta::ToPlainType<T> {
            return meta::ToPlainType<T> { std::forward<T>(second).get() + std::forward<decltype(first)>(first) };
        }

        template<typename T>
            requires(meta::IsStrongType<meta::ToPlainType<T>>)
        constexpr auto operator-(meta::PlainIs<ArithmeticTag::Type<T>> auto&& first, T&& second) noexcept
          -> meta::ToPlainType<T> {
            return meta::ToPlainType<T> { std::forward<T>(second).get() - std::forward<decltype(first)>(first) };
        }

        template<typename T>
            requires(meta::IsStrongType<meta::ToPlainType<T>>)
        constexpr auto operator*(meta::PlainIs<ArithmeticTag::Type<T>> auto&& first, T&& second) noexcept
          -> meta::ToPlainType<T> {
            return meta::ToPlainType<T> { std::forward<T>(second).get() * std::forward<decltype(first)>(first) };
        }

        template<typename T>
            requires(meta::IsStrongType<meta::ToPlainType<T>>)
        constexpr auto operator/(meta::PlainIs<ArithmeticTag::Type<T>> auto&& first, T&& second) noexcept
          -> meta::ToPlainType<T> {
            return meta::ToPlainType<T> { std::forward<T>(second).get() / std::forward<decltype(first)>(first) };
        }

        template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
        class StrongType: public Capabilities... {
          public:
            using ValueType = T;

            constexpr explicit StrongType(ValueType value) noexcept(meta::IsNoexceptConstructible<ValueType, ValueType>);
            template<class... Args>
            constexpr explicit(sizeof...(Args) == 1)
              StrongType(std::in_place_t, Args&&... args) noexcept(meta::IsNoexceptConstructible<ValueType, Args...>)
                requires(meta::IsConstructible<ValueType, Args...>);
            constexpr ~StrongType() noexcept(meta::IsNoexceptDestructible<ValueType>);

            constexpr StrongType(const StrongType&) noexcept(meta::IsNoexceptCopyConstructible<ValueType>)
                requires(meta::IsCopyConstructible<ValueType>);
            constexpr auto operator=(const StrongType&) noexcept(meta::IsNoexceptCopyAssignable<ValueType>) -> StrongType&
                requires(meta::IsCopyAssignable<ValueType>);

            constexpr StrongType(StrongType&&) noexcept(meta::IsNoexceptMoveConstructible<ValueType>)
                requires(meta::IsMoveConstructible<ValueType>);
            constexpr auto operator=(StrongType&&) noexcept(meta::IsNoexceptMoveAssignable<ValueType>) -> StrongType&
                requires(meta::IsMoveAssignable<ValueType>);

            template<typename Self>
            [[nodiscard]]
            constexpr explicit(not meta::HasImplicitConvertionCapability<Capabilities...>) operator T(this Self&& self) noexcept;

            [[nodiscard]]
            constexpr explicit(not meta::HasImplicitConvertionCapability<Capabilities...>) operator T&() & noexcept;
            [[nodiscard]]
            constexpr explicit(not meta::HasImplicitConvertionCapability<Capabilities...>) operator const T&() const & noexcept;

            [[nodiscard]]
            constexpr auto get(this auto&& self) noexcept -> decltype(auto);

          private:
            ValueType m_value;
        };

        template<typename T>
        constexpr auto value_of(T&& value) noexcept -> decltype(auto);

        // template<meta::IsStrongType                                    First,
        //          meta::PlainIs<typename ToPlainType<First>::ValueType> Second>
        //     requires(meta::HasArithmeticCapability<ToPlainType<First>>)
        // constexpr auto operator+(First&& first, Second&& second) {
        //     return ToPlainType<First> { std::forward<First>(first).get()
        //                                 + std::forward<Second>(second) };
        // }

        // template<meta::IsStrongType First, meta::PlainIs<First> Second>
        //     requires(meta::HasArithmeticCapability<ToPlainType<First>>)
        // constexpr auto operator+(First&& first, Second&& second) {
        //     return ToPlainType<First> { std::forward<First>(first).get()
        //                                 + std::forward<Second>(second).get() };
        // }

        // template<meta::IsStrongType                                    First,
        //          meta::PlainIs<typename ToPlainType<First>::ValueType> Second>
        //     requires(meta::HasArithmeticCapability<ToPlainType<First>>)
        // constexpr auto operator+=(First& first, Second&& second) {
        //     first.get() += std::forward<Second>(second);
        // }

        // template<meta::IsStrongType First, meta::PlainIs<First> Second>
        //     requires(meta::HasArithmeticCapability<ToPlainType<First>>)
        // constexpr auto operator+=(First& first, Second&& second) {
        //     first.get() += std::forward<Second>(second).get();
        // }

        template<meta::HashType Ret = hash32, meta::IsStrongType T>
        constexpr auto hasher(const T& value) noexcept -> Ret;

        template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities, typename FormatContext>
        auto format_as(const StrongType<T, Tag, Name, Capabilities...>& value, FormatContext& ctx) noexcept
          -> decltype(ctx.out());
    }} // namespace stormkit::core
}

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE
    constexpr StrongType<T, Tag, Name, Capabilities...>::
      StrongType(ValueType value) noexcept(meta::IsNoexceptConstructible<ValueType, ValueType>)
        : m_value { value } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    template<class... Args>
    STORMKIT_FORCE_INLINE
    constexpr StrongType<T, Tag, Name, Capabilities...>::
      StrongType(std::in_place_t, Args&&... args) noexcept(meta::IsNoexceptConstructible<ValueType, Args...>)
        requires(meta::IsConstructible<ValueType, Args...>)
        : m_value { std::forward<Args>(args)... } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE
    constexpr StrongType<T, Tag, Name, Capabilities...>::~StrongType() noexcept(meta::IsNoexceptDestructible<
                                                                                ValueType>) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE
    constexpr StrongType<T, Tag, Name, Capabilities...>::StrongType(const StrongType&) noexcept(meta::IsNoexceptCopyConstructible<
                                                                                                ValueType>)
        requires(meta::IsCopyConstructible<ValueType>)
    = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE
    constexpr auto StrongType<T, Tag, Name, Capabilities...>::
      operator=(const StrongType&) noexcept(meta::IsNoexceptCopyAssignable<ValueType>) -> StrongType&
        requires(meta::IsCopyAssignable<ValueType>)
    = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE
    constexpr StrongType<T, Tag, Name, Capabilities...>::StrongType(StrongType&&) noexcept(meta::IsNoexceptMoveConstructible<
                                                                                           ValueType>)
        requires(meta::IsMoveConstructible<ValueType>)
    = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE
    constexpr auto StrongType<T, Tag, Name, Capabilities...>::operator=(StrongType&&) noexcept(meta::IsNoexceptMoveAssignable<
                                                                                               ValueType>) -> StrongType&
        requires(meta::IsMoveAssignable<ValueType>)
    = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    template<typename Self>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr StrongType<T, Tag, Name, Capabilities...>::operator T(this Self&& self) noexcept {
        return std::forward_like<decltype(self)>(self.m_value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr StrongType<T, Tag, Name, Capabilities...>::operator T&() & noexcept {
        return get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr StrongType<T, Tag, Name, Capabilities...>::operator const T&() const & noexcept {
        return get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto StrongType<T, Tag, Name, Capabilities...>::get(this auto&& self) noexcept -> decltype(auto) {
        return std::forward_like<decltype(self)>(self.m_value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE STORMKIT_PURE
    constexpr auto value_of(T&& value) noexcept -> decltype(auto) {
        if constexpr (meta::IsStrongType<T>) return std::forward<T>(value).get();
        else
            return std::forward<T>(value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::HashType Ret, meta::IsStrongType T>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const T& value) noexcept -> Ret {
        return hash<Ret>(value.get());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<meta::IsPlain T, typename Tag, meta::ConstexprString Name, typename... Capabilities, typename FormatContext>
    STORMKIT_FORCE_INLINE
    inline auto format_as(const StrongType<T, Tag, Name, Capabilities...>& value, FormatContext& ctx) noexcept
      -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[{} value: {}]", Name, value.get());
    }
}} // namespace stormkit::core
