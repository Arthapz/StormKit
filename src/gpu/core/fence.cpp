// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.core;

import std;

import stormkit.core;

import :vulkan;

namespace stormkit::gpu {
    namespace {
        struct FenceAPI {
            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView FenceType>
            static auto status(const FenceType& fence) noexcept -> Expected<Fence::Status> {
                const auto& device = fence.device();

                const auto
                  result = Try((vk::call_checked<VkResult, VK_NOT_READY>(device.device_table().vkGetFenceStatus, device, fence)));
                if (result == VK_NOT_READY) Return Fence::Status::UNSIGNALED;
                Return Fence::Status::SIGNALED;
            }

            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView FenceType>
            static auto wait(const FenceType& fence, const std::chrono::milliseconds& wait_for) noexcept -> Expected<Result> {
                const auto& device = fence.device();
                const auto  handle = fence.native_handle();

                const auto
                  result = Try((vk::call_checked<VkResult,
                                                 VK_NOT_READY>(device.device_table().vkWaitForFences,
                                                               device,
                                                               1u,
                                                               &handle,
                                                               true,
                                                               std::chrono::duration_cast<std::chrono::nanoseconds>(wait_for)
                                                                 .count())));

                Return vk::from_vk<Result>(result);
            }

            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView FenceType>
            static auto reset(const FenceType& fence) noexcept -> Expected<void> {
                const auto& device = fence.device();
                const auto  handle = fence.native_handle();

                Try(vk::call_checked(device.device_table().vkResetFences, device, 1u, &handle));

                Return {};
            }
        };
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::status() const noexcept -> Expected<Status> {
        return FenceAPI::status(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::wait(const std::chrono::milliseconds& wait_for) const noexcept -> Expected<Result> {
        return FenceAPI::wait(*this, wait_for);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::reset() const noexcept -> Expected<void> {
        return FenceAPI::reset(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::do_init(PrivateTag, bool signaled) noexcept -> Expected<void> {
        const auto flags = (signaled) ? VkFenceCreateFlags { VK_FENCE_CREATE_SIGNALED_BIT } : VkFenceCreateFlags {};

        const auto create_info = VkFenceCreateInfo { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = flags };

        m_vk_handle = Try(vk::call_checked<VkFence>(m_device.device_table().vkCreateFence, m_device, &create_info, nullptr));

        Return {};
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        auto Fence::status() const noexcept -> Expected<gpu::Fence::Status> {
            return FenceAPI::status(*this);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Fence::wait(const std::chrono::milliseconds& wait_for) const noexcept -> Expected<Result> {
            return FenceAPI::wait(*this, wait_for);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Fence::reset() const noexcept -> Expected<void> {
            return FenceAPI::reset(*this);
        }
    } // namespace view
} // namespace stormkit::gpu
