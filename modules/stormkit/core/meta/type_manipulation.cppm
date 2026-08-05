// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core.meta.type_manipulation;

import std;

import stormkit.core.meta.concepts;
import stormkit.core.meta.algorithms;

namespace stormkit { inline namespace core { namespace meta {
    export {
        template<typename T>
        using ToPlainType = std::remove_cvref_t<T>;

        // template<typename T, template<typename> concept C>
        // concept PlainTypeTo = C<ToPlainType<T>>;

        template<typename T>
        using CanonicalT = std::remove_cvref_t<T>;

        template<auto value>
        using CanonicalTOf = CanonicalT<decltype(value)>;

        template<typename T>
        using ToDecayedType = std::decay_t<T>;

        // template<typename T, template<typename> concept C>
        // concept DecayedTypeTo = C<ToDecayedType<T>>;

        template<typename T>
        using AddConst = std::add_const_t<T>;

        template<bool VALUE, typename T>
        using AddConstIf = If<VALUE, AddConst<T>, T>;

        template<typename T>
        using AddVolatile = std::add_volatile_t<T>;

        template<typename T>
        using AddRValueReference = std::add_rvalue_reference_t<T>;

        template<typename T>
        using AddLValueReference = std::add_lvalue_reference_t<T>;

        template<typename T>
        using RemoveConst = std::remove_const_t<T>;

        template<typename T>
        using RemoveReferencesType = std::remove_reference_t<T>;

        template<typename T>
        using RemovePointerType = std::remove_pointer_t<T>;

        template<typename T>
        using RemoveIndirectionsType = RemovePointerType<RemoveReferencesType<T>>;
    }

    namespace details {
        template<typename T, typename U>
        struct ForwardConst {
          private:
            using T1 = RemoveIndirectionsType<T>;
            using U1 = Select<IsConst<T1>, AddConst<U>, U>;
            using U2 = Select<IsVolatile<T1>, AddVolatile<U1>, U1>;

          public:
            using Type = U2;
        };

        template<typename T, typename U>
        struct ForwardRef {
          private:
            using U2 = Select<IsLValueReference<T>, AddLValueReference<U>, U>;
            using U3 = Select<IsRValueReference<T>, AddRValueReference<U2>, U2>;

          public:
            using Type = U2;
        };
    } // namespace details

    export {
        template<typename T, typename U>
        using ForwardConst = details::ForwardConst<T, U>::Type;

        template<typename T, typename U>
        using ForwardRefTo = details::ForwardRef<T, U>::Type;

        template<typename T, typename U>
        using ForwardLike = ForwardRefTo<T, ForwardConst<T, U>>;

        template<typename T>
        using In = If<ShouldPassByValue<T>, T, const T&>;

        template<typename T>
        using Take = If<ShouldPassByValue<T>, T, T&&>;
    }
}}} // namespace stormkit::core::meta
