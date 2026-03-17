// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:meta.algorithms;

import std;

import :meta.concepts;

namespace stormkit { inline namespace core { namespace meta::details {
    // template<bool, template<class> class Then, class Arg, class Else>
    // struct LazyEvaluation final {
    //     using type = Else;
    // };

    // template<template<class> class Then, class Arg, class Else>
    // struct LazyEvaluation<true, Then, Arg, Else> final {
    //     using type = Then<Arg>;
    // };

    // /*
    //     LazyEvaluationType permet de choisir un type basé sur une condition sans instancier préventivement la branche 'Then'.
    //     Contrairement à std::conditional<Condition, A, B>::type, qui force l'instanciation de A ET B même si Condition est
    //     fausse (pouvant causer des erreurs de compilation si une branche est invalide pour les types donnés),
    //     LazyEvaluationType n'instancie Then<Arg> que si la condition est vraie.
    // */
    // template<bool Condition, template<class> class Then, class Arg, class Else>
    // using LazyEvaluationType = typename LazyEvaluation<Condition, Then, Arg, Else>::type;

    template<class T>
    struct LazyType {
        using Type = T;
    };
    template<bool, template<class...> typename, typename, typename>
    struct If;

    template<template<class...> typename LazyType, typename Then, typename OrElse>
    struct If<false, LazyType, Then, OrElse> {
        using Type = LazyType<OrElse>::Type;
    };

    template<template<class...> typename LazyType, typename Then, typename OrElse>
    struct If<true, LazyType, Then, OrElse> {
        using Type = LazyType<Then>::Type;
    };
}}} // namespace stormkit::core::meta::details

export namespace stormkit { inline namespace core { namespace meta {
    template<bool Cond, typename Then, typename OrElse>
    using If = details::If<Cond, details::LazyType, Then, OrElse>::Type;

    template<bool Cond, typename Then, typename OrElse>
    using Select = std::conditional_t<Cond, Then, OrElse>;

    template<typename Predicate, template<typename...> class Variant, typename... Ts>
    constexpr auto variant_type_find_if(const Variant<Ts...>&, Predicate&& predicate) noexcept -> std::size_t;
}}} // namespace stormkit::core::meta

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core { namespace meta {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts, typename Predicate>
    STORMKIT_FORCE_INLINE
    constexpr auto variant_type_find_if_impl(Predicate&& predicate) noexcept -> std::size_t {
        auto found = std::variant_npos;
        [&]<std::size_t... Indices>(std::index_sequence<Indices...>) noexcept {
            if constexpr ((requires {
                              { std::forward<Predicate>(predicate).template operator()<Indices, Ts> } -> IsBooleanTestable;
                          } and ...))
                (((std::forward<Predicate>(predicate).template operator()<Indices, Ts>()) and (found = Indices, true)) or ...);
            else
                static_assert(false, "Type not found");
        }(std::index_sequence_for<Ts...>());
        return found;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Predicate, template<typename...> class Variant, typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto variant_type_find_if(const Variant<Ts...>&, Predicate&& predicate) noexcept -> std::size_t {
        return variant_type_find_if_impl<Ts...>(std::forward<Predicate>(predicate));
    }
}}} // namespace stormkit::core::meta
