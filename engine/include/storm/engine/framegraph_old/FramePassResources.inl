// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::TextureViewConstSpan FramePassResources::textureViews() const noexcept {
        return m_texture_views;
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::RenderPass &FramePassResources::renderpass() const noexcept {
        return *m_render_pass;
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Framebuffer &FramePassResources::framebuffer() const noexcept {
        return *m_framebuffer;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::DescriptorSetLayout &FramePassResources::descriptorSetLayout() const
        noexcept {
        return *m_descriptor_layout;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::DescriptorSet &FramePassResources::descriptorSet() const noexcept {
        return *m_descriptor_set;
    }
} // namespace storm::engine
