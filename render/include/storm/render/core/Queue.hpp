// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <initializer_list>
#include <optional>

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/sync/Fwd.hpp>

namespace storm::render {
	class STORM_PUBLIC Queue : public core::NonCopyable {
	  public:
		explicit Queue(const render::Device &device, QueueFlag flags,
					   std::uint32_t family_index, VkQueue queue);
		 ~Queue() ;

		Queue(Queue &&);
		Queue &operator=(Queue &&);

		CommandBufferOwnedPtr createCommandBuffer(
			CommandBufferLevel level = CommandBufferLevel::Primary) const;

		 void waitIdle() const noexcept ;

		void submit( // todo optimise rvalue / lvalue
			storm::core::span<const CommandBufferConstObserverPtr>
				command_buffers,
			storm::core::span<const SemaphoreConstObserverPtr> wait_semaphores =
				{},
			storm::core::span<const SemaphoreConstObserverPtr>
				signal_semaphores  = {},
			FenceObserverPtr fence = nullptr) const noexcept;

		 std::vector<CommandBufferOwnedPtr> createCommandBuffers(
			std::size_t count,
			CommandBufferLevel level = CommandBufferLevel::Primary) const ;

		inline QueueFlag type() const noexcept {
			return m_queue_flag;
		}

		inline const Device &device() const noexcept {
            return *m_device;
		}

		inline VkQueue vkQueue() const noexcept {
			STORM_EXPECTS(m_vk_queue != VK_NULL_HANDLE);
			return m_vk_queue;
		}

		inline operator VkQueue() const noexcept {
			STORM_EXPECTS(m_vk_queue != VK_NULL_HANDLE);
			return m_vk_queue;
		}

		inline VkCommandPool vkCommandPool() const noexcept {
			STORM_EXPECTS(m_vk_command_pool != VK_NULL_HANDLE);
			return m_vk_command_pool;
		}

	  private:
        DeviceConstObserverPtr m_device;

		QueueFlag m_queue_flag;

		std::uint32_t m_family_index;

		VkQueue m_vk_queue = VK_NULL_HANDLE;

		VkCommandPool m_vk_command_pool = VK_NULL_HANDLE;
	};

} // namespace storm::render
