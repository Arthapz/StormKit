// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::engine - ///////////
#include <storm/engine/scene/Camera.hpp>

namespace storm::engine {
    class STORM_PUBLIC FPSCamera: public Camera {
      public:
        struct Inputs {
            bool up    = false;
            bool down  = false;
            bool right = false;
            bool left  = false;

            bool mouse_updated = false;
            bool mouse_ignore  = false;
            float x_mouse      = 0.f;
            float y_mouse      = 0.f;
        };

        FPSCamera(const Engine &engine, core::Extentf viewport);
        ~FPSCamera() override;

        FPSCamera(FPSCamera &&);
        FPSCamera &operator=(FPSCamera &&);

        inline void feedInputs(Inputs inputs);
        void update(float delta) noexcept override;

      private:
        Inputs m_inputs;

        core::Vector3f m_front               = { 0.f, 0.f, 0.f };
        static constexpr core::Vector3f m_up = { 0.f, 1.f, 0.f };

        float m_last_x_mouse = 0.f;
        float m_last_y_mouse = 0.f;
    };
} // namespace storm::engine

#include "FPSCamera.inl"