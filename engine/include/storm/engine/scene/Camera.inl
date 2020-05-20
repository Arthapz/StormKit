// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
     Camera::Type Camera::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
     void Camera::setViewport(core::Extentf viewport) noexcept {
        m_viewport = std::move(viewport);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     const core::Extentf &Camera::viewport() const noexcept { return m_viewport; }

    /////////////////////////////////////
    /////////////////////////////////////
     void Camera::setPosition(core::Vector3f position) noexcept {
        m_position = std::move(position);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     const core::Vector3f &Camera::position() const noexcept { return m_position; }

    /////////////////////////////////////
    /////////////////////////////////////
     void Camera::setRotation(core::Vector3f orientation) noexcept {
        m_orientation = std::move(orientation);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     const core::Vector3f &Camera::rotation() const noexcept { return m_orientation; }

    /////////////////////////////////////
    /////////////////////////////////////
     const core::Matrixf &Camera::projectionMatrix() const noexcept {
        return m_projection_matrix;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     const core::Matrixf &Camera::viewMatrix() const noexcept { return m_view_matrix; }
} // namespace storm::engine
