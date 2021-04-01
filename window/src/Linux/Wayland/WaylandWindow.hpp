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

/////////// - XCB - ///////////
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xkbcommon/xkbcommon.h>

namespace storm::window::details {
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
        Handles m_handles;
        friend class WaylandInputHandler;
    };
    DECLARE_PTR_AND_REF(WaylandWindow)
} // namespace storm::window::details
