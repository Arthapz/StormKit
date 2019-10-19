// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
	class STORM_PUBLIC Semaphore : public core::NonCopyable {
	  public:
		explicit Semaphore(const Device &device);
		~Semaphore();

		Semaphore(Semaphore &&);
		Semaphore &operator=(Semaphore &&);

		inline VkSemaphore vkSemaphore() const noexcept {
			STORM_EXPECTS(m_vk_semaphore != VK_NULL_HANDLE);
			return m_vk_semaphore;
		}

		inline operator VkSemaphore() const noexcept {
			STORM_EXPECTS(m_vk_semaphore != VK_NULL_HANDLE);
			return m_vk_semaphore;
		}
	  private:
		DeviceConstObserverPtr m_device;

		VkSemaphore m_vk_semaphore = VK_NULL_HANDLE;
	};
} // namespace storm::render
