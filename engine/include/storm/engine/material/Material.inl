#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setGraphicsPipelineState(render::GraphicsPipelineState state) noexcept {
        m_pipeline_state = std::move(state);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::GraphicsPipelineState &Material::pipelineState() const noexcept {
        return m_pipeline_state;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setBaseColorMap(const render::Texture &map) noexcept {
        m_base_color_map = core::makeConstObserver(map);

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Material::hasBaseColorMap() const noexcept { return m_base_color_map != nullptr; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setNormalMap(const render::Texture &map) noexcept {
        m_normal_map = core::makeConstObserver(map);

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Material::hasNormalMap() const noexcept { return m_normal_map != nullptr; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setMetallicRoughnessMap(const render::Texture &map) noexcept {
        m_metallic_roughness_map = core::makeConstObserver(map);

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Material::hasMetallicRoughnessMap() const noexcept {
        return m_metallic_roughness_map != nullptr;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setAmbiantOcclusionMap(const render::Texture &map) noexcept {
        m_ambiant_occlusion_map = core::makeConstObserver(map);

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Material::hasAmbiantOcclusionMap() const noexcept {
        return m_ambiant_occlusion_map != nullptr;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setEmissiveMap(const render::Texture &map) noexcept {
        m_emissive_map = core::makeConstObserver(map);

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    bool Material::hasEmissiveMap() const noexcept { return m_emissive_map != nullptr; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setBaseColorFactor(const core::RGBColorF &color) noexcept {
        m_data.base_color_factor = color.toVector4();

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::RGBColorF Material::baseColorFactor() const noexcept {
        auto color = core::RGBColorF { 1.f, 1.f, 1.f, 1.f };
        color.fromVector4(m_data.base_color_factor);

        return color;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setMetallicFactor(float factor) noexcept {
        m_data.metallic_factor = factor;

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline float Material::metallicFactor() const noexcept { return m_data.metallic_factor; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setRoughnessFactor(float factor) noexcept {
        m_data.metallic_factor = factor;

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline float Material::roughnessFactor() const noexcept { return m_data.roughness_factor; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline float Material::ambiantOcclusionFactor() const noexcept {
        return m_data.roughness_factor;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setAmbiantOcclusionFactor(float factor) noexcept {
        m_data.ambiant_occlusion_factor = factor;

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::RGBColorF Material::emissiveFactor() const noexcept {
        auto color = core::RGBColorF { 1.f, 1.f, 1.f, 1.f };
        color.fromVector4(m_data.emissive_factor);

        return color;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Material::setEmissiveFactor(const core::RGBColorF &factor) noexcept {
        m_data.emissive_factor = factor.toVector4();

        m_is_dirty            = true;
        m_need_recompute_hash = true;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const std::vector<std::byte> &Material::pushConstantsData() const noexcept {
        return m_push_constant_data;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::DescriptorSet &Material::descriptorSet() const noexcept {
        return *m_descriptor_set;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool Material::isDirty() const noexcept { return m_is_dirty; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UInt64 Material::hash() const noexcept {
        if (m_need_recompute_hash) {
            recomputeHash();
            m_need_recompute_hash = false;
        }

        return m_hash;
    }
} // namespace storm::engine
