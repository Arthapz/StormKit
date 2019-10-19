// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Math.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Fwd.hpp>

#include <storm/render/sync/Fwd.hpp>

#include <storm/window/Window.hpp>

namespace storm::render {
	class STORM_PUBLIC Surface : public core::NonCopyable {
	  public:
		static constexpr auto MAX_FRAMES_IN_FLIGHT = 3;

		struct Frame {
			std::uint32_t image_index;

			SemaphoreConstObserverPtr image_available;
			SemaphoreConstObserverPtr render_finished;
            FenceObserverPtr in_flight;
		};

		Surface(const window::Window &window, const Instance &instance);
		 ~Surface() ;

		Surface(Surface &&);
		Surface &operator=(Surface &&);

		 void initialize(const Device &device) ;
		 void recreate()						  ;
		 void destroy()						  ;

		 Frame acquireNextFrame()		 ;
		 void present(const Frame &frame) ;

		inline storm::core::span<const TextureOwnedPtr> textureOutput() const
			noexcept {
			return m_textures;
		}

		inline PixelFormat pixelFormat() const noexcept {
			return m_pixel_format;
		}

		inline std::uint32_t imageCount() const noexcept {
			return m_image_count;
		}

		inline bool needRecreate() const noexcept {
			return m_need_recreate;
		}

		inline const core::Extent &extent() const noexcept {
			return m_extent;
		}

		inline VkSurfaceKHR vkSurface() const noexcept {
			STORM_EXPECTS(m_vk_surface != VK_NULL_HANDLE);
			return m_vk_surface;
		}

		inline operator VkSurfaceKHR() const noexcept {
			STORM_EXPECTS(m_vk_surface != VK_NULL_HANDLE);
			return m_vk_surface;
		}

	  public:
		STORM_PRIVATE void createSwapchain();
		STORM_PRIVATE void destroySwapchain();

		STORM_PRIVATE VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			storm::core::span<const VkSurfaceFormatKHR> formats) noexcept;
		STORM_PRIVATE VkPresentModeKHR chooseSwapPresentMode(
			storm::core::span<const VkPresentModeKHR> present_modes) noexcept;
		STORM_PRIVATE VkExtent2D chooseSwapExtent(
			const VkSurfaceCapabilitiesKHR &capabilities) noexcept;
		STORM_PRIVATE std::uint32_t chooseImageCount(
			const VkSurfaceCapabilitiesKHR &capabilities) noexcept;

		InstanceConstObserverPtr m_instance;
        DeviceConstObserverPtr m_device = nullptr;

        window::WindowConstObserverPtr m_window;

        PixelFormat m_pixel_format	= PixelFormat::Undefined;
		std::uint32_t m_image_count = 1;

        std::vector<TextureOwnedPtr> m_textures;

		std::vector<SemaphoreOwnedPtr> m_image_availables;
		std::vector<SemaphoreOwnedPtr> m_render_finisheds;
		std::vector<FenceOwnedPtr> m_in_flight_fences;

		bool m_need_recreate = false;

		core::Extent m_extent;

		VkSurfaceKHR m_vk_surface		  = VK_NULL_HANDLE;
		VkSwapchainKHR m_vk_swapchain	 = VK_NULL_HANDLE;
		VkSwapchainKHR m_vk_old_swapchain = VK_NULL_HANDLE;

		std::vector<VkImage> m_vk_images;

		std::uint32_t m_current_frame = 0;
	};

} // namespace storm::render
