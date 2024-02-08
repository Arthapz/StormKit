// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/Core/PlatformMacro.hpp>

#if defined(STORMKIT_OS_LINUX)
    #include <wayland-client.h>
    #include <xcb/xcb.h>
#elif defined(STORMKIT_OS_WINDOWS)
    #include <windows.h>
#endif

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Wsi;
import stormkit.Gpu.Vulkan;

import :Core;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Surface::Surface(const Instance& instance, const wsi::Window& window, Tag)
        : InstanceObject { instance } {
#if defined(STORMKIT_OS_WINDOWS)
        const auto create_surface = [this, &window] {
            const auto create_info =
                vk::Win32SurfaceCreateInfoKHR { .flags     = {},
                                                .hinstance = GetModuleHandleW(nullptr),
                                                .hwnd =
                                                    reinterpret_cast<HWND>(window.nativeHandle()) };
            return this->instance().vkHandle().createWin32SurfaceKHR(create_info, nullptr);
        };
#elif defined(STORMKIT_OS_MACOS)
        const auto create_surface = [this, &window] {
            const auto create_info = VkMacOSSurfaceCreateInfoMVK {
                .sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK,
                .pView = m_window->nativeHandle();
            };
            CHECK_VK_ERROR(
                vkCreateMacOSSurfaceMVK(this->instance(), &create_info, nullptr, &m_surface));
        };
#elif defined(STORMKIT_OS_LINUX)
        const auto make_wayland_surface = [this, &window] {
            struct Handles {
                wl_display* display;
                wl_surface* surface;
            }* handles = std::bit_cast<Handles*>(window.nativeHandle());

            const auto create_info =
                vk::WaylandSurfaceCreateInfoKHR { .display = handles->display,
                                                  .surface = handles->surface };

            return this->instance().vkHandle().createWaylandSurfaceKHR(create_info, nullptr);
        };
        const auto make_xcb_surface = [this, &window] {
            struct Handles {
                xcb_connection_t* connection;
                xcb_window_t      window;
            }* handles = reinterpret_cast<Handles*>(window.nativeHandle());

            const auto create_info =
                vk::XcbSurfaceCreateInfoKHR { .connection = handles->connection,
                                              .window     = handles->window };

            return this->instance().vkHandle().createXcbSurfaceKHR(create_info, nullptr);
        };

        const auto create_surface = [&make_wayland_surface, &make_xcb_surface] noexcept
            -> core::FunctionRef<VulkanExpected<vk::raii::SurfaceKHR>> {
            const auto is_wayland = std::getenv("WAYLAND_DISPLAY") != nullptr;

            if (is_wayland) return make_wayland_surface;

            return make_xcb_surface;
        }();

#elif defined(STORMKIT_OS_IOS)
        const auto create_surface = [this, &window] {
            const auto create_info =
                VkIOSSurfaceCreateInfoMVK { .sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK,
                                            .pView = m_window->nativeHandle() };
            CHECK_VK_ERROR(vkCreateIOSSurfaceMVK(this->instance(), &create_info, &m_surface));
        };
#else
        core::assertWithMessage(true, "This platform WSI is not supported !");
#endif

        create_surface()
            .transform(core::monadic::set(m_vk_surface))
            .transform_error(
                core::monadic::map(core::monadic::narrow<Result>(), core::throwError()));
    }
} // namespace stormkit::gpu
