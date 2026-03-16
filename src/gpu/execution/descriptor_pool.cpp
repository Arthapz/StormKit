
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

namespace stdr = std::ranges;
namespace stdv = std::views;

using namespace std::literals;

namespace stormkit::gpu {
    namespace {
        struct DescriptorPoolAPI {
            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView DescriptorPoolType>
            static auto create_vk_descriptor_sets(const DescriptorPoolType&   descriptor_pool,
                                                  usize                       count,
                                                  view::DescriptorSetLayout&& layout) noexcept
              -> Expected<std::vector<VkDescriptorSet>> {
                const auto vk_layout     = vk::to_vk(layout);
                const auto allocate_info = VkDescriptorSetAllocateInfo {
                    .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                    .pNext              = nullptr,
                    .descriptorPool     = descriptor_pool,
                    .descriptorSetCount = as<u32>(count),
                    .pSetLayouts        = &vk_layout,
                };

                const auto& device       = descriptor_pool.device();
                const auto& device_table = device.device_table();

                return vk::allocate_checked<VkDescriptorSet>(count,
                                                             device_table.vkAllocateDescriptorSets,
                                                             device,
                                                             &allocate_info);
            }

            /////////////////////////////////////
            /////////////////////////////////////
            static auto delete_vk_descriptor_set(view::Device&&, view::DescriptorPool&&, VkDescriptorSet) -> void {}
        };
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::do_init(PrivateTag, std::span<const Size>&& sizes, u32 max_sets) noexcept -> Expected<void> {
        const auto pool_sizes = transform(sizes, [](const Size& size) static noexcept {
            return VkDescriptorPoolSize {
                .type            = vk::to_vk<VkDescriptorType>(size.type),
                .descriptorCount = size.descriptor_count,
            };
        });

        const auto create_info = VkDescriptorPoolCreateInfo {
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext         = nullptr,
            .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets       = max_sets,
            .poolSizeCount = as<u32>(stdr::size(sizes)),
            .pPoolSizes    = stdr::data(pool_sizes),
        };

        const auto& device       = this->device();
        const auto& device_table = device.device_table();

        m_vk_handle = Try(vk::call_checked<VkDescriptorPool>(device_table.vkCreateDescriptorPool, device, &create_info, nullptr));
        Return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::create_vk_descriptor_sets(usize count, view::DescriptorSetLayout&& layout) const noexcept
      -> Expected<std::vector<VkDescriptorSet>> {
        return DescriptorPoolAPI::create_vk_descriptor_sets(*this, count, std::move(layout));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::delete_vk_descriptor_set(view::Device         device,
                                                  view::DescriptorPool command_pool,
                                                  VkDescriptorSet      descriptor_set) noexcept -> void {
        DescriptorPoolAPI::delete_vk_descriptor_set(std::move(device), std::move(command_pool), descriptor_set);
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        auto DescriptorPool::create_vk_descriptor_sets(usize count, DescriptorSetLayout&& layout) const noexcept
          -> Expected<std::vector<VkDescriptorSet>> {
            return DescriptorPoolAPI::create_vk_descriptor_sets(*this, count, std::move(layout));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto DescriptorPool::delete_vk_descriptor_set(Device          device,
                                                      DescriptorPool  command_pool,
                                                      VkDescriptorSet descriptor_set) noexcept -> void {
            DescriptorPoolAPI::delete_vk_descriptor_set(std::move(device), std::move(command_pool), descriptor_set);
        }
    } // namespace view
} // namespace stormkit::gpu
