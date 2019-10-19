// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::module {
    /////////////////////////////////////
    /////////////////////////////////////
    inline bool AbstractModule::isLoaded() const noexcept { return m_is_loaded; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    inline std::function<Signature> AbstractModule::getFunc(std::string_view name) {
        return std::function<Signature>(reinterpret_cast<Signature *>(_getFunc(name)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class Signature>
    inline Signature AbstractModule::getCFunc(std::string_view name) {
        return reinterpret_cast<Signature>(_getFunc(name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    inline const std::filesystem::path &AbstractModule::getFilepath() const { return m_filepath; }
} // namespace storm::module
