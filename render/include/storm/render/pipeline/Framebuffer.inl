// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Extentu &Framebuffer::extent() const noexcept { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &Framebuffer::device() const noexcept { return m_render_pass->device(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<const TextureViewConstObserverPtr> Framebuffer::attachments() const
        noexcept {
        return m_attachments;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Framebuffer Framebuffer::vkFramebuffer() const noexcept {
        STORM_EXPECTS(m_vk_framebuffer);
        return *m_vk_framebuffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Framebuffer::operator vk::Framebuffer() const noexcept { return vkFramebuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Framebuffer Framebuffer::vkHandle() const noexcept { return vkFramebuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Framebuffer::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkFramebuffer_T *());
    }
} // namespace storm::render
