// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<Texture> Surface::textures() noexcept { return m_textures; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<const Texture> Surface::textures() const noexcept {
        return m_textures;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PixelFormat Surface::pixelFormat() const noexcept { return m_pixel_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Surface::bufferingCount() const noexcept { return m_buffering_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Surface::textureCount() const noexcept { return m_image_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Surface::needRecreate() const noexcept { return m_need_recreate; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Extentu &Surface::extent() const noexcept { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::SurfaceKHR Surface::vkSurface() const noexcept {
        STORM_EXPECTS(m_vk_surface);
        return *m_vk_surface;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Surface::operator vk::SurfaceKHR() const noexcept { return vkSurface(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::SurfaceKHR Surface::vkHandle() const noexcept { return vkSurface(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Surface::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkSurfaceKHR_T *());
    }
} // namespace storm::render
