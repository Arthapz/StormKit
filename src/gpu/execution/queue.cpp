// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.execution;

import std;

import stormkit.core;

import stormkit.gpu.core;

using namespace std::literals;

namespace stdr = std::ranges;
namespace stdv = std::views;
namespace stdp = std::pmr;

namespace stormkit::gpu {
    namespace {
        struct QueueAPI {
            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView QueueType>
            static auto wait_idle(const QueueType& queue) noexcept -> Expected<void> {
                const auto& device       = queue.device();
                const auto& device_table = device.device_table();

                return vk::call_checked(device_table.vkQueueWaitIdle, queue);
            }

            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView QueueType>
            static auto submit(const QueueType&                     queue,
                               std::span<const Queue::SubmitInfo>&& submit_infos,
                               std::optional<const view::Fence>&&   fence) noexcept -> Expected<void> {
                struct SubmitInfoRange {
                    std::span<const VkSemaphore>          wait_semaphores;
                    std::span<const VkPipelineStageFlags> wait_dst_stages;
                    std::span<const VkCommandBuffer>      command_buffers;
                    std::span<const VkSemaphore>          signal_semaphores;
                };

                const auto bytes_count = [&submit_infos] noexcept {
                    auto _wait_semaphores_count   = 0uz;
                    auto _wait_dst_stages_count   = 0uz;
                    auto _command_buffers_count   = 0uz;
                    auto _signal_semaphores_count = 0uz;

                    for (auto&& submit_info : submit_infos) {
                        _wait_semaphores_count   = stdr::size(submit_info.wait_semaphores);
                        _wait_dst_stages_count   = stdr::size(submit_info.wait_dst_stages);
                        _command_buffers_count   = stdr::size(submit_info.command_buffers);
                        _signal_semaphores_count = stdr::size(submit_info.signal_semaphores);
                    }
                    return _wait_semaphores_count * sizeof(VkSemaphore)
                           + _wait_dst_stages_count * sizeof(VkPipelineStageFlags)
                           + _command_buffers_count * sizeof(VkCommandBuffer)
                           + _signal_semaphores_count * sizeof(VkSemaphore)
                           + stdr::size(submit_infos) * sizeof(SubmitInfoRange);
                }();

                auto memory_resource = stdp::monotonic_buffer_resource { bytes_count };

                auto wait_semaphores_buf = stdp::vector<stdp::vector<VkSemaphore>> { &memory_resource };
                wait_semaphores_buf.reserve(stdr::size(submit_infos));
                auto wait_dst_stages_buf = stdp::vector<stdp::vector<VkPipelineStageFlags>> { &memory_resource };
                wait_dst_stages_buf.reserve(stdr::size(submit_infos));
                auto command_buffers_buf = stdp::vector<stdp::vector<VkCommandBuffer>> { &memory_resource };
                command_buffers_buf.reserve(stdr::size(submit_infos));
                auto signal_semaphores_buf = stdp::vector<stdp::vector<VkSemaphore>> { &memory_resource };
                signal_semaphores_buf.reserve(stdr::size(submit_infos));

                const auto submit_ranges = [&] noexcept {
                    auto vec = stdp::vector<SubmitInfoRange> { &memory_resource };
                    vec.reserve(stdr::size(submit_infos));
                    for (auto&& submit_info : submit_infos) {
                        auto& wait_semaphores = wait_semaphores_buf.emplace_back(std::from_range,
                                                                                 submit_info.wait_semaphores
                                                                                   | stdv::transform(vk::monadic::to_vk()));

                        auto&
                          wait_dst_stages = wait_dst_stages_buf
                                              .emplace_back(std::from_range,
                                                            submit_info.wait_dst_stages
                                                              | stdv::transform(vk::monadic::to_vk<VkPipelineStageFlagBits>()));

                        auto& command_buffers = command_buffers_buf.emplace_back(std::from_range,
                                                                                 submit_info.command_buffers
                                                                                   | stdv::transform(vk::monadic::to_vk()));

                        auto& signal_semaphores = signal_semaphores_buf.emplace_back(std::from_range,
                                                                                     submit_info.signal_semaphores
                                                                                       | stdv::transform(vk::monadic::to_vk()));

                        vec.emplace_back(SubmitInfoRange {
                          .wait_semaphores   = wait_semaphores,
                          .wait_dst_stages   = wait_dst_stages,
                          .command_buffers   = command_buffers,
                          .signal_semaphores = signal_semaphores,
                        });
                    }
                    return vec;
                }();

                const auto vk_submit_infos = submit_ranges
                                             | stdv::transform([](auto&& submit_range) noexcept {
                                                   EXPECTS(stdr::size(submit_range.wait_semaphores)
                                                           == stdr::size(submit_range.wait_dst_stages));

                                                   return VkSubmitInfo {
                                                       .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                                       .pNext                = nullptr,
                                                       .waitSemaphoreCount   = as<u32>(stdr::size(submit_range.wait_semaphores)),
                                                       .pWaitSemaphores      = stdr::data(submit_range.wait_semaphores),
                                                       .pWaitDstStageMask    = stdr::data(submit_range.wait_dst_stages),
                                                       .commandBufferCount   = as<u32>(stdr::size(submit_range.command_buffers)),
                                                       .pCommandBuffers      = stdr::data(submit_range.command_buffers),
                                                       .signalSemaphoreCount = as<u32>(stdr::size(submit_range
                                                                                                    .signal_semaphores)),
                                                       .pSignalSemaphores    = stdr::data(submit_range.signal_semaphores),
                                                   };
                                               })
                                             | stdr::to<std::vector>();

                const auto vk_fence = either(fence, vk::monadic::to_vk(), core::monadic::init<VkFence>(VK_NULL_HANDLE));

                const auto& device       = queue.device();
                const auto& device_table = device.device_table();
                return vk::call_checked(device_table.vkQueueSubmit,
                                        queue,
                                        stdr::size(vk_submit_infos),
                                        stdr::data(vk_submit_infos),
                                        vk_fence);
            }

            /////////////////////////////////////
            /////////////////////////////////////
            template<meta::IsOwnedOrView QueueType>
            static auto present(const QueueType&                 queue,
                                std::span<const view::SwapChain> swapchains,
                                std::span<const view::Semaphore> wait_semaphores,
                                std::span<const u32>             image_indices) noexcept -> Expected<Result> {
                EXPECTS(stdr::size(wait_semaphores) >= 1);
                EXPECTS(stdr::size(image_indices) >= 1);

                const auto swapchains_count      = stdr::size(swapchains);
                const auto wait_semaphores_count = stdr::size(wait_semaphores);

                const auto bytes_count = swapchains_count * sizeof(VkSwapchainKHR) + wait_semaphores_count * sizeof(VkSemaphore);
                auto       memory_resource = stdp::monotonic_buffer_resource { bytes_count };

                const auto vk_swapchains = stdp::vector<VkSwapchainKHR> { std::from_range,
                                                                          swapchains | stdv::transform(vk::monadic::to_vk()),
                                                                          &memory_resource };
                const auto vk_semaphores = stdp::vector<VkSemaphore> { std::from_range,
                                                                       wait_semaphores | stdv::transform(vk::monadic::to_vk()),
                                                                       &memory_resource };

                const auto present_info = VkPresentInfoKHR {
                    .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                    .pNext              = nullptr,
                    .waitSemaphoreCount = as<u32>(stdr::size(vk_semaphores)),
                    .pWaitSemaphores    = stdr::data(vk_semaphores),
                    .swapchainCount     = as<u32>(stdr::size(vk_swapchains)),
                    .pSwapchains        = stdr::data(vk_swapchains),
                    .pImageIndices      = stdr::data(image_indices),
                    .pResults           = nullptr,
                };

                const auto& device       = queue.device();
                const auto& device_table = device.device_table();
                const auto
                       result = Try((vk::call_checked<VkResult, VK_ERROR_OUT_OF_DATE_KHR, VK_SUBOPTIMAL_KHR>(device_table
                                                                                                               .vkQueuePresentKHR,
                                                                                                             queue,
                                                                                                             &present_info)));
                Return vk::from_vk<Result>(result);
            }
        };
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::wait_idle() const noexcept -> Expected<void> {
        return QueueAPI::wait_idle(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::present(std::span<const view::SwapChain> swapchains,
                        std::span<const view::Semaphore> wait_semaphores,
                        std::span<const u32>             image_indices) const noexcept -> Expected<Result> {
        return QueueAPI::present(*this, std::move(swapchains), std::move(wait_semaphores), std::move(image_indices));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::submit(std::span<const SubmitInfo> submit_infos, std::optional<view::Fence> fence) const noexcept
      -> Expected<void> {
        return QueueAPI::submit(*this, std::move(submit_infos), std::move(fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::do_init(PrivateTag, const Device::QueueEntry& entry) -> void {
        m_entry = entry;

        const auto& device       = this->device();
        const auto& device_table = device.device_table();
        m_vk_handle              = vk::call<VkQueue>(device_table.vkGetDeviceQueue, device, m_entry.id, 0);
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        auto Queue::wait_idle() const noexcept -> Expected<void> {
            return QueueAPI::wait_idle(*this);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Queue::present(std::span<const view::SwapChain> swapchains,
                            std::span<const view::Semaphore> wait_semaphores,
                            std::span<const u32>             image_indices) const noexcept -> Expected<Result> {
            return QueueAPI::present(*this, std::move(swapchains), std::move(wait_semaphores), std::move(image_indices));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Queue::submit(std::span<const gpu::Queue::SubmitInfo> submit_infos, std::optional<Fence> fence) const noexcept
          -> Expected<void> {
            return QueueAPI::submit(*this, std::move(submit_infos), std::move(fence));
        }
    } // namespace view
} // namespace stormkit::gpu
