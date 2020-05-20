// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
     void RenderPass::addSubpass(Subpass subpass) {
        m_subpasses.emplace_back(std::move(subpass));
    }

    /////////////////////////////////////
    /////////////////////////////////////
     const Device &RenderPass::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
     core::span<const RenderPass::AttachmentDescription>
        RenderPass::attachmentDescriptions() const noexcept {
        return m_attachment_descriptions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::span<const RenderPass::Subpass> RenderPass::subpasses() const noexcept {
        return m_subpasses;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     vk::RenderPass RenderPass::vkRenderPass() const noexcept {
        STORM_EXPECTS(m_vk_render_pass);
        return *m_vk_render_pass;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     RenderPass::operator vk::RenderPass() const noexcept { return vkRenderPass(); }

    /////////////////////////////////////
    /////////////////////////////////////
     vk::RenderPass RenderPass::vkHandle() const noexcept { return vkRenderPass(); }

    /////////////////////////////////////
    /////////////////////////////////////
     core::UInt64 RenderPass::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkRenderPass_T *());
    }
} // namespace storm::render
