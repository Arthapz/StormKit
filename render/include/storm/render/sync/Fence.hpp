// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <cstdint>
#include <limits>

#include <storm/core/Platform.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
	class STORM_PUBLIC Fence : public core::NonCopyable {
	  public:
		explicit Fence(const Device &device);
		~Fence();

		Fence(Fence &&);
		Fence &operator=(Fence &&);

		WaitResult wait(std::uint64_t wait_for = std::numeric_limits<std::uint64_t>::max()) const;
		void reset()								  ;

		inline VkFence vkFence() const noexcept {
			STORM_EXPECTS(m_vk_fence != VK_NULL_HANDLE);
			return m_vk_fence;
		}

		inline operator VkFence() const noexcept {
			STORM_EXPECTS(m_vk_fence != VK_NULL_HANDLE);
			return m_vk_fence;
		}
	  private:
		DeviceConstObserverPtr m_device;

		VkFence m_vk_fence = VK_NULL_HANDLE;
	};
} // namespace storm::render
