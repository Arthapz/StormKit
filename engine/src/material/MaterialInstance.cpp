/////////// - StormKit::core - ///////////
#include <storm/core/Ranges.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/material/MaterialInstance.hpp>

#include <storm/engine/scene/Scene.hpp>

using namespace storm;
using namespace storm::engine;

namespace std {
    core::Hash64
        hash<MaterialInstance>::operator()(const MaterialInstance &material) const noexcept {
        return material.hash();
    }
} // namespace std

inline constexpr core::UOffset getSizeOfType(Material::UniformType type) noexcept {
    switch (type) {
        case Material::UniformType::UInt8: return sizeof(core::UInt8);
        case Material::UniformType::UInt16: return sizeof(core::UInt16);
        case Material::UniformType::UInt32: return sizeof(core::UInt32);
        case Material::UniformType::Int8: return sizeof(core::Int8);
        case Material::UniformType::Int16: return sizeof(core::Int16);
        case Material::UniformType::Int32: return sizeof(core::Int32);
        case Material::UniformType::Float: return sizeof(float);
        case Material::UniformType::Double: return sizeof(double);
        case Material::UniformType::Vec2u: return sizeof(core::Vector2u);
        case Material::UniformType::Vec2i: return sizeof(core::Vector2i);
        case Material::UniformType::Vec2f: return sizeof(core::Vector2f);
        case Material::UniformType::Vec4u: return sizeof(core::Vector4u);
        case Material::UniformType::Vec4i: return sizeof(core::Vector4i);
        case Material::UniformType::Vec4f: return sizeof(core::Vector4f);
        case Material::UniformType::Mat4u: return sizeof(core::Matrixf);
        case Material::UniformType::Mat4i: return sizeof(core::Matrixi);
        case Material::UniformType::Mat4f: return sizeof(core::Matrixu);
    }

    return core::UOffset { 0u };
}

inline constexpr core::UInt32 getAlignmentOfType(Material::UniformType type) noexcept {
    switch (type) {
        case Material::UniformType::UInt8: return sizeof(core::UInt8);
        case Material::UniformType::UInt16: return sizeof(core::UInt16);
        case Material::UniformType::UInt32: return sizeof(core::UInt32);
        case Material::UniformType::Int8: return sizeof(core::Int8);
        case Material::UniformType::Int16: return sizeof(core::Int16);
        case Material::UniformType::Int32: return sizeof(core::Int32);
        case Material::UniformType::Float: return sizeof(float);
        case Material::UniformType::Double: return sizeof(double);
        case Material::UniformType::Vec2u: return 2 * sizeof(core::UInt32);
        case Material::UniformType::Vec2i: return 2 * sizeof(core::Int32);
        case Material::UniformType::Vec2f: return 2 * sizeof(float);
        case Material::UniformType::Mat4u: [[fallthrough]];
        case Material::UniformType::Vec4u: return 4 * sizeof(core::UInt32);
        case Material::UniformType::Mat4i: [[fallthrough]];
        case Material::UniformType::Vec4i: return 4 * sizeof(core::Int32);
        case Material::UniformType::Mat4f: [[fallthrough]];
        case Material::UniformType::Vec4f: return 4 * sizeof(float);
    }

    return core::UOffset { 0u };
}

template<typename T, typename U>
inline constexpr auto align(T size, U alignement) {
    const auto left_over = size % alignement;

    if (left_over > 0) return size + (alignement - left_over);

    return size;
}

////////////////////////////////////////
////////////////////////////////////////
MaterialInstance::MaterialInstance(const Scene &scene, const Material &material)
    : Bindable { [&material](Bindable::DescriptorSetVariant &layout) {
          layout = core::makeConstObserver(material.m_descriptor_set_layout);
      } },
      m_engine { &scene.engine() }, m_parent { &material } {
    const auto buffering_count = m_engine->surface().bufferingCount();
    const auto &device         = m_engine->device();

    const auto &pool = m_engine->descriptorPool();

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(buffering_count, descriptorLayout());

    const auto &sampleds = m_parent->m_data.samplers;
    m_sampled_textures.reserve(std::size(sampleds));

    const auto &default_map = scene.texturePool().get("BlankTexture");
    for (const auto &[binding, name] : sampleds) {
        m_sampled_textures.emplace(name,
                                   SampledBinding { .binding = binding,
                                                    .view    = default_map.createViewPtr(),
                                                    .sampler = device.createSamplerPtr() });

        m_buffer_binding = std::max(m_buffer_binding, static_cast<core::Int32>(binding));
    }
    m_buffer_binding += 1;

    const auto &uniforms = m_parent->m_data.uniforms;

    auto current_offset = core::UOffset { 0u };
    for (const auto &[name, type] : uniforms) {
        m_data_offsets.emplace(name, current_offset);

        const auto size         = getSizeOfType(type);
        const auto aligned_size = align(size, getAlignmentOfType(type));

        current_offset += aligned_size;
    }

    m_bytes.resize(current_offset, std::byte { 0 });

    m_buffer = std::make_unique<RingHardwareBuffer>(buffering_count,
                                                    device,
                                                    render::HardwareBufferUsage::Uniform,
                                                    current_offset);
}

////////////////////////////////////////
////////////////////////////////////////
MaterialInstance::~MaterialInstance() = default;

////////////////////////////////////////
////////////////////////////////////////
MaterialInstance::MaterialInstance(MaterialInstance &&) = default;

////////////////////////////////////////
////////////////////////////////////////
MaterialInstance &MaterialInstance::operator=(MaterialInstance &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void MaterialInstance::flush() {
    if (!m_dirty) return;

    auto descriptors = render::DescriptorArray {};

    for (auto &[_, sampled_texture] : m_sampled_textures) {
        descriptors.emplace_back(render::TextureDescriptor {
            .binding      = sampled_texture.binding,
            .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
            .texture_view = core::makeConstObserver(sampled_texture.view),
            .sampler      = core::makeConstObserver(sampled_texture.sampler) });

        if (sampled_texture.dirty) { sampled_texture.dirty = false; }
    }

    if (m_bytes_dirty && !std::empty(m_bytes)) {
        m_buffer->next();
        m_buffer->upload<core::Byte>(m_bytes);

        m_bytes_dirty = false;
    }

    if (!std::empty(m_bytes)) {
        auto offset = m_buffer->currentOffset();
        descriptors.emplace_back(
            render::BufferDescriptor { .binding = static_cast<Material::Binding>(m_buffer_binding),
                                       .buffer  = core::makeConstObserver(m_buffer->buffer()),
                                       .range  = gsl::narrow_cast<core::UInt32>(std::size(m_bytes)),
                                       .offset = offset });
    }

    if (!std::empty(descriptors)) {
        next();
        descriptorSet().update(descriptors);
    }

    m_dirty = false;
}

////////////////////////////////////////
////////////////////////////////////////
void MaterialInstance::recomputeHash() const noexcept {
    m_hash = 0u;

    core::hash_combine(m_hash, *m_parent);
    core::hash_combine(m_hash, m_rasterization_state);

    m_dirty_hash = false;
}
