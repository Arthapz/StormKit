
// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.resource;

import std;

import stormkit.core;

import stormkit.gpu.core;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    auto ImageView::do_init(PrivateTag,
                            view::Image                  image,
                            ImageViewType                type,
                            const ImageSubresourceRange& subresource_range) noexcept -> Expected<void> {
        m_type              = type;
        m_subresource_range = subresource_range;

        const auto vk_subresource_range = VkImageSubresourceRange {
            .aspectMask     = vk::to_vk<VkImageAspectFlags>(m_subresource_range.aspect_mask),
            .baseMipLevel   = m_subresource_range.base_mip_level,
            .levelCount     = m_subresource_range.level_count,
            .baseArrayLayer = m_subresource_range.base_array_layer,
            .layerCount     = m_subresource_range.layer_count,
        };

        const auto create_info = VkImageViewCreateInfo {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .image            = image,
            .viewType         = vk::to_vk<VkImageViewType>(m_type),
            .format           = vk::to_vk<VkFormat>(image.format()),
            .components       = { .r = VK_COMPONENT_SWIZZLE_R,
                                 .g = VK_COMPONENT_SWIZZLE_G,
                                 .b = VK_COMPONENT_SWIZZLE_B,
                                 .a = VK_COMPONENT_SWIZZLE_A },
            .subresourceRange = vk_subresource_range,
        };

        const auto& device = this->device();

        m_vk_handle = Try(vk::call_checked<VkImageView>(device.device_table().vkCreateImageView, device, &create_info, nullptr));
        Return {};
    }
} // namespace stormkit::gpu
