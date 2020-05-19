// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline Camera::Type Camera::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Camera::setViewport(core::Extentf viewport) noexcept {
        m_viewport = std::move(viewport);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Extentf &Camera::viewport() const noexcept { return m_viewport; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Camera::setPosition(core::Vector3f position) noexcept {
        m_position = std::move(position);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Vector3f &Camera::position() const noexcept { return m_position; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Camera::setRotation(core::Vector3f orientation) noexcept {
        m_orientation = std::move(orientation);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Vector3f &Camera::rotation() const noexcept { return m_orientation; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Matrixf &Camera::projectionMatrix() const noexcept {
        return m_projection_matrix;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Matrixf &Camera::viewMatrix() const noexcept { return m_view_matrix; }
} // namespace storm::engine
