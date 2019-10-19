// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Fwd.hpp>

#include <storm/render/pipeline/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC DebugGUI: public core::NonCopyable {
      public:
        explicit DebugGUI(engine::Engine &engine);
        ~DebugGUI();

        DebugGUI(DebugGUI &&);
        DebugGUI &operator=(DebugGUI &&);

        void init(const render::RenderPass &render_pass);

        void update(const window::Window &window);
        void render(render::CommandBuffer &cmb, const render::RenderPass &render_pass);

      private:
        engine::EngineObserverPtr m_engine;

        render::CommandBufferOwnedPtrArray m_sub_command_buffers;

        core::UInt32 m_buffer_count;
        core::UInt32 m_current_buffer = 0;

        float m_current_cpu_time = 0.f;
        std::array<float, 20> m_frame_times;
        core::UInt8 m_frame_time_pointer = 0u;
        core::UInt32 m_max_fps           = 300u;
    };
} // namespace storm::engine

#include "DebugGUI.inl"
