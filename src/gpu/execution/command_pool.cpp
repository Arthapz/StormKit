// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.execution;

import std;

import stormkit.core;

import stormkit.gpu.core;

using namespace std::literals;

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    auto CommandPoolInterface<Base>::create_vk_command_buffers(usize count, CommandBufferLevel level) const noexcept
      -> Expected<std::vector<VkCommandBuffer>> {
        const auto& device       = Base::owner();
        const auto& device_table = device.device_table();

        const auto allocate_info = VkCommandBufferAllocateInfo {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = *this,
            .level              = vk::to_vk<VkCommandBufferLevel>(level),
            .commandBufferCount = as<u32>(count)
        };

        return vk::allocate_checked<VkCommandBuffer>(count, device_table.vkAllocateCommandBuffers, device, &allocate_info);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    auto CommandPoolInterface<Base>::delete_vk_command_buffers(view::Device      device,
                                                               view::CommandPool pool,
                                                               VkCommandBuffer   cmb) noexcept -> void {
        const auto& device_table = device.device_table();
        vk::call(device_table.vkFreeCommandBuffers, device, pool, 1, &cmb);
    }

    template class CommandPoolInterface<CommandPoolImplementation>;
    template class CommandPoolInterface<view::CommandPoolImplementation>;

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandPoolImplementation::do_init(PrivateTag) noexcept -> Expected<void> {
        const auto& device       = owner();
        const auto& device_table = device.device_table();

        const auto create_info = VkCommandPoolCreateInfo {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = 0,
        };

        m_vk_handle = Try(vk::call_checked<VkCommandPool>(device_table.vkCreateCommandPool, device, &create_info, nullptr));
        Return {};
    }
} // namespace stormkit::gpu
