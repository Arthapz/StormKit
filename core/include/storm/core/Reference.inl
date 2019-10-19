// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename U, typename T, typename... Others>
    inline const U &
        ReferenceResolve<U, T, Others...>::operator()(const Reference<T, Others...> &owner) {
        return owner.m_next.template get<U>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename U, typename T, typename... Others>
    inline U &ReferenceResolve<U, T, Others...>::operator()(Reference<T, Others...> &owner) {
        return owner.m_next.template get<U>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    inline const T &
        ReferenceResolve<T, T, Others...>::operator()(const Reference<T, Others...> &owner) {
        return owner.m_value.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    inline T &ReferenceResolve<T, T, Others...>::operator()(Reference<T, Others...> &owner) {
        return owner.m_value.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    Reference<T, Others...>::Reference(T &value, Others &... others)
        : m_value { value }, m_next { others... } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    Reference<T, Others...>::~Reference() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    Reference<T, Others...>::Reference(Reference &&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    Reference<T, Others...> &Reference<T, Others...>::operator=(Reference &&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    template<typename U>
    inline U &Reference<T, Others...>::get() {
        return ReferenceResolve<U, T, Others...> {}(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    template<typename U>
    inline const U &Reference<T, Others...>::get() const {
        return ReferenceResolve<U, T, Others...> {}(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    template<typename U, typename V>
    inline V &Reference<T, Others...>::getAs() {
        static_assert(std::is_base_of_v<U, V>, "U need to be inheriting from V");

        return static_cast<V &>(ReferenceResolve<U, T, Others...> {}(*this));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Others>
    template<typename U, typename V>
    inline const V &Reference<T, Others...>::getAs() const {
        static_assert(std::is_base_of_v<U, V>, "U need to be inheriting from V");

        return static_cast<const V &>(ReferenceResolve<U, T, Others...> {}(*this));
    }
} // namespace storm::core
