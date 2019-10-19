// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <unordered_map>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/scene/Camera.hpp>

namespace storm::engine {
    class STORM_PUBLIC Scene: public core::NonCopyable {
      public:
        explicit Scene(const engine::Engine &engine);
        ~Scene();

        Scene(Scene &&);
        Scene &operator=(Scene &&);

        void update(float delta) noexcept;

        inline void setCamera(Camera &camera) noexcept;

        [[nodiscard]] inline entities::EntityManager &entities() noexcept;

        [[nodiscard]] inline core::UOffset cameraOffset() const noexcept;
        [[nodiscard]] inline render::DescriptorSet &cameraDescriptorSet() const noexcept;

      private:
        CameraOwnedPtr m_default_camera;

        EngineConstObserverPtr m_engine;

        entities::EntityManager m_entities;

        CameraObserverPtr m_camera;
        render::DescriptorSetOwnedPtr m_camera_descriptor_set;
    };
} // namespace storm::engine

#include "Scene.inl"
