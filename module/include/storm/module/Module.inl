// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::module {
    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Module::isLoaded() const noexcept { return m_impl->isLoaded(); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    inline std::function<Signature> Module::getFunc(std::string_view name) const {
        STORM_EXPECTS(!name.empty());

        return m_impl->getFunc<Signature>(name);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    inline Signature Module::getCFunc(std::string_view name) const {
        STORM_EXPECTS(!name.empty());

        return m_impl->getCFunc<Signature>(name);
    }
} // namespace storm::module
