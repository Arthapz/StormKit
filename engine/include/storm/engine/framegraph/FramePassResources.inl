// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    const Resource &FramePassResources::get(FramePassResourceID<Resource> id) const noexcept {
        return m_frame_graph->get<Resource>(id);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    const typename Resource::Descriptor &
        FramePassResources::getDescriptor(FramePassResourceID<Resource> id) const noexcept {
        return getResource(id).descriptor();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    const FramePassResource<Resource> &
        FramePassResources::getResource(FramePassResourceID<Resource> id) const noexcept {
        return m_frame_graph->getResource<Resource>(id);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     const render::RenderPass &FramePassResources::renderPass() const noexcept {
        return *m_render_pass;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
     const render::Framebuffer &FramePassResources::framebuffer() const noexcept {
        return *m_framebuffer;
    }

} // namespace storm::engine
