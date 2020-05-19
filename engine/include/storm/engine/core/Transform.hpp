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
        inline void setPosition(float x, float y, float z) noexcept;
        inline void setXPosition(float x) noexcept;
        inline void setYPosition(float y) noexcept;
        inline void setZPosition(float z) noexcept;

        inline void move(const core::Vector2f &offset) noexcept;
        inline void move(const core::Vector3f &offset) noexcept;
        inline void move(float x, float y, float z) noexcept;
        inline void moveX(float x) noexcept;
        inline void moveY(float y) noexcept;
        inline void moveZ(float z) noexcept;

        [[nodiscard]] inline const core::Vector3f &scale() const noexcept;

        inline void setScale(const core::Vector3f &scale) noexcept;
        inline void setScale(const core::Vector2f &scale) noexcept;
        inline void setScale(float x_scale, float y_scale, float z_scale) noexcept;
        inline void setXScale(float scale) noexcept;
        inline void setYScale(float scale) noexcept;
        inline void setZScale(float scale) noexcept;

        [[nodiscard]] inline core::Quaternionf orientation() const noexcept;

        inline void setOrientation(const core::Vector3f &orientation) noexcept;
        inline void setOrientation(float yaw, float pitch, float roll) noexcept;
        inline void setYaw(float yaw) noexcept;
        inline void setPitch(float pitch) noexcept;
        inline void setRoll(float roll) noexcept;

        inline void rotate(const core::Vector3f &rotation) noexcept;
        inline void rotateYaw(float value) noexcept;
        inline void rotatePitch(float value) noexcept;
        inline void rotateRoll(float value) noexcept;

        [[nodiscard]] inline const core::Matrixf &matrix() const noexcept;

        void flush() noexcept;

      private:
        void recomputeMatrix() const noexcept;

        EngineObserverPtr m_engine;

        core::Vector3f m_position = core::Vector3f { 0.f, 0.f, 0.f };
        core::Quaternionf m_yaw   = core::Quaternionf { 1.f, 0.f, 0.f, 0.f };
        core::Quaternionf m_pitch = core::Quaternionf { 1.f, 0.f, 0.f, 0.f };
        core::Quaternionf m_roll  = core::Quaternionf { 1.f, 0.f, 0.f, 0.f };
        core::Vector3f m_scale    = core::Vector3f { 1.f, 1.f, 1.f };

        mutable bool m_is_updated                = true;
        mutable core::Matrixf m_transform_matrix = core::Matrixf { 1.f };

        RingHardwareBufferOwnedPtr m_transform_buffer;
    };
} // namespace storm::engine

#include "Transform.inl"
