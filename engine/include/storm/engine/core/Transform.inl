// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Transform.hpp"

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Vector3f &Transform::position() const noexcept { return m_position; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setPosition(const core::Vector2f &position) noexcept {
        setXPosition(position.x);
        setYPosition(position.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setPosition(const core::Vector3f &position) noexcept {
        setXPosition(position.x);
        setYPosition(position.y);
        setZPosition(position.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setPosition(float x, float y, float z) noexcept {
        setXPosition(x);
        setYPosition(y);
        setZPosition(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setXPosition(float x) noexcept {
        m_position.x = x;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setYPosition(float y) noexcept {
        m_position.y = y;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setZPosition(float z) noexcept {
        m_position.z = z;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::move(const core::Vector2f &position) noexcept {
        moveX(position.x);
        moveY(position.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::move(const core::Vector3f &position) noexcept {
        moveX(position.x);
        moveY(position.y);
        moveZ(position.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::move(float x, float y, float z) noexcept {
        moveX(x);
        moveY(y);
        moveZ(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::moveX(float x) noexcept {
        m_position.x += x;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::moveY(float y) noexcept {
        m_position.y += y;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::moveZ(float z) noexcept {
        m_position.z += z;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Vector3f &Transform::scale() const noexcept { return m_scale; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setScale(const core::Vector2f &scale) noexcept {
        setXScale(scale.x);
        setYScale(scale.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setScale(const core::Vector3f &scale) noexcept {
        setXScale(scale.x);
        setYScale(scale.y);
        setZScale(scale.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setScale(float x, float y, float z) noexcept {
        setXScale(x);
        setYScale(y);
        setZScale(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setXScale(float x) noexcept {
        m_scale.x = x;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setYScale(float y) noexcept {
        m_scale.y = y;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setZScale(float z) noexcept {
        m_scale.z = z;

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::Quaternionf Transform::orientation() const noexcept {
        return m_yaw * m_pitch * m_roll;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setOrientation(const core::Vector3f &orientation) noexcept {
        setYaw(orientation.x);
        setPitch(orientation.y);
        setRoll(orientation.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setOrientation(float yaw, float pitch, float roll) noexcept {
        setYaw(yaw);
        setPitch(pitch);
        setRoll(roll);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setYaw(float yaw) noexcept {
        m_yaw = core::angleAxis(core::radians(yaw), core::Vector3f { 1.f, 0.f, 0.f });

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setPitch(float pitch) noexcept {
        m_pitch = core::angleAxis(core::radians(pitch), core::Vector3f { 0.f, 1.f, 0.f });

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::setRoll(float roll) noexcept {
        m_roll = core::angleAxis(core::radians(roll), core::Vector3f { 0.f, 0.f, 1.f });

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::rotate(const core::Vector3f &rotation) noexcept {
        rotateYaw(rotation.x);
        rotatePitch(rotation.y);
        rotateRoll(rotation.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::rotateYaw(float yaw) noexcept {
        m_yaw = core::rotate(m_yaw, core::radians(yaw), core::Vector3f { 1.f, 0.f, 0.f });

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::rotatePitch(float pitch) noexcept {
        m_pitch = core::rotate(m_pitch, core::radians(pitch), core::Vector3f { 0.f, 1.f, 0.f });

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline void Transform::rotateRoll(float roll) noexcept {
        m_roll = core::rotate(m_roll, core::radians(roll), core::Vector3f { 0.f, 0.f, 1.f });

        m_is_updated = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Matrixf &Transform::matrix() const noexcept {
        if (m_is_updated) recomputeMatrix();

        return m_transform_matrix;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UOffset Transform::transformOffset() const noexcept {
        return m_transform_buffer->currentOffset();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const render::BufferDescriptor &Transform::transformDescriptor() const noexcept {
        return m_transform_descriptor;
    }
} // namespace storm::engine
