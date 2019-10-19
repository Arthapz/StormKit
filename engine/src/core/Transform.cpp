/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Transform.hpp>

using namespace storm;
using namespace storm::engine;

struct alignas(16) TransformData {
    core::Matrixf transform     = core::Matrixf { 1.f };
    core::Matrixf inv_transform = core::Matrixf { 1.f };
};

////////////////////////////////////////
////////////////////////////////////////
Transform::Transform(const Engine &engine) : m_engine { &engine } {
    const auto buffering_count = m_engine->surface().bufferingCount();
    const auto &device         = m_engine->device();

    m_transform_buffer = std::make_unique<RingHardwareBuffer>(buffering_count,
                                                              device,
                                                              render::HardwareBufferUsage::Uniform,
                                                              sizeof(TransformData));

    m_transform_descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_transform_buffer->buffer()),
                                   .range   = sizeof(TransformData),
                                   .offset  = 0u };

    m_is_updated = true;
    ensureUpdated();
}

////////////////////////////////////////
////////////////////////////////////////
Transform::~Transform() = default;

////////////////////////////////////////
////////////////////////////////////////
Transform::Transform(Transform &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Transform &Transform::operator=(Transform &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Transform::ensureUpdated() noexcept {
    if (!m_is_updated) return;

    recomputeMatrix();

    const auto data = TransformData { .transform     = m_transform_matrix,
                                      .inv_transform = core::inverse(m_transform_matrix) };

    m_transform_buffer->next();
    m_transform_buffer->upload<TransformData>({ &data, 1 });

    m_is_updated = false;
}

////////////////////////////////////////
////////////////////////////////////////
void Transform::recomputeMatrix() const noexcept {
    const auto translation_matrix = core::translate(core::Matrixf { 1.f }, m_position);
    const auto rotation_matrix    = core::mat4_cast(orientation());
    const auto scale_matrix       = core::scale(core::Matrixf { 1.f }, m_scale);

    m_transform_matrix = translation_matrix * rotation_matrix * scale_matrix;
}
