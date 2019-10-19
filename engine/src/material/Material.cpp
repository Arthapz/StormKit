/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Ranges.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/material/Material.hpp>

using namespace storm;
using namespace storm::engine;

namespace std {
    core::Hash64 hash<Material>::operator()(const Material &material) const noexcept {
        return material.hash();
    }
} // namespace std

////////////////////////////////////////
////////////////////////////////////////
Material::Material(const Engine &engine) : m_engine { &engine } {
    m_push_constant_data.resize(sizeof(m_data));

    const auto &pool             = m_engine->descriptorPool();
    const auto &pipeline_builder = m_engine->pipelineBuilder();

    m_sampler        = m_engine->device().createSamplerPtr();
    m_descriptor_set = pool.allocateDescriptorSetPtr(pipeline_builder.pbrMaterialLayout());

    auto descriptors = render::DescriptorArray {};

    const auto &map = m_engine->texturePool().get("BlankTexture");

    m_default_map_view = map.createViewPtr();
    descriptors.emplace_back(storm::render::TextureDescriptor {
        .binding      = 0,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_default_map_view),
        .sampler      = core::makeConstObserver(m_sampler) });
    descriptors.emplace_back(storm::render::TextureDescriptor {
        .binding      = 1,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_default_map_view),
        .sampler      = core::makeConstObserver(m_sampler) });
    descriptors.emplace_back(storm::render::TextureDescriptor {
        .binding      = 2,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_default_map_view),
        .sampler      = core::makeConstObserver(m_sampler) });
    descriptors.emplace_back(storm::render::TextureDescriptor {
        .binding      = 3,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_default_map_view),
        .sampler      = core::makeConstObserver(m_sampler) });
    descriptors.emplace_back(storm::render::TextureDescriptor {
        .binding      = 4,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_default_map_view),
        .sampler      = core::makeConstObserver(m_sampler) }); // todo generate correct layout

    m_descriptor_set->update(descriptors);
}

////////////////////////////////////////
////////////////////////////////////////
Material::~Material() = default;

////////////////////////////////////////
////////////////////////////////////////
Material::Material(Material &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Material &Material::operator=(Material &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Material::recomputeHash() const noexcept {
    m_hash = 0u;

    core::hash_combine(m_hash, m_pipeline_state);
    core::hash_combine(m_hash, m_data.base_color_factor);
    core::hash_combine(m_hash, m_base_color_map);
    core::hash_combine(m_hash, m_normal_map);
    core::hash_combine(m_hash, m_metallic_roughness_map);
    core::hash_combine(m_hash, m_ambiant_occlusion_map);

    core::hash_combine(m_hash, m_data.metallic_factor);
    core::hash_combine(m_hash, m_data.roughness_factor);
    core::hash_combine(m_hash, m_data.ambiant_occlusion_factor);
}

////////////////////////////////////////
////////////////////////////////////////
bool Material::ensureIsUpdated() {
    if (!m_is_dirty) return false;

    auto data = core::makeConstSpan<std::byte>(m_data);
    m_push_constant_data.resize(std::size(data));

    core::ranges::copy(data, core::ranges::begin(m_push_constant_data));

    auto descriptors = render::DescriptorArray {};

    if (m_base_color_map) {
        if (!m_base_color_map_view || &m_base_color_map_view->texture() != m_base_color_map.get()) {
            m_base_color_map_view = m_base_color_map->createViewPtr();

            descriptors.emplace_back(storm::render::TextureDescriptor {
                .binding      = 0,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(m_base_color_map_view),
                .sampler      = core::makeConstObserver(m_sampler) });
        }
    }

    if (m_normal_map) {
        if (!m_normal_map_view || &m_normal_map_view->texture() != m_normal_map.get()) {
            m_normal_map_view = m_normal_map->createViewPtr();

            descriptors.emplace_back(storm::render::TextureDescriptor {
                .binding      = 1,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(m_normal_map_view),
                .sampler      = core::makeConstObserver(m_sampler) });
        }
    }

    if (m_metallic_roughness_map) {
        if (!m_metallic_roughness_map_view ||
            &m_metallic_roughness_map_view->texture() != m_metallic_roughness_map.get()) {
            m_metallic_roughness_map_view = m_metallic_roughness_map->createViewPtr();

            descriptors.emplace_back(storm::render::TextureDescriptor {
                .binding      = 2,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(m_metallic_roughness_map_view),
                .sampler      = core::makeConstObserver(m_sampler) });
        }
    }

    if (m_ambiant_occlusion_map) {
        if (!m_ambiant_occlusion_map_view ||
            &m_ambiant_occlusion_map_view->texture() != m_ambiant_occlusion_map.get()) {
            m_ambiant_occlusion_map_view = m_ambiant_occlusion_map->createViewPtr();

            descriptors.emplace_back(storm::render::TextureDescriptor {
                .binding      = 3,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(m_ambiant_occlusion_map_view),
                .sampler      = core::makeConstObserver(m_sampler) });
        }
    }

    if (m_emissive_map) {
        if (!m_emissive_map_view || &m_emissive_map_view->texture() != m_emissive_map.get()) {
            m_emissive_map_view = m_emissive_map->createViewPtr();

            descriptors.emplace_back(storm::render::TextureDescriptor {
                .binding      = 4,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(m_emissive_map_view),
                .sampler      = core::makeConstObserver(m_sampler) });
        }
    }

    if (!std::empty(descriptors)) { m_descriptor_set->update(descriptors); }

    m_is_dirty = false;

    return true;
}
