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
Camera::Camera(const Engine &engine, Type type, core::Extentf viewport)
    : m_engine { &engine }, m_type { type }, m_viewport { std::move(viewport) } {
    const auto buffering_count = m_engine->surface().bufferingCount();
    const auto &device         = m_engine->device();

    m_camera_buffer = std::make_unique<RingHardwareBuffer>(buffering_count,
                                                           device,
                                                           render::HardwareBufferUsage::Uniform,
                                                           sizeof(CameraData));

    m_camera_descriptor =
        render::BufferDescriptor { .type    = render::DescriptorType::Uniform_Buffer_Dynamic,
                                   .binding = 0u,
                                   .buffer  = core::makeConstObserver(m_camera_buffer->buffer()),
                                   .range   = sizeof(CameraData),
                                   .offset  = 0u };

    if (m_type == Type::Perspective)
        m_projection_matrix =
            core::perspective(m_fov, viewport.width / viewport.height, 0.1f, 100.f);
    else if (m_type == Type::Orthographic)
        m_projection_matrix = core::ortho(0.f, viewport.width, 0.f, viewport.height);

    updateBuffer();
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
void Camera::updateBuffer() noexcept {
    const auto data = CameraData { .position   = core::Vector4f { m_position, 0.f },
                                   .projection = m_projection_matrix,
                                   .view       = m_view_matrix };

    m_camera_buffer->next();
    m_camera_buffer->upload<CameraData>({ &data, 1 });
}