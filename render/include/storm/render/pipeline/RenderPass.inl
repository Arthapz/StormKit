// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void RenderPass::addSubpass(Subpass subpass) {
        m_subpasses.emplace_back(std::move(subpass));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &RenderPass::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::span<const RenderPass::AttachmentDescription>
        RenderPass::attachmentDescriptions() const noexcept {
        return m_attachment_descriptions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::span<const RenderPass::Subpass> RenderPass::subpasses() const noexcept {
        return m_subpasses;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::RenderPass RenderPass::vkRenderPass() const noexcept {
        STORM_EXPECTS(m_vk_render_pass);
        return *m_vk_render_pass;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline RenderPass::operator vk::RenderPass() const noexcept { return vkRenderPass(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::RenderPass RenderPass::vkHandle() const noexcept { return vkRenderPass(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 RenderPass::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkRenderPass_T *());
    }
} // namespace storm::render
