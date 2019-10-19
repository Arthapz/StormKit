
// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <array>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Queue::Queue(const render::Device &device, render::QueueFlag flags,
			 std::uint32_t family_index, VkQueue queue)
	: m_device{&device}, m_queue_flag{flags}, m_family_index{family_index}, m_vk_queue{
																	  queue} {
	const auto &device_ = static_cast<const Device &>(*m_device);
	const auto command_pool_create_info = VkCommandPoolCreateInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
				 VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		.queueFamilyIndex = m_family_index};

	m_vk_command_pool = device_.createVkCommandPool(command_pool_create_info);
}

/////////////////////////////////////
/////////////////////////////////////
Queue::~Queue() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_command_pool != VK_NULL_HANDLE)
		device.destroyVkCommandPool(m_vk_command_pool);
}

/////////////////////////////////////
/////////////////////////////////////
Queue::Queue(Queue &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Queue &Queue::operator=(Queue &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Queue::waitIdle() const noexcept {
	STORM_EXPECTS(m_vk_queue != VK_NULL_HANDLE);

	const auto &device = static_cast<const Device &>(*m_device);

	device.waitIdle(m_vk_queue);
}

/////////////////////////////////////
/////////////////////////////////////
void Queue::submit(
	storm::core::span<const CommandBufferConstObserverPtr>
		command_buffers,
	storm::core::span<const SemaphoreConstObserverPtr> wait_semaphores,
	storm::core::span<const SemaphoreConstObserverPtr>
		signal_semaphores,
	render::FenceObserverPtr fence) const noexcept {
	const auto &device		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device.vkDeviceTable();

	auto vk_command_buffers = std::vector<VkCommandBuffer>{};
	vk_command_buffers.reserve(std::size(command_buffers));
	auto vk_wait_semaphores = std::vector<VkSemaphore>{};
	vk_wait_semaphores.reserve(std::size(wait_semaphores));
	auto vk_signal_semaphores = std::vector<VkSemaphore>{};
	vk_signal_semaphores.reserve(std::size(signal_semaphores));

	for (const auto &command_buffer : command_buffers)
		vk_command_buffers.emplace_back(
			static_cast<const CommandBuffer &>(*command_buffer));

	for (const auto &semaphore : wait_semaphores)
		vk_wait_semaphores.emplace_back(
			static_cast<const Semaphore &>(*semaphore));

	for (const auto &semaphore : signal_semaphores)
		vk_signal_semaphores.emplace_back(
			static_cast<const Semaphore &>(*semaphore));

	auto wait_stages = std::vector<VkPipelineStageFlags>{};
	wait_stages.resize(std::size(wait_semaphores),
					   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	const auto submit_info = VkSubmitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount =
			gsl::narrow_cast<std::uint32_t>(std::size(vk_wait_semaphores)),
		.pWaitSemaphores   = std::data(vk_wait_semaphores),
		.pWaitDstStageMask = std::data(wait_stages),
		.commandBufferCount =
			gsl::narrow_cast<std::uint32_t>(std::size(vk_command_buffers)),
		.pCommandBuffers = std::data(vk_command_buffers),
		.signalSemaphoreCount =
			gsl::narrow_cast<std::uint32_t>(std::size(vk_signal_semaphores)),
		.pSignalSemaphores = std::data(vk_signal_semaphores)};

	auto vk_fence = VkFence{VK_NULL_HANDLE};
	if (fence != nullptr) {
		fence->reset();
		vk_fence = static_cast<const Fence &>(*fence);
	}

	device_table.vkQueueSubmit(m_vk_queue, 1, &submit_info, vk_fence);
}

/////////////////////////////////////
/////////////////////////////////////
CommandBufferOwnedPtr
	Queue::createCommandBuffer(CommandBufferLevel level) const {
	auto command_buffer = std::move(createCommandBuffers(1, level)[0]);

	return command_buffer;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<render::CommandBufferOwnedPtr>
	Queue::createCommandBuffers(std::size_t count,
								render::CommandBufferLevel level) const {
	const auto &device = static_cast<const Device &>(*m_device);

	const auto allocate_info = VkCommandBufferAllocateInfo{
		.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool		= m_vk_command_pool,
		.level				= toVK(level),
		.commandBufferCount = gsl::narrow_cast<std::uint32_t>(count)};

	const auto vk_command_buffers =
		device.allocateVkCommandBuffers(allocate_info);

	auto command_buffers = std::vector<render::CommandBufferOwnedPtr>{};
	command_buffers.reserve(std::size(vk_command_buffers));

	for (const auto vk_command_buffer : vk_command_buffers)
		command_buffers.emplace_back(
			std::make_unique<CommandBuffer>(*this, level, vk_command_buffer));

	return command_buffers;
}
