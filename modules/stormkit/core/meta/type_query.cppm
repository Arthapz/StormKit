// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:meta.type_query;

import std;

import :meta.concepts;
import :meta.type_manipulation;

namespace stdr = std::ranges;

namespace stormkit { inline namespace core { namespace meta {
    namespace details {
        template<typename... Ts>
        struct AlwaysTrue: std::false_type {};

        template<typename... Ts>
        struct AlwaysFalse: std::false_type {};

        template<typename T>
        struct UnderlyingType;

        template<typename T>
        struct PointerType;

        template<IsPointer T>
        struct PointerType<T> {
            using Type = typename std::pointer_traits<T>::pointer;
        };

        template<typename T>
        struct PointerType<std::reference_wrapper<T>> {
            using Type = std::reference_wrapper<T>::type*;
        };

        template<typename T>
        struct PointedType;

        template<IsPointer T>
        struct PointedType<T> {
            using Type = typename std::pointer_traits<T>::element_type;
        };

        template<typename T>
        struct PointedType<std::reference_wrapper<T>> {
            using Type = std::reference_wrapper<T>::type;
        };

        template<typename T>
        struct ContainedType;

        template<IsContainer T>
        struct ContainedType<T> {
            using Type = typename T::value_type;
        };

        template<stdr::range T>
        struct ContainedType<T> {
            using Type = typename T::value_type;
        };

        template<typename T>
        struct ContainedOrPointedOrTType {
            using Type = T;
        };

        template<IsContainer T>
        struct ContainedOrPointedOrTType<T>: ContainedType<T> {};

        template<IsPointer T>
        struct ContainedOrPointedOrTType<T>: PointedType<T> {};

        template<typename T>
        struct ContainedOrPointedType;

        template<IsContainer T>
        struct ContainedOrPointedType<T>: ContainedType<T> {};

        template<IsPointer T>
        struct ContainedOrPointedType<T>: PointedType<T> {};

        template<class T>
        struct CallableTrait;

        template<class Return, class... Args>
        struct SignatureTrait {
            using ReturnType = Return;
            // using ArgumentsTypes = std::tuple<Args...>;
        };

        template<class Return, class... Args>
        struct CallableTrait<Return(Args...)>: SignatureTrait<Return, Args...> {};

        template<class Return, class... Args>
        struct CallableTrait<Return(Args...) noexcept>: SignatureTrait<Return, Args...> {};

        template<class Return, class... Args>
        struct CallableTrait<Return (*)(Args...)>: CallableTrait<Return(Args...)> {};

        template<class Return, class... Args>
        struct CallableTrait<Return (*)(Args...) noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...)>: CallableTrait<Return(Args...)> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) const>: CallableTrait<Return(Args...)> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) &>: CallableTrait<Return(Args...)> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) const &>: CallableTrait<Return(Args...)> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) &&>: CallableTrait<Return(Args...)> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) const &&>: CallableTrait<Return(Args...)> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) const noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) & noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) const & noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) && noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<class Object, class Return, class... Args>
        struct CallableTrait<Return (Object::*)(Args...) const && noexcept>: CallableTrait<Return(Args...) noexcept> {};

        template<typename T>
        concept HasStdValueType = requires() { typename T::value_type; };

        template<typename T>
        concept HasValueType = requires() { typename T::ValueType; };

        template<typename T>
        struct ValueType;

        template<HasValueType T>
        struct ValueType<T> {
            using Type = typename T::ValueType;
        };

        template<HasStdValueType T>
            requires(not HasValueType<T>)
        struct ValueType<T> {
            using Type = typename T::value_type;
        };

        template<typename T>
        concept HasStdElementType = requires() { typename T::element_type; };

        template<typename T>
        concept HasElementType = requires() { typename T::ElementType; };

        template<typename T>
        struct ElementType;

        template<HasElementType T>
        struct ElementType<T> {
            using Type = typename T::ElementType;
        };

        template<HasStdElementType T>
            requires(not HasElementType<T>)
        struct ElementType<T> {
            using Type = typename T::element_type;
        };
    } // namespace details

    export {
        template<typename T>
        using UnderlyingType = std::underlying_type_t<T>;

        template<typename T>
        using PointerType = details::PointerType<T>::Type;

        template<typename T>
        using PointedType = details::PointedType<T>::Type;

        template<typename T>
        using ContainedType = details::ContainedType<T>::Type;

        template<typename T>
        using ContainedOrPointedType = details::ContainedOrPointedType<T>::Type;

        template<typename T>
        using ContainedOrPointedOrTType = details::ContainedOrPointedOrTType<T>::Type;

        template<typename T>
        using ReturnType = details::CallableTrait<T>::ReturnType;

        template<HasExpectedType T>
        using ExpectedType = typename T::ExpectedType;

        template<HasValueType T>
        using ElementType = details::ElementType<T>::Type;

        template<HasValueType T>
        using ValueType = details::ValueType<T>::Type;

        template<stdr::range Range>
        using IteratorType = stdr::iterator_t<Range>;

        template<stdr::range Range>
        using SentinelType = stdr::sentinel_t<Range>;

        template<stdr::input_range Range>
        using RangeType = RemoveReferencesType<stdr::range_reference_t<Range>>;

        template<IsArithmetic T, IsArithmetic V>
        constexpr auto is_greater() noexcept;

        template<IsArithmetic T, IsArithmetic V>
        using SafeNarrowHelperType = Select<is_greater<T, V>(), T, V>;

        template<IsArithmetic T, IsArithmetic V>
        using SafeNarrowHelperOtherType = Select<is_greater<T, V>(), V, T>;

        template<IsArithmetic T>
        using ArithmeticOrderingType = Select<IsIntegral<T>, std::strong_ordering, std::partial_ordering>;

        template<IsEnumeration>
        constexpr auto enumerate() noexcept -> decltype(auto) = delete;

        template<typename T>
        consteval auto name_of() noexcept -> std::string_view;
    }
}}} // namespace stormkit::core::meta

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core { namespace meta {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<IsArithmetic T, IsArithmetic V>
    constexpr auto is_greater() noexcept {
        using Type = decltype(T {} + V {});

        return static_cast<Type>(std::numeric_limits<T>::max()) > static_cast<Type>(std::numeric_limits<V>::max());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    consteval auto name_of() noexcept -> std::string_view {
#if defined(STORMKIT_COMPILER_GCC)
        constexpr auto prefix   = std::string_view { " [with T = " };
        constexpr auto suffix   = std::string_view { "]" };
        constexpr auto function = std::string_view { __PRETTY_FUNCTION__ };
#elif defined(STORMKIT_COMPILER_CLANG)
        constexpr auto prefix   = std::string_view { " [T = " };
        constexpr auto suffix   = std::string_view { "]" };
        constexpr auto function = std::string_view { __PRETTY_FUNCTION__ };
#elif defined(STORMKIT_COMPILER_MSVC)
        constexpr auto prefix   = std::string_view { "<" };
        constexpr auto suffix   = std::string_view { ">(void)" };
        constexpr auto function = std::string_view { __FUNCSIG__ };
#else
    #error Unsupported compiler
#endif
        static_assert(not function.empty());

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end   = function.rfind(suffix);

        static_assert(start < end);

        constexpr auto        name = function.substr(start, (end - start));
        static constexpr auto res  = []<std::size_t... Idxs>(std::string_view str, std::index_sequence<Idxs...>) {
            return std::array { str[Idxs]... };
        }(name, std::make_index_sequence<name.size()> {});

        return std::string_view { res.data(), res.size() };
    }
}}} // namespace stormkit::core::meta
