// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window {
    /////////////////////////////////////
    /////////////////////////////////////
    inline const std::string &AbstractWindow::title() const noexcept { return m_title; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const VideoSettings &AbstractWindow::videoSettings() const noexcept {
        return m_video_settings;
    }
} // namespace storm::window
