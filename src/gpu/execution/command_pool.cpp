// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.execution;

import std;

import stormkit.core;

import stormkit.gpu.core;

using namespace std::literals;

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace stormkit::gpu {
    namespace {
        struct CommandPoolAPI {
            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView CommandPoolType>
            static auto create_vk_command_buffers(const CommandPoolType& pool, usize count, CommandBufferLevel level)
              const noexcept -> Expected<std::vector<VkCommandBuffer>> {
                const auto& device       = cmb.device();
                const auto& device_table = device.device_table();

                const auto allocate_info = VkCommandBufferAllocateInfo {
                    .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                    .pNext              = nullptr,
                    .commandPool        = pool,
                    .level              = vk::to_vk<VkCommandBufferLevel>(level),
                    .commandBufferCount = as<u32>(count)
                };

                return vk::allocate_checked<VkCommandBuffer>(count,
                                                             device_table.vkAllocateCommandBuffers,
                                                             device,
                                                             &allocate_info);
            }

            /////////////////////////////////////
            /////////////////////////////////////
            static auto delete_vk_command_buffers(Device          device,
                                                  CommandPool     command_pool,
                                                  VkCommandBuffer command_buffer) noexcept -> void {
                vk::call(device_table.vkFreeCommandBuffers, device, command_pool, 1, &command_buffer);
            }
        };
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandPool::do_init() noexcept -> Expected<void> {
        const auto& device       = device();
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

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandPool::create_vk_command_buffers(usize count, CommandBufferLevel level) const noexcept
      -> Expected<std::vector<VkCommandBuffer>> {
        return CommandPoolAPI::create_vk_command_buffers(*this, count, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandPool::delete_vk_command_buffers(Device device, CommandPool pool, VkCommandBuffer cmb) noexcept -> void {
        return CommandPoolAPI::create_vk_command_buffers(std::move(device), std::move(pool), cmb);
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        auto CommandPool::create_vk_command_buffers(usize count, CommandBufferLevel level) const noexcept
          -> Expected<std::vector<VkCommandBuffer>> {
            return CommandPoolAPI::create_vk_command_buffers(*this, count, level);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto CommandPool::delete_vk_command_buffers(Device device, CommandPool pool, VkCommandBuffer cmb) noexcept -> void {
            return CommandPoolAPI::create_vk_command_buffers(std::move(device), std::move(pool), cmb);
        }
    } // namespace view
} // namespace stormkit::gpu
