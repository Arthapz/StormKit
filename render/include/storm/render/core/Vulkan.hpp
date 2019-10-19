// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

#include <gsl/string_span>

#include <vk_mem_alloc.h>
#include <volk.h>

#include <storm/core/Configure.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>

#undef VK_NULL_HANDLE
#define VK_NULL_HANDLE nullptr

namespace storm::render {
	template <typename T>
	inline constexpr auto vkMakeVersion(T &&major, T &&minor, T &&patch) {
		return (((major) << 22u) | ((minor) << 12u) | (patch));
	}

	template <typename T> inline constexpr auto vkVersionMajor(T &&version) {
		return static_cast<std::remove_reference_t<T>>(
			static_cast<std::uint32_t>(version >> 22u));
	}

	template <typename T> inline constexpr auto vkVersionMinor(T &&version) {
		return static_cast<std::remove_reference_t<T>>(
			static_cast<std::uint32_t>((version >> 12u) & 0x3ffu));
	}

	template <typename T> inline constexpr auto vkVersionPatch(T &&version) {
		return static_cast<std::remove_reference_t<T>>(
			static_cast<std::uint32_t>(version & 0xfffu));
	}

#if defined(VK_USE_PLATFORM_XCB_KHR)
	static constexpr const auto SURFACE_EXTENSION_NAME =
		VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	static constexpr const auto SURFACE_EXTENSION_NAME =
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	static constexpr const auto SURFACE_EXTENSION_NAME =
		VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#endif

	static constexpr const auto DEVICE_EXTENSIONS =
		std::array<gsl::czstring<>, 1>{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

#if defined(STORM_OS_LINUX)
	static constexpr const auto VALIDATION_LAYERS =
		std::array<gsl::czstring<>, 2>{
			"VK_LAYER_KHRONOS_validation",
			"VK_LAYER_MESA_overlay",
		};
#else
	static constexpr const auto VALIDATION_LAYERS =
		std::array<const char *, 1>{"VK_LAYER_KHRONOS_validation"};
#endif

	static constexpr const auto INSTANCE_EXTENSIONS =
		std::array<gsl::czstring<>, 2>{
			VK_KHR_SURFACE_EXTENSION_NAME,
			SURFACE_EXTENSION_NAME,
		};

	static constexpr const auto STORMKIT_VERSION = vkMakeVersion(
		STORM_MAJOR_VERSION, STORM_MINOR_VERSION, STORM_PATCH_VERSION);

#ifdef STORM_BUILD_DEBUG
	static constexpr const auto ENABLE_VALIDATION = true;
#else
    static constexpr const auto ENABLE_VALIDATION = false;
#endif
} // namespace storm::render::vulkan
