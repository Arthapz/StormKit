// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <Windows.h>
#include <gsl/gsl_util>
#include <storm/window/VideoSettings.hpp>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
std::vector<VideoSettings> VideoSettings::getDesktopModes() {
    auto dm = DEVMODE {};
    ZeroMemory(&dm, sizeof(DEVMODE));

    dm.dmSize = sizeof(dm);

    auto video_settings = std::vector<VideoSettings> {};
    for (auto i = 0; EnumDisplaySettings(nullptr, i, &dm) != 0; ++i) {
        auto video_setting =
            VideoSettings { .size = { .width  = gsl::narrow_cast<core::UInt16>(dm.dmPelsWidth),
                                      .height = gsl::narrow_cast<core::UInt16>(dm.dmPelsHeight) }

            };

        video_settings.emplace_back(std::move(video_setting));
    }

    return video_settings;
}

/////////////////////////////////////
/////////////////////////////////////
VideoSettings VideoSettings::getDesktopFullscreenSize() {
    auto width  = static_cast<core::UInt32>(GetSystemMetrics(SM_CXSIZE));
    auto height = static_cast<core::UInt32>(GetSystemMetrics(SM_CYSIZE));
    return { width, height };
}
