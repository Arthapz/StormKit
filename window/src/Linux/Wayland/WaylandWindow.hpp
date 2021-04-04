// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../../AbstractWindow.hpp"

/////////// - STL - ///////////
#include <functional>
#include <memory>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - Wayland - ///////////
#include "xdg-shell.h"
#include <wayland-client.h>

namespace storm::window::details {
    STORMKIT_RAII_CAPSULE(WLDisplay, wl_display, wl_display_disconnect)
    STORMKIT_RAII_CAPSULE(WLRegistry, wl_registry, wl_registry_destroy)
    STORMKIT_RAII_CAPSULE(WLCompositor, wl_compositor, wl_compositor_destroy)
    STORMKIT_RAII_CAPSULE(XDGShell, xdg_wm_base, xdg_wm_base_destroy)
    STORMKIT_RAII_CAPSULE(WLSurface, wl_surface, wl_surface_destroy)
    STORMKIT_RAII_CAPSULE(XDGSurface, xdg_surface, xdg_surface_destroy)
    STORMKIT_RAII_CAPSULE(XDGTopLevel, xdg_toplevel, xdg_toplevel_destroy)
    STORMKIT_RAII_CAPSULE(WLShm, wl_shm, wl_shm_destroy)
    STORMKIT_RAII_CAPSULE(WLShmPool, wl_shm_pool, wl_shm_pool_destroy)
    STORMKIT_RAII_CAPSULE(WLBuffer, wl_buffer, wl_buffer_destroy)
    STORMKIT_RAII_CAPSULE(WLShell, wl_shell, wl_shell_destroy)
    STORMKIT_RAII_CAPSULE(WLShellSurface, wl_shell_surface, wl_shell_surface_destroy)

    class STORMKIT_PRIVATE WaylandWindow final: public AbstractWindow {
      public:
        struct Handles {
            wl_display *display;
            wl_surface *surface;
        };

        WaylandWindow();
        WaylandWindow(std::string title,
                      const VideoSettings &settings,
                      storm::window::WindowStyle style);
        ~WaylandWindow() override;

        WaylandWindow(WaylandWindow &&) noexcept;
        WaylandWindow &operator=(WaylandWindow &&) noexcept;

        void create(std::string title, const VideoSettings &settings, WindowStyle style) override;
        void close() noexcept override;

        [[nodiscard]] bool pollEvent(Event &event) noexcept override;
        [[nodiscard]] bool waitEvent(Event &event) noexcept override;

        void setTitle(std::string title) noexcept override;
        void setVideoSettings(const VideoSettings &settings) noexcept override;

        [[nodiscard]] const core::Extentu &size() const noexcept override;

        [[nodiscard]] bool isOpen() const noexcept override;
        [[nodiscard]] bool isVisible() const noexcept override;

        [[nodiscard]] NativeHandle nativeHandle() const noexcept override;
        [[nodiscard]] const Handles &waylandHandles() const noexcept { return m_handles; }

        void registryGlobal(wl_registry *registry,
                            std::uint32_t id,
                            const char *interface,
                            std::uint32_t version) noexcept;
        void surfaceConfigure(xdg_surface *surface, std::uint32_t serial) noexcept;
        void toplevelConfigure(xdg_toplevel *xdg_tl,
                               std::int32_t width,
                               std::int32_t height,
                               wl_array *state) noexcept;
        void toplevelClose(xdg_toplevel *xdg_tl) noexcept;
        void shellSurfaceConfigure(wl_shell_surface *xdg_tl,
                                   std::uint32_t edges,
                                   std::int32_t width,
                                   std::int32_t height) noexcept;

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(WaylandWindow)
      private:
        void createXDGShell() noexcept;
        void createWaylandShell() noexcept;
        void createPixelbuffer() noexcept;

        WLDisplayScoped m_display;
        WLRegistryScoped m_registry;
        WLCompositorScoped m_compositor;
        WLShellScoped m_wayland_shell;
        XDGShellScoped m_xdg_shell;
        WLSurfaceScoped m_surface;
        XDGSurfaceScoped m_xdg_surface;
        WLShellSurfaceScoped m_wlshell_surface;
        XDGTopLevelScoped m_xdg_toplevel;
        WLShmScoped m_shm;
        WLShmPoolScoped m_shm_pool;
        WLBufferScoped m_buffer;

        Handles m_handles;

        bool m_opened          = false;
        bool m_visible         = false;
        core::Extentu m_extent = {};

        bool m_configured = false;

        friend class WaylandInputHandler;
    };
    DECLARE_PTR_AND_REF(WaylandWindow)
} // namespace storm::window::details
