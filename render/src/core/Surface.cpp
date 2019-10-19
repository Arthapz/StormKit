// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Surface::Surface(const window::Window &window, const render::Instance &instance)
	: m_instance{&instance}, m_window{&window} {
#if defined(STORM_OS_WINDOWS)
	const auto create_info = VkWin32SurfaceCreateInfoKHR{
		.sType	   = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.hinstance = GetModuleHandleW(nullptr),
		.hwnd	   = reinterpret_cast<HWND>(m_window->nativeHandle())};
#elif defined(STORM_OS_MACOS)
	const auto create_info = VkMacOSSurfaceCreateInfoMVK{
		.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK,
		.pView = m_window->nativeHandle()};
#elif defined(STORM_OS_LINUX)
	struct Handles {
		xcb_connection_t *connection;
		xcb_window_t window;
	} *handles = reinterpret_cast<Handles *>(m_window->nativeHandle());

	const auto create_info = VkXcbSurfaceCreateInfoKHR{
		.sType		= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
		.connection = handles->connection,
		.window		= handles->window};
#endif

	m_vk_surface		 = m_instance->createVkSurface(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
Surface::~Surface() {
	const auto &context = static_cast<const Instance &>(*m_instance);

	if (m_vk_surface != VK_NULL_HANDLE)
		vkDestroySurfaceKHR(context, m_vk_surface, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
Surface::Surface(Surface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Surface &Surface::operator=(Surface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Surface::initialize(const render::Device &device) {
	m_device = core::makeConstObserver(&device);

	createSwapchain();

	for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		m_image_availables.emplace_back(device.createSemaphore());
		m_render_finisheds.emplace_back(device.createSemaphore());
		m_in_flight_fences.emplace_back(device.createFence());
	}
}

/////////////////////////////////////
/////////////////////////////////////
void Surface::recreate() {
	STORM_EXPECTS(m_device != nullptr);

	m_need_recreate = false;

	createSwapchain();
}

/////////////////////////////////////
/////////////////////////////////////
void Surface::destroy() {
	m_textures.clear();

	m_image_availables.clear();
	m_render_finisheds.clear();
	m_in_flight_fences.clear();

	destroySwapchain();
}

/////////////////////////////////////
/////////////////////////////////////
render::Surface::Frame Surface::acquireNextFrame() {
	const auto &device		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device.vkDeviceTable();
	auto image_index		 = std::uint32_t{0u};

	const auto &image_available =
		*m_image_availables[m_current_frame];
	const auto &render_finished =
		*m_render_finisheds[m_current_frame];
	auto &in_flight =
		*m_in_flight_fences[m_current_frame];

	in_flight.wait();
	in_flight.reset();

	device_table.vkAcquireNextImageKHR(
		device, m_vk_swapchain, std::numeric_limits<std::uint64_t>::max(),
		image_available, VK_NULL_HANDLE, &image_index);

	return Frame{.image_index	  = image_index,
				 .image_available = _std::make_observer(&image_available),
				 .render_finished = _std::make_observer(&render_finished),
				 .in_flight		  = _std::make_observer(&in_flight)};
}

/////////////////////////////////////
/////////////////////////////////////
void Surface::present(const render::Surface::Frame &frame) {
	const auto &device		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device.vkDeviceTable();

	auto vk_wait_semaphore =
		static_cast<const Semaphore &>(*frame.render_finished).vkSemaphore();

	const auto present_info =
		VkPresentInfoKHR{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
						 .waitSemaphoreCount = 1,
						 .pWaitSemaphores	 = &vk_wait_semaphore,
						 .swapchainCount	 = 1,
						 .pSwapchains		 = &m_vk_swapchain,
						 .pImageIndices		 = &frame.image_index,
						 .pResults			 = nullptr};

	auto result = device_table.vkQueuePresentKHR(
		static_cast<const Queue &>(device.graphicsQueue()), &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		m_need_recreate = true;

	m_current_frame = (m_current_frame + 1);
	if (m_current_frame >= MAX_FRAMES_IN_FLIGHT)
		m_current_frame -= MAX_FRAMES_IN_FLIGHT;
}

/////////////////////////////////////
/////////////////////////////////////
void Surface::createSwapchain() {
	const auto &device_ = static_cast<const Device &>(*m_device);

	const auto &physical_device = device_.physicalDevice();

	const auto capabilities = physical_device.queryVkSurfaceCapabilities(*this);
	const auto formats		= physical_device.queryVkSurfaceFormats(*this);
	const auto present_modes = physical_device.queryVkPresentModes(*this);

	const auto format			  = chooseSwapSurfaceFormat(formats);
	const auto present_mode		  = chooseSwapPresentMode(present_modes);
	const auto extent			  = chooseSwapExtent(capabilities);
	const auto image_count		  = chooseImageCount(capabilities);
	const auto image_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;

	if (m_vk_swapchain != VK_NULL_HANDLE) {
		std::swap(m_vk_old_swapchain, m_vk_swapchain);
	}

	const auto create_info = VkSwapchainCreateInfoKHR{
		.sType			  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface		  = m_vk_surface,
		.minImageCount	  = image_count,
		.imageFormat	  = format.format,
		.imageColorSpace  = format.colorSpace,
		.imageExtent	  = extent,
		.imageArrayLayers = 1,
		.imageUsage		  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
					  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.imageSharingMode = image_sharing_mode,
		.preTransform	  = capabilities.currentTransform,
		.compositeAlpha	  = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode	  = present_mode,
		.clipped		  = VK_TRUE,
		.oldSwapchain	  = m_vk_old_swapchain};

	m_vk_swapchain = device_.createVkSwapchain(create_info);

	if (m_vk_old_swapchain != VK_NULL_HANDLE) {
		m_textures.clear();
		m_vk_images.clear();

		device_.destroyVkSwapchain(m_vk_old_swapchain);
		m_vk_old_swapchain = VK_NULL_HANDLE;
	}

	auto vk_image_count = 0u;
	vkGetSwapchainImagesKHR(device_, m_vk_swapchain, &vk_image_count, nullptr);
	m_vk_images.resize(vk_image_count);
	vkGetSwapchainImagesKHR(device_, m_vk_swapchain, &vk_image_count,
							std::data(m_vk_images));

	m_pixel_format = fromVK(VkPixelFormatType{format.format});
	m_image_count  = vk_image_count;
	m_extent	   = {extent.width, extent.height};

	for (const auto vk_image : m_vk_images)
		m_textures.emplace_back(std::make_unique<Texture>(
			device_, m_extent, m_pixel_format, vk_image));

	auto transition_command_buffer =
		m_device->graphicsQueue().createCommandBuffer();
	transition_command_buffer->begin(true);
	for (auto &texture : m_textures)
		transition_command_buffer->transitionImageLayout(
			*texture, storm::render::ImageLayout::Present_Src);
	transition_command_buffer->end();

	transition_command_buffer->build();

	auto fence = m_device->createFence();

	auto to_submit = core::makeConstObserverArray(transition_command_buffer);
	m_device->graphicsQueue().submit(to_submit, {}, {},
									 core::makeObserver(fence));

	auto fences = std::array{std::cref(*fence)};
	m_device->waitForFences(fences);
}

/////////////////////////////////////
/////////////////////////////////////
void Surface::destroySwapchain() {
	if (m_vk_swapchain != VK_NULL_HANDLE) {
		STORM_EXPECTS(m_device != nullptr);
		const auto &device = static_cast<const Device &>(*m_device);
		device.destroyVkSwapchain(m_vk_swapchain);
	}
}

/////////////////////////////////////
/////////////////////////////////////
VkSurfaceFormatKHR Surface::chooseSwapSurfaceFormat(
	storm::core::span<const VkSurfaceFormatKHR> formats) noexcept {
	for (const auto format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
			format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}

	return formats[0];
}

/////////////////////////////////////
/////////////////////////////////////
VkPresentModeKHR Surface::chooseSwapPresentMode(
	storm::core::span<const VkPresentModeKHR> present_modes) noexcept {
	auto present_mode_ = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto &present_mode : present_modes) {
		if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return present_mode;
	}

	return present_mode_;
}

/////////////////////////////////////
/////////////////////////////////////
VkExtent2D Surface::chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR &capabilities) noexcept {
	constexpr const static auto int_max =
		std::numeric_limits<std::uint32_t>::max();

	if (capabilities.currentExtent.width != int_max &&
		capabilities.currentExtent.height != int_max)
		return capabilities.currentExtent;

	auto actual_extent	= VkExtent2D{m_window->size().w, m_window->size().h};
	actual_extent.width = std::max(
		capabilities.minImageExtent.width,
		std::min(capabilities.maxImageExtent.width, actual_extent.width));
	actual_extent.height = std::max(
		capabilities.minImageExtent.height,
		std::min(capabilities.maxImageExtent.height, actual_extent.height));

	return actual_extent;
}

/////////////////////////////////////
/////////////////////////////////////
std::uint32_t Surface::chooseImageCount(
	const VkSurfaceCapabilitiesKHR &capabilities) noexcept {
	auto image_count = capabilities.minImageCount + 1;

	if (capabilities.maxImageCount > 0 &&
		image_count > capabilities.maxImageCount)
		image_count = capabilities.maxImageCount;

	return image_count;
}


