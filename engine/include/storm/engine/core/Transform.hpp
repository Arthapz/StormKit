// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Bindable.hpp>
#include <storm/engine/core/RingHardwareBuffer.hpp>

namespace storm::engine {
    struct TransformFlag {};
    class STORM_PUBLIC Transform: public StaticBindable<TransformFlag> {
      public:
        Transform(Engine &engine);
        ~Transform();

        Transform(Transform &&);
        Transform &operator=(Transform &&);

        [[nodiscard]] inline const core::Vector3f &position() const noexcept;

        inline void setPosition(const core::Vector2f &offset) noexcept;
        inline void setPosition(const core::Vector3f &offset) noexcept;
        inline void setPosition(float x, float y, float z = 0.f) noexcept;
        inline void setXPosition(float x) noexcept;
        inline void setYPosition(float y) noexcept;
        inline void setZPosition(float z) noexcept;

        inline void move(const core::Vector2f &offset) noexcept;
        inline void move(const core::Vector3f &offset) noexcept;
        inline void move(float x, float y, float z = 0.f) noexcept;
        inline void moveX(float x) noexcept;
        inline void moveY(float y) noexcept;
        inline void moveZ(float z) noexcept;

        [[nodiscard]] inline const core::Vector3f &scale() const noexcept;

        inline void setScale(const core::Vector3f &scale) noexcept;
        inline void setScale(const core::Vector2f &scale) noexcept;
        inline void setScale(float x_scale, float y_scale, float z_scale = 1.f) noexcept;
        inline void setXScale(float scale) noexcept;
        inline void setYScale(float scale) noexcept;
        inline void setZScale(float scale) noexcept;

        inline void scale(const core::Vector3f &scale) noexcept;
        inline void scale(const core::Vector2f &scale) noexcept;
        inline void scale(float x_scale, float y_scale, float z_scale = 0.f) noexcept;
        inline void scaleX(float scale) noexcept;
        inline void scaleY(float scale) noexcept;
        inline void scaleZ(float scale) noexcept;

        [[nodiscard]] inline const core::Quaternionf &orientation() const noexcept;
        [[nodiscard]] inline const core::Vector3f &orientationEuler() const noexcept;

        inline void setOrientation(const core::Vector3f &orientation) noexcept;
        inline void setOrientation(const core::Vector2f &orientation) noexcept;
        inline void setOrientation(float pitch, float yaw, float roll = 0.f) noexcept;
        inline void setYaw(float pitch) noexcept;
        inline void setPitch(float yaw) noexcept;
        inline void setRoll(float roll) noexcept;

        inline void rotate(const core::Vector3f &rotation) noexcept;
        inline void rotate(const core::Vector2f &rotation) noexcept;
        inline void rotate(float yaw, float pitch, float roll = 0.f) noexcept;
        inline void rotateYaw(float value) noexcept;
        inline void rotatePitch(float value) noexcept;
        inline void rotateRoll(float value) noexcept;

        inline void setMatrix(const core::Matrixf &matrix) noexcept;
        [[nodiscard]] inline const core::Matrixf &matrix() const noexcept;

        void flush() noexcept;

      private:
        void recomputeOrientation() const noexcept;
        void recomputeMatrix() const noexcept;
        void extract(const core::Matrixf &matrix) noexcept;

        EngineObserverPtr m_engine;

        core::Vector3f m_position          = core::Vector3f { 0.f, 0.f, 0.f };
        core::Vector3f m_orientation_euler = core::Vector3f { 0.f, 0.f, 0.f };
        core::Vector3f m_scale             = core::Vector3f { 1.f, 1.f, 1.f };

        mutable bool m_orientation_dirty         = true;
        mutable bool m_dirty                     = true;
        mutable bool m_is_updated                = true;
        mutable core::Quaternionf m_orientation  = core::Quaternionf { 0.f, 0.f, 0.f, 0.f };
        mutable core::Matrixf m_transform_matrix = core::Matrixf { 1.f };

        RingHardwareBufferOwnedPtr m_transform_buffer;
    };
} // namespace storm::engine

#include "Transform.inl"
