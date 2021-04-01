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
#include <wayland-client.h>

namespace storm::window::details {
    STORMKIT_RAII_CAPSULE(WLDisplay, wl_display, wl_display_disconnect)
    STORMKIT_RAII_CAPSULE(WLRegistry, wl_registry, wl_registry_destroy)
    STORMKIT_RAII_CAPSULE(WLCompositor, wl_compositor, wl_compositor_destroy)
    STORMKIT_RAII_CAPSULE(WLShell, wl_shell, wl_shell_destroy)
    STORMKIT_RAII_CAPSULE(WLSurface, wl_surface, wl_surface_destroy)
    STORMKIT_RAII_CAPSULE(WLShellSurface, wl_shell_surface, wl_shell_surface_destroy)

    class STORMKIT_PRIVATE WaylandWindow final: public details::AbstractWindow {
      public:
        struct Handles {};

        WaylandWindow();
        WaylandWindow(std::string title,
                      const VideoSettings &settings,
                      storm::window::WindowStyle style);
        ~WaylandWindow() override;

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
        [[nodiscard]] const Handles &xcbHandles() const noexcept { return m_handles; }

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(WaylandWindow)
      private:
        WLDisplayScoped m_display;
        WLRegistryScoped m_registry;
        WLCompositorScoped m_compositor;
        WLShellScoped m_shell;
        WLSurfaceScoped m_surface;
        WLShellSurfaceScoped m_shell_surface;

        Handles m_handles;
        friend class WaylandInputHandler;
    };
    DECLARE_PTR_AND_REF(WaylandWindow)
} // namespace storm::window::details
