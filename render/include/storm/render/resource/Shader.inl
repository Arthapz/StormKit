// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline ShaderStage Shader::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::span<const core::Byte> Shader::source() const noexcept { return m_source; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &Shader::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::ShaderModule Shader::vkShaderModule() const noexcept {
        STORM_EXPECTS(m_vk_shader_module);
        return *m_vk_shader_module;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Shader::operator vk::ShaderModule() const noexcept {
        STORM_EXPECTS(m_vk_shader_module);
        return *m_vk_shader_module;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::ShaderModule Shader::vkHandle() const noexcept { return vkShaderModule(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Shader::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkShaderModule_T *());
    }
} // namespace storm::render
