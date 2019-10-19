// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline void Scene::setCamera(Camera &camera) noexcept {
        m_camera = core::makeObserver(camera);

        const auto descriptors = std::array { render::Descriptor { m_camera->cameraDescriptor() } };
        m_camera_descriptor_set->update(descriptors);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline entities::EntityManager &Scene::entities() noexcept { return m_entities; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UOffset Scene::cameraOffset() const noexcept { return m_camera->cameraOffset(); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::DescriptorSet &Scene::cameraDescriptorSet() const noexcept {
        return *m_camera_descriptor_set;
    }
} // namespace storm::engine
