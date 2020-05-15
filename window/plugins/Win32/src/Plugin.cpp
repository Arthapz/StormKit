#include "InputHandlerImpl.hpp"
#include "WindowImpl.hpp"

/////////// - STL - ///////////
#include <memory>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Ranges.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/VideoSettings.hpp>
#include <storm/window/Window.hpp>

/////////// - Windows - ///////////
#include <Windows.h>

DEFINE_WINDOW_PLUGIN(storm::window::WindowImpl, storm::window::InputHandlerImpl)

/////////////////////////////////////
/////////////////////////////////////
const storm::window::VideoSettings *getDesktopModes(storm::core::ArraySize &size) {
    static auto video_settings = std::vector<storm::window::VideoSettings> {};
    static auto init           = false;

    if (!init) {
        auto dm = DEVMODE {};
        ZeroMemory(&dm, sizeof(DEVMODE));

        dm.dmSize = sizeof(dm);

        auto video_settings = std::vector<storm::window::VideoSettings> {};
        for (auto i = 0; EnumDisplaySettings(nullptr, i, &dm) != 0; ++i) {
            auto video_setting = storm::window::VideoSettings {
                .size = { .width  = gsl::narrow_cast<storm::core::UInt16>(dm.dmPelsWidth),
                          .height = gsl::narrow_cast<storm::core::UInt16>(dm.dmPelsHeight) }
            };

            video_settings.emplace_back(std::move(video_setting));
        }

        init = true;
    }

    size = std::size(video_settings);

    return std::data(video_settings);
}

/////////////////////////////////////
/////////////////////////////////////
const storm::window::VideoSettings *getDesktopFullscreenSize() {
    static auto video_setting = storm::window::VideoSettings {};
    static auto init          = false;

    if (!init) {
        video_setting.size.width  = static_cast<storm::core::UInt16>(GetSystemMetrics(SM_CXSIZE));
        video_setting.size.height = static_cast<storm::core::UInt16>(GetSystemMetrics(SM_CYSIZE));

        init = true;
    }
    return &video_setting;
}
