/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/scene/FPSCamera.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FPSCamera::FPSCamera(const Engine &engine, core::Extentf viewport)
    : Camera { engine, Camera::Type::Perspective, std::move(viewport) } {
}

////////////////////////////////////////
////////////////////////////////////////
FPSCamera::~FPSCamera() = default;

////////////////////////////////////////
////////////////////////////////////////
FPSCamera::FPSCamera(FPSCamera &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FPSCamera &FPSCamera::operator=(FPSCamera &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void FPSCamera::update(float delta) noexcept {
    static auto first_iteration = true;

    using storm::core::cos;
    using storm::core::radians;
    using storm::core::sin;

    const auto need_update =
        m_inputs.up || m_inputs.down || m_inputs.right || m_inputs.left || m_inputs.mouse_updated;
    if (!need_update && !first_iteration) return;

    if (first_iteration || m_inputs.mouse_ignore) {
        m_last_x_mouse = m_inputs.x_mouse;
        m_last_y_mouse = m_inputs.y_mouse;
    }

    const auto rotation_speed = delta * m_rotation_speed;
    if ((m_inputs.mouse_updated || first_iteration) && !m_inputs.mouse_ignore) {
        auto x_offset = m_last_x_mouse - m_inputs.x_mouse;
        auto y_offset = m_inputs.y_mouse - m_last_y_mouse;

        x_offset *= rotation_speed.x;
        y_offset *= rotation_speed.y;

        m_orientation.x += x_offset;
        m_orientation.y += y_offset;

        m_orientation.y = std::min(m_orientation.y, 89.f);
        m_orientation.y = std::max(m_orientation.y, -89.f);
    }

    auto front = core::Vector3f {};
    front.x    = cos(radians(m_orientation.x)) * cos(radians(m_orientation.y));
    front.y    = sin(radians(m_orientation.y));
    front.z    = sin(radians(m_orientation.x)) * cos(radians(m_orientation.y));
    m_front    = core::normalize(front);

    const auto move_speed = delta * m_move_speed;
    if (m_inputs.up) m_position += front * move_speed;
    if (m_inputs.down) m_position -= front * move_speed;
    if (m_inputs.left) m_position += core::normalize(core::cross(m_front, m_up)) * move_speed;
    if (m_inputs.right) m_position -= core::normalize(core::cross(m_front, m_up)) * move_speed;

    m_view_matrix = storm::core::lookAt(m_position, m_position + m_front, m_up);

    m_last_x_mouse = m_inputs.x_mouse;
    m_last_y_mouse = m_inputs.y_mouse;

    first_iteration = false;

    m_inputs = {};
}
