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
Transform::Transform(Engine &engine) : m_engine { &engine } {
    const auto buffering_count = m_engine->surface().bufferingCount();
    const auto &device         = m_engine->device();
    const auto &pool           = m_engine->descriptorPool();

    m_transform_buffer = std::make_unique<RingHardwareBuffer>(buffering_count,
                                                              device,
                                                              render::HardwareBufferUsage::Uniform,
                                                              sizeof(TransformData));

    const auto descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_transform_buffer->buffer()),
                                   .range   = sizeof(TransformData),
                                   .offset  = 0u };
    const auto descriptors =
        render::DescriptorStaticArray<1> { render::Descriptor { std::move(descriptor) } };

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());
    descriptorSet().update(descriptors);

    m_is_updated = true;
    m_dirty      = true;
    flush();
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
void Transform::flush() noexcept {
    if (!m_dirty) return;

    const auto m = matrix();

    const auto data = TransformData { .transform = m, .inv_transform = core::inverse(m) };

    m_transform_buffer->next();
    m_transform_buffer->upload<TransformData>({ &data, 1 });

    m_offset = m_transform_buffer->currentOffset();

    m_dirty = false;
}

////////////////////////////////////////
////////////////////////////////////////
void Transform::recomputeOrientation() const noexcept {
    const auto pitch =
        core::angleAxis(core::radians(m_orientation_euler.x), core::Vector3f { 1.f, 0.f, 0.f });
    const auto yaw =
        core::angleAxis(core::radians(m_orientation_euler.y), core::Vector3f { 0.f, 1.f, 0.f });
    const auto roll =
        core::angleAxis(core::radians(m_orientation_euler.z), core::Vector3f { 0.f, 0.f, 1.f });

    m_orientation = roll * pitch * yaw;
}

////////////////////////////////////////
////////////////////////////////////////
void Transform::recomputeMatrix() const noexcept {
    const auto &quat = orientation();

    const auto translation_matrix = core::translate(core::Matrixf { 1.f }, m_position);
    const auto rotation_matrix    = core::Matrixf { quat };
    const auto scale_matrix       = core::scale(core::Matrixf { 1.f }, m_scale);

    m_transform_matrix = scale_matrix * rotation_matrix * translation_matrix;
}

////////////////////////////////////////
////////////////////////////////////////
void Transform::extract(const core::Matrixf &matrix) noexcept {
    m_position = core::Vector3f { core::row(matrix, 0)[3],
                                  core::row(matrix, 1)[3],
                                  core::row(matrix, 2)[3] };

    auto sx = core::length(core::row(matrix, 0));
    auto sy = core::length(core::row(matrix, 1));
    auto sz = core::length(core::row(matrix, 2));

    auto det = core::determinant(matrix);
    if (det < 0) sx = -sx;

    const auto inv_sx = 1.f / sx;
    const auto inv_sy = 1.f / sy;
    const auto inv_sz = 1.f / sz;

    auto _m = matrix;
    core::row(_m, 0)[0] *= inv_sx;
    core::row(_m, 0)[1] *= inv_sx;
    core::row(_m, 0)[2] *= inv_sx;

    core::row(_m, 1)[0] *= inv_sy;
    core::row(_m, 1)[1] *= inv_sy;
    core::row(_m, 1)[2] *= inv_sy;

    core::row(_m, 2)[0] *= inv_sz;
    core::row(_m, 2)[1] *= inv_sz;
    core::row(_m, 2)[2] *= inv_sz;

    const auto euler = core::eulerAngles(core::Quaternionf { _m });

    m_scale.x = sx;
    m_scale.y = sx;
    m_scale.z = sx;

    m_orientation_euler.x = core::degrees(euler.x);
    m_orientation_euler.y = core::degrees(euler.y);
    m_orientation_euler.z = core::degrees(euler.z);
}
