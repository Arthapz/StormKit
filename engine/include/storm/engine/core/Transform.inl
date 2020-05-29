// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    const core::Vector3f &Transform::position() const noexcept { return m_position; }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setPosition(const core::Vector2f &position) noexcept {
        setXPosition(position.x);
        setYPosition(position.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setPosition(const core::Vector3f &position) noexcept {
        setXPosition(position.x);
        setYPosition(position.y);
        setZPosition(position.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setPosition(float x, float y, float z) noexcept {
        setXPosition(x);
        setYPosition(y);
        setZPosition(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setXPosition(float x) noexcept {
        m_position.x = x;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setYPosition(float y) noexcept {
        m_position.y = y;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setZPosition(float z) noexcept {
        m_position.z = z;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::move(const core::Vector2f &position) noexcept {
        moveX(position.x);
        moveY(position.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::move(const core::Vector3f &position) noexcept {
        moveX(position.x);
        moveY(position.y);
        moveZ(position.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::move(float x, float y, float z) noexcept {
        moveX(x);
        moveY(y);
        moveZ(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::moveX(float x) noexcept {
        m_position.x += x;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::moveY(float y) noexcept {
        m_position.y += y;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::moveZ(float z) noexcept {
        m_position.z += z;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Vector3f &Transform::scale() const noexcept { return m_scale; }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setScale(const core::Vector2f &scale) noexcept {
        setXScale(scale.x);
        setYScale(scale.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setScale(const core::Vector3f &scale) noexcept {
        setXScale(scale.x);
        setYScale(scale.y);
        setZScale(scale.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setScale(float x, float y, float z) noexcept {
        setXScale(x);
        setYScale(y);
        setZScale(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setXScale(float x) noexcept {
        m_scale.x = x;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setYScale(float y) noexcept {
        m_scale.y = y;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setZScale(float z) noexcept {
        m_scale.z = z;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::scale(const core::Vector3f &scale) noexcept {
        scaleX(scale.x);
        scaleY(scale.y);
        scaleZ(scale.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::scale(const core::Vector2f &scale) noexcept {
        scaleX(scale.x);
        scaleY(scale.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::scale(float x_scale, float y_scale, float z_scale) noexcept {
        scaleX(x_scale);
        scaleY(y_scale);
        scaleZ(z_scale);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::scaleX(float scale) noexcept {
        m_scale.x += scale;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::scaleY(float scale) noexcept {
        m_scale.y += scale;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::scaleZ(float scale) noexcept {
        m_scale.z += scale;

        m_is_updated = true;
        m_dirty      = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Quaternionf &Transform::orientation() const noexcept {
        if (m_orientation_dirty) {
            recomputeOrientation();
            m_orientation_dirty = false;
        }

        return m_orientation;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    const core::Vector3f &Transform::orientationEuler() const noexcept {
        return m_orientation_euler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setOrientation(const core::Vector3f &orientation) noexcept {
        setPitch(orientation.x);
        setYaw(orientation.y);
        setRoll(orientation.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setOrientation(const core::Vector2f &orientation) noexcept {
        setPitch(orientation.x);
        setYaw(orientation.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setOrientation(float pitch, float yaw, float roll) noexcept {
        setPitch(pitch);
        setYaw(yaw);
        setRoll(roll);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setYaw(float yaw) noexcept {
        m_orientation_euler.y = yaw;

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setPitch(float pitch) noexcept {
        m_orientation_euler.x = pitch;

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setRoll(float roll) noexcept {
        m_orientation_euler.z = roll;

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::rotate(const core::Vector3f &rotation) noexcept {
        rotatePitch(rotation.x);
        rotateYaw(rotation.y);
        rotateRoll(rotation.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::rotate(const core::Vector2f &rotation) noexcept {
        rotatePitch(rotation.x);
        rotateYaw(rotation.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::rotate(float pitch, float yaw, float roll) noexcept {
        rotatePitch(pitch);
        rotateYaw(yaw);
        rotateRoll(roll);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::rotateYaw(float yaw) noexcept {
        m_orientation_euler.y += yaw;

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::rotatePitch(float pitch) noexcept {
        m_orientation_euler.x += pitch;

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::rotateRoll(float roll) noexcept {
        m_orientation_euler.z += roll;

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void Transform::setMatrix(const core::Matrixf &matrix) noexcept {
        extract(matrix);

        m_is_updated        = true;
        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Matrixf &Transform::matrix() const noexcept {
        if (m_is_updated) {
            recomputeMatrix();
            m_is_updated = false;
        }

        return m_transform_matrix;
    }
} // namespace storm::engine
