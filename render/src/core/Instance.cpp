// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <map>

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/PhysicalDeviceInfo.hpp>
#include <storm/render/core/Surface.hpp>

using namespace storm;
using namespace storm::log;
using namespace storm::render;

std::uint64_t
	scorePhysicalDevice(const render::PhysicalDevice &physical_device) {
	if (!physical_device.checkExtensionSupport(DEVICE_EXTENSIONS))
		return 0u;

	auto score = std::uint64_t{0u};

	const auto &info = physical_device.info();

	if (info.type == render::PhysicalDeviceType::Discrete_GPU)
		score += 10000000u;
	else if (info.type == render::PhysicalDeviceType::Virtual_GPU)
		score += 5000000u;
	else if (info.type == render::PhysicalDeviceType::Integrated_GPU)
		score += 250000u;

	const auto &capabilities = physical_device.capabilities();

	score += capabilities.limits.max_image_dimension_1D;
	score += capabilities.limits.max_image_dimension_2D;
	score += capabilities.limits.max_image_dimension_3D;
	score += capabilities.limits.max_image_dimension_cube;
	score += capabilities.limits.max_uniform_buffer_range;

	return score;
}

extern "C" {
/////////////////////////////////////
/////////////////////////////////////
VKAPI_ATTR VkBool32
	vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
					VkDebugUtilsMessageTypeFlagsEXT message_type,
					const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
					[[maybe_unused]] void *user_data) {
	const auto module  = "Vulkan backend"_module;
	auto format_string = "";

	if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		format_string = "[Validation] {}";
	else if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
		format_string = "[General] {}";
	else if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		format_string = "[Performance] {}";

	switch (message_severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		LogHandler::dlog(module, format_string, callback_data->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LogHandler::elog(module, format_string, callback_data->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LogHandler::wlog(module, format_string, callback_data->pMessage);
		break;
	default:
		break;
	}

	return VK_FALSE;
}
}

/////////////////////////////////////
/////////////////////////////////////
Instance::Instance() {
	volkInitialize();

	createInstance();
	retrievePhysicalDevices();
}

/////////////////////////////////////
/////////////////////////////////////
Instance::~Instance() {
	if (ENABLE_VALIDATION && m_messenger != VK_NULL_HANDLE)
		vkDestroyDebugUtilsMessengerEXT(m_instance, m_messenger, nullptr);

	if (m_instance != VK_NULL_HANDLE)
		vkDestroyInstance(m_instance, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
Instance::Instance(Instance &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Instance &Instance::operator=(Instance &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Instance::createInstance() noexcept {
	const auto app_info =
		VkApplicationInfo{.sType		 = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                          .pEngineName	 = "StormKit C++",
						  .engineVersion = STORMKIT_VERSION,
                          .apiVersion	 = VK_API_VERSION_1_0};

	auto create_info =
		VkInstanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                             .pApplicationInfo	= &app_info,
							 .enabledLayerCount = 0};

	auto instance_extensions = std::vector<const char *>{
		std::cbegin(INSTANCE_EXTENSIONS), std::cend(INSTANCE_EXTENSIONS)};

	const auto enable_validation =
		checkValidationLayerSupport(ENABLE_VALIDATION);
	if (enable_validation) {
		create_info.enabledLayerCount =
			gsl::narrow_cast<std::uint32_t>(std::size(VALIDATION_LAYERS));
		create_info.ppEnabledLayerNames = std::data(VALIDATION_LAYERS);

		instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	create_info.enabledExtensionCount =
		gsl::narrow_cast<std::uint32_t>(std::size(instance_extensions));
	create_info.ppEnabledExtensionNames = std::data(instance_extensions);

	const auto result = vkCreateInstance(&create_info, nullptr, &m_instance);
	STORM_ENSURES(result == VK_SUCCESS);

	volkLoadInstance(m_instance);

	if (enable_validation)
		createDebugReportCallback();
}

/////////////////////////////////////
/////////////////////////////////////
void Instance::createDebugReportCallback() {
	const auto debug_create_info = VkDebugUtilsMessengerCreateInfoEXT{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = vkDebugCallback};

	const auto result = vkCreateDebugUtilsMessengerEXT(
		m_instance, &debug_create_info, nullptr, &m_messenger);
	STORM_ENSURES(result == VK_SUCCESS);
}

void Instance::retrievePhysicalDevices() noexcept {
	auto physical_device_count = std::uint32_t{0};
	vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr);

	auto physical_devices =
		std::vector<VkPhysicalDevice>{physical_device_count};
	vkEnumeratePhysicalDevices(m_instance, &physical_device_count,
							   std::data(physical_devices));

	for (auto physical_device : physical_devices)
		m_physical_devices.emplace_back(
			std::make_unique<PhysicalDevice>(physical_device, *this));
}
/////////////////////////////////////
/////////////////////////////////////
render::SurfaceOwnedPtr
	Instance::createSurface(const window::Window &window) const {
	return std::make_unique<Surface>(window, *this);
}

/////////////////////////////////////
/////////////////////////////////////
const render::PhysicalDevice &Instance::pickPhysicalDevice() const noexcept {
	auto ranked_devices =
		std::multimap<std::uint64_t, render::PhysicalDeviceCRef>{};

	for (const auto &physical_device : m_physical_devices) {
		const auto score = scorePhysicalDevice(*physical_device);

		ranked_devices.emplace(score, *physical_device);
	}

	// ASSERT(ranked_devices.rbegin()->first >= 0u);

	return ranked_devices.rbegin()->second.get();
}

/////////////////////////////////////
/////////////////////////////////////
const render::PhysicalDevice &
	Instance::pickPhysicalDevice(const render::Surface &surface) noexcept {
	auto ranked_devices =
		std::multimap<std::uint64_t, render::PhysicalDeviceCRef>{};

	for (auto &physical_device : m_physical_devices) {
		physical_device->checkIfPresentSupportIsEnabled(surface);

		const auto score = scorePhysicalDevice(*physical_device);
		LogHandler::ilog("Vulkan backend"_module, "Found {} with score: {}",
						 physical_device->info().device_name, score);

		ranked_devices.emplace(score, *physical_device);
	}

	// ASSERT(ranked_devices.rbegin()->first >= 0u);

	return ranked_devices.rbegin()->second.get();
}

#if defined(STORM_OS_WINDOWS)
/////////////////////////////////////
/////////////////////////////////////
VkSurfaceKHR Instance::createVkSurface(
	const VkWin32SurfaceCreateInfoKHR &create_info) const noexcept {
	auto vk_surface = VkSurfaceKHR{};

	auto func_vkCreateWin32SurfaceKHR =
		reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
			vkGetInstanceProcAddr(m_instance, "vkCreateWin32SurfaceKHR"));
	auto result = func_vkCreateWin32SurfaceKHR(m_instance, &create_info,
											   nullptr, &vk_surface);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_surface;
}
#elif defined(STORM_OS_MACOS)
/////////////////////////////////////
/////////////////////////////////////
VkSurfaceKHR Instance::createVkSurface(
	const VkMacOSSurfaceCreateInfoMVK &create_info) const noexcept {
	auto vk_surface = VkSurfaceKHR{};

	auto func_vkCreateMacOSSurfaceMVK =
		reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(
			vkGetInstanceProcAddr(m_instance, "vkCreateMacOSSurfaceMVK"));
	auto result = func_vkCreateMacOSSurfaceMVK(m_instance, &create_info,
											   nullptr, &vk_surface);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_surface;
}
#elif defined(STORM_OS_LINUX)
/////////////////////////////////////
/////////////////////////////////////
VkSurfaceKHR Instance::createVkSurface(
    const VkXcbSurfaceCreateInfoKHR &create_info) const noexcept {
	auto vk_surface = VkSurfaceKHR{};

	auto func_vkCreateXcbSurfaceKHR =
		reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(
			vkGetInstanceProcAddr(m_instance, "vkCreateXcbSurfaceKHR"));
	auto result = func_vkCreateXcbSurfaceKHR(m_instance, &create_info, nullptr,
											 &vk_surface);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_surface;
}
#endif

/////////////////////////////////////
/////////////////////////////////////
bool Instance::checkValidationLayerSupport(bool enable_validation) noexcept {
	if (!enable_validation)
		return enable_validation;

	auto available_layer_count = std::uint32_t{0};
	vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);

	auto available_layers =
		std::vector<VkLayerProperties>{available_layer_count};
	vkEnumerateInstanceLayerProperties(&available_layer_count,
									   std::data(available_layers));

	for (auto layer_name : std::as_const(VALIDATION_LAYERS)) {
		auto layer_found = false;

		for (const auto &layer_properties : available_layers) {
			if (std::strcmp(layer_name, layer_properties.layerName) == 0) {
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
			return false;
	}

	return enable_validation;
}
