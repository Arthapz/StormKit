/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Camera.hpp>

using namespace storm;
using namespace storm::engine;

struct alignas(16) CameraData {
    core::Vector4f position  = core::Vector4f { 0.f, 0.f, 0.f, 0.f };
    core::Matrixf projection = core::Matrixf { 1.f };
    core::Matrixf view       = core::Matrixf { 1.f };
};

////////////////////////////////////////
////////////////////////////////////////
Camera::Camera(const Engine &engine, Type type, core::Extentf viewport, float znear, float zfar)
    : m_engine { &engine }, m_type { type }, m_viewport { std::move(viewport) } {
    const auto buffering_count = m_engine->surface().bufferingCount();
    const auto &device         = m_engine->device();
    const auto &pool           = m_engine->descriptorPool();

    m_camera_buffer = std::make_unique<RingHardwareBuffer>(buffering_count,
                                                           device,
                                                           render::HardwareBufferUsage::Uniform,
                                                           sizeof(CameraData));
    device.setObjectName(m_camera_buffer->buffer(), "StormKit:CameraBuffer");

    const auto descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_camera_buffer->buffer()),
                                   .range   = sizeof(CameraData),
                                   .offset  = 0u };
    const auto descriptors =
        render::DescriptorStaticArray<1> { render::Descriptor { std::move(descriptor) } };

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());

    for (auto &set : m_descriptor_sets) device.setObjectName(*set, "StormKit:CameraDescriptorSet");

    descriptorSet().update(descriptors);

    if (m_type == Type::Perspective)
        m_projection_matrix =
            core::perspective(m_fov, viewport.width / viewport.height, znear, zfar);
    else if (m_type == Type::Orthographic)
        m_projection_matrix = core::ortho(0.f, viewport.width, 0.f, viewport.height, znear, zfar);

    flush();
}

////////////////////////////////////////
////////////////////////////////////////
Camera::~Camera() = default;

////////////////////////////////////////
////////////////////////////////////////
Camera::Camera(Camera &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Camera &Camera::operator=(Camera &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Camera::update(float delta) noexcept {
}

////////////////////////////////////////
////////////////////////////////////////
void Camera::flush() noexcept {
    using storm::core::cos;
    using storm::core::radians;
    using storm::core::sin;

    /*const auto position = core::Vector4f {
        -m_position.z * sin(core::radians(m_orientation.y)) *
    cos(core::radians(m_orientation.x)), -m_position.z *
    sin(radians(m_orientation.x)), m_position.z * cos(radians(m_orientation.y)) *
    cos(radians(m_orientation.x)), 0.f
    };*/

    const auto position = core::Vector4f { m_position, 0.f };

    const auto data = CameraData { .position   = position,
                                   .projection = m_projection_matrix,
                                   .view       = m_view_matrix };

    m_camera_buffer->next();
    m_camera_buffer->upload<CameraData>({ &data, 1 });

    m_offset = m_camera_buffer->currentOffset();
}
