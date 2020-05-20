// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void Material::addShader(const render::Shader &shader) {
        STORM_EXPECTS(!m_finalized);

        m_data.shader_state.shaders.emplace_back(core::makeConstObserver(shader));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void Material::addSampler(Binding binding, std::string name) {
        STORM_EXPECTS(!m_finalized);

        m_data.samplers.emplace_back(binding, std::move(name));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void Material::addUniform(std::string name, UniformType type) {
        STORM_EXPECTS(!m_finalized);

        m_data.uniforms.emplace_back(std::move(name), type);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    core::Hash64 Material::hash() const noexcept {
        STORM_EXPECTS(m_finalized);

        return m_hash;
    }
} // namespace storm::engine
