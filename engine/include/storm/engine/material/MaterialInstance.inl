// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void MaterialInstance::setDoubleSided(bool double_sided) noexcept {
        m_rasterization_state.cull_mode =
            (double_sided) ? render::CullMode::None : render::CullMode::Back;

        m_dirty      = true;
        m_dirty_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void MaterialInstance::setWireFrameEnabled(bool enabled) noexcept {
        m_rasterization_state.polygon_mode =
            (enabled) ? render::PolygonMode::Line : render::PolygonMode::Fill;

        m_dirty      = true;
        m_dirty_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void MaterialInstance::setSamplerTexture(std::string_view name,
                                             const render::Texture &texture) {
        const auto it = core::ranges::find_if(m_sampled_textures, [&name](const auto &pair) {
            return name == pair.first;
        });

        STORM_EXPECTS(it != core::ranges::cend(m_sampled_textures));

        auto &[_, binding] = *it;
        binding.view       = texture.createViewPtr();
        binding.dirty      = true;

        m_dirty      = true;
        m_dirty_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void MaterialInstance::setRawDataValue(std::string_view name, core::ByteConstSpan bytes) {
        const auto it = core::ranges::find_if(m_data_offsets, [&name](const auto &pair) {
            return name == pair.first;
        });

        STORM_EXPECTS(it != core::ranges::cend(m_data_offsets));

        core::ranges::copy(bytes, core::ranges::begin(m_bytes) + it->second);

        m_dirty       = true;
        m_bytes_dirty = true;
        m_dirty_hash  = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    void MaterialInstance::setDataValue(std::string_view name, T &&value) {
        const auto *bytes = reinterpret_cast<const core::Byte *>(&value);
        setRawDataValue(name, { bytes, sizeof(value) });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const Material &MaterialInstance::parent() const noexcept { return *m_parent; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    core::Hash64 MaterialInstance::hash() const noexcept {
        if (m_dirty_hash) recomputeHash();

        return m_hash;
    }
} // namespace storm::engine
