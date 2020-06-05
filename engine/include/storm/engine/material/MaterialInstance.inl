// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void MaterialInstance::setFrontFace(render::FrontFace face) {
        m_rasterization_state.front_face = face;

        m_dirty      = true;
        m_dirty_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void MaterialInstance::setCullMode(render::CullMode mode) {
        m_rasterization_state.cull_mode = mode;

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
    void MaterialInstance::setSampledTexture(
        std::string_view name,
        const render::Texture &texture,
        render::TextureViewType type,
        std::optional<render::TextureSubresourceRange> subresource_range,
        std::optional<render::Sampler::Settings> sampler_settings) {
        auto settings = sampler_settings.value_or(
            render::Sampler::Settings { .enable_anisotropy = true,
                                        .max_anisotropy    = m_engine->maxAnisotropy(),
                                        .compare_operation = render::CompareOperation::Never,
                                        .max_lod = static_cast<float>(texture.mipLevels()) });

        auto range = subresource_range.value_or(
            render::TextureSubresourceRange { .level_count = texture.mipLevels() });

        const auto it = std::find_if(m_sampled_textures.begin(),
                                     m_sampled_textures.end(),
                                     [&name](const auto &pair) { return name == pair.first; });

        STORM_EXPECTS(it != m_sampled_textures.cend());

        auto &[_, binding] = *it;
        binding.view       = texture.createViewPtr(type, std::move(range));
        binding.sampler    = m_engine->device().createSamplerPtr(std::move(settings));
        binding.dirty      = true;

        m_dirty      = true;
        m_dirty_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void MaterialInstance::setSamplerSettings(std::string_view name,
                                              render::Sampler::Settings sampler_settings) {
        const auto it = std::find_if(m_sampled_textures.begin(),
                                     m_sampled_textures.end(),
                                     [&name](const auto &pair) { return name == pair.first; });

        STORM_EXPECTS(it != m_sampled_textures.cend());

        auto &[_, binding] = *it;
        binding.sampler    = m_engine->device().createSamplerPtr(std::move(sampler_settings));
        binding.dirty      = true;

        m_dirty      = true;
        m_dirty_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void MaterialInstance::setRawDataValue(std::string_view name, core::ByteConstSpan bytes) {
        const auto it = std::find_if(std::cbegin(m_data_offsets),
                                     std::cend(m_data_offsets),
                                     [&name](const auto &pair) { return name == pair.first; });

        STORM_EXPECTS(it != std::cend(m_data_offsets));

        std::copy(std::cbegin(bytes), std::cend(bytes), std::begin(m_bytes) + it->second);

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
