// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <string_view>
#include <unordered_map>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/pipeline/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassResources: public core::NonCopyable {
      public:
        FramePassResources(FrameGraph &frame_graph,
                           const FramePassBase &pass,
                           const render::RenderPass &render_pass,
                           const render::Framebuffer &framebuffer);
        ~FramePassResources();

        FramePassResources(FramePassResources &&);
        FramePassResources &operator=(FramePassResources &&);

        template<typename Resource>
        [[nodiscard]] const Resource &get(FramePassResourceID<Resource> id) const noexcept;

        template<typename Resource>
        [[nodiscard]] const FramePassResource<Resource> &
            getResource(FramePassResourceID<Resource> id) const noexcept;

        template<typename Resource>
        [[nodiscard]] const typename Resource::Descriptor &
            getDescriptor(FramePassResourceID<Resource> id) const noexcept;

        [[nodiscard]] inline const render::RenderPass &renderPass() const noexcept;
        [[nodiscard]] inline const render::Framebuffer &framebuffer() const noexcept;

      private:
        FrameGraphObserverPtr m_frame_graph;
        FramePassBaseConstObserverPtr m_frame_pass;

        render::RenderPassConstObserverPtr m_render_pass;
        render::FramebufferConstObserverPtr m_framebuffer;
    };
} // namespace storm::engine

#include "FramePassResources.inl"
