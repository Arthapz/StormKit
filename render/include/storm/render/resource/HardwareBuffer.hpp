// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <optional>

#include <storm/core/Filesystem.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/window/Window.hpp>

namespace storm::render {
	class STORM_PUBLIC HardwareBuffer : public core::NonCopyable {
	  public:
		explicit HardwareBuffer(
			const Device &device, HardwareBufferUsage usage, std::size_t size,
			MemoryProperty property = MemoryProperty::Host_Visible |
									  MemoryProperty::Host_Coherent);
		 ~HardwareBuffer();

		HardwareBuffer(HardwareBuffer &&);
		HardwareBuffer &operator=(HardwareBuffer &&);

		inline const Device &device() const noexcept {
            return *m_device;
		}
		inline HardwareBufferUsage usage() const noexcept {
			return m_usage;
		}

		 std::byte *map(std::uint32_t offset )			;
		 void flush(std::ptrdiff_t offset, std::size_t size) ;
		 void unmap()										;

		template <typename T>
		inline void upload(storm::core::span<const T> data,
						   std::ptrdiff_t offset = 0) {
			const auto size = std::size(data) * sizeof(T);

			auto data_ptr = map(gsl::narrow_cast<std::uint32_t>(offset));
			std::memcpy(data_ptr, std::data(data), size);
			flush(offset, size);
			unmap();
		}

		inline VkBuffer vkBuffer() const noexcept {
			STORM_EXPECTS(m_vk_buffer != VK_NULL_HANDLE);
			return m_vk_buffer;
		}

		inline operator VkBuffer() const noexcept {
			STORM_EXPECTS(m_vk_buffer != VK_NULL_HANDLE);
			return m_vk_buffer;
		}

	  private:
		static std::uint32_t findMemoryType(
			std::uint32_t type_filter, VkMemoryPropertyFlags properties,
			const VkPhysicalDeviceMemoryProperties &mem_properties,
			const VkMemoryRequirements &mem_requirements);

		DeviceConstObserverPtr m_device;
		HardwareBufferUsage m_usage;
		std::size_t m_size;

		VkBuffer m_vk_buffer			  = VK_NULL_HANDLE;
		VmaAllocation m_vma_buffer_memory = VK_NULL_HANDLE;
	};

} // namespace storm::render
