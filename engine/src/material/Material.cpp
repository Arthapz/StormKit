/////////// - StormKit::core - ///////////
#include <storm/core/Ranges.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Scene.hpp>

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
Material::Material(Scene &scene) : m_scene { &scene } {
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
void Material::finalize() noexcept {
    STORM_EXPECTS(!m_finalized);

    const auto &engine = m_scene->engine();
    const auto &device = engine.device();

    m_descriptor_set_layout = device.createDescriptorSetLayoutPtr();

    auto last_binding = -1;
    for (const auto &[binding, name] : m_data.samplers) {
        m_descriptor_set_layout->addBinding({ binding,
                                              render::DescriptorType::Combined_Texture_Sampler,
                                              render::ShaderStage::Fragment,
                                              1 });

        last_binding = std::max(static_cast<core::Int32>(binding), last_binding);
    }

    if (!std::empty(m_data.uniforms)) {
        m_descriptor_set_layout->addBinding({ last_binding + 1u,
                                              render::DescriptorType::Uniform_Buffer,
                                              render::ShaderStage::Fragment,
                                              1 });
    }

    m_descriptor_set_layout->bake();

    m_hash = 0u;

    core::hash_combine(m_hash, m_data.shader_state);

    for (const auto &[binding, name] : m_data.samplers) {
        auto hash = core::Hash64 { 0u };
        core::hash_combine(hash, binding);
        core::hash_combine(hash, name);

        core::hash_combine(m_hash, hash);
    }

    for (const auto &[name, type] : m_data.uniforms) {
        auto hash = core::Hash64 { 0u };
        core::hash_combine(hash, name);
        core::hash_combine(hash, type);

        core::hash_combine(m_hash, hash);
    }

    m_finalized = true;
}

////////////////////////////////////////
////////////////////////////////////////
MaterialInstanceOwnedPtr Material::createInstancePtr() const noexcept {
    STORM_EXPECTS(m_finalized);

    return std::make_unique<MaterialInstance>(*m_scene, *this);
}
