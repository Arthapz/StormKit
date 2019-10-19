// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>

/////////// - StormKit::render - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/RingHardwareBuffer.hpp>

namespace storm::engine {
    class STORM_PUBLIC Camera: public core::NonCopyable {
      public:
        enum class Type { Perspective, Orthographic };

        Camera(const Engine &engine, Type type, core::Extentf viewport);
        virtual ~Camera();

        Camera(Camera &&);
        Camera &operator=(Camera &&);

        [[nodiscard]] inline Type type() const noexcept;

        inline void setViewport(core::Extentf viewport) noexcept;
        [[nodiscard]] inline const core::Extentf &viewport() const noexcept;

        inline void setPosition(core::Vector3f position) noexcept;
        [[nodiscard]] inline const core::Vector3f &position() const noexcept;

        inline void setRotation(core::Vector3f orientation) noexcept;
        [[nodiscard]] inline const core::Vector3f &rotation() const noexcept;

        virtual void update(float delta) noexcept;

        [[nodiscard]] inline const core::Matrixf &projectionMatrix() const noexcept;
        [[nodiscard]] inline const core::Matrixf &viewMatrix() const noexcept;

        [[nodiscard]] inline core::UOffset cameraOffset() const noexcept;
        [[nodiscard]] inline const render::BufferDescriptor &cameraDescriptor() const noexcept;

      protected:
        void updateBuffer() noexcept;

        EngineConstObserverPtr m_engine;

        Type m_type;

        float m_near_plane = 0.1f;
        float m_far_plane  = 1000.f;
        float m_fov        = core::radians(90.f);

        core::Extentf m_viewport = { 0.f, 0.f };

        core::Vector3f m_orientation = { 0.f, 0.f, 0.f };
        core::Vector3f m_position    = { 0.f, 0.f, 0.f };

        core::Vector3f m_move_speed     = { 8.f, 8.f, 8.f };
        core::Vector3f m_rotation_speed = { 8.f, 8.f, 8.f };

        core::Matrixf m_projection_matrix = core::Matrixf { 1.f };
        core::Matrixf m_view_matrix       = core::Matrixf { 1.f };

        render::BufferDescriptor m_camera_descriptor;
        RingHardwareBufferOwnedPtr m_camera_buffer;
    };
} // namespace storm::engine

#include "Camera.inl"