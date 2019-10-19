// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline core::Extentu Texture::extent() const noexcept { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PixelFormat Texture::format() const noexcept { return m_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline TextureType Texture::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline SampleCountFlag Texture::samples() const noexcept { return m_samples; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Texture::mipLevels() const noexcept { return m_mip_levels; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Texture::layers() const noexcept { return m_layers; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &Texture::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Image Texture::vkImage() const noexcept {
        if (m_non_owning_texture) { return m_non_owning_texture; }

        STORM_EXPECTS(m_vk_texture);
        return *m_vk_texture;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Texture::operator vk::Image() const noexcept { return vkImage(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Image Texture::vkHandle() const noexcept { return vkImage(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Texture::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkImage_T *());
    }
} // namespace storm::render
