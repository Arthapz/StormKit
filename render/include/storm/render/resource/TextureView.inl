// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline const Texture &TextureView::texture() const noexcept {
        // STORM_EXPECTS(m_texture != nullptr);

        return *m_texture;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline TextureViewType TextureView::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const TextureSubresourceRange &TextureView::subresourceRange() const noexcept {
        return m_subresource_range;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::ImageView TextureView::vkImageView() const noexcept {
        STORM_EXPECTS(m_vk_image_view);
        return *m_vk_image_view;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline TextureView::operator vk::ImageView() const noexcept { return vkImageView(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::ImageView TextureView::vkHandle() const noexcept { return vkImageView(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 TextureView::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkImageView_T *());
    }
} // namespace storm::render
