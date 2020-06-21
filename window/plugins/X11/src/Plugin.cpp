#include "InputHandlerImpl.hpp"
#include "WindowImpl.hpp"

/////////// - STL - ///////////
#include <memory>
#include <vector>

/////////// - StormKit::window - ///////////
#include <storm/window/VideoSettings.hpp>
#include <storm/window/Window.hpp>

/////////// - XCB - ///////////
#include <xcb/randr.h>
#include <xcb/xcb.h>

DEFINE_WINDOW_PLUGIN(storm::window::WindowImpl, storm::window::InputHandlerImpl)

/////////////////////////////////////
/////////////////////////////////////
const storm::window::VideoSettings *getDesktopModes(storm::core::ArraySize &size) {
    static auto video_settings = std::vector<storm::window::VideoSettings> {};
    static auto init           = false;

    if (!init) {
        auto display = xcb_connect(nullptr, nullptr);
        auto screen  = xcb_setup_roots_iterator(xcb_get_setup(display)).data;
        auto root    = screen->root;

        auto reply = xcb_randr_get_screen_resources_current_reply(
            display,
            xcb_randr_get_screen_resources_current(display, root),
            nullptr);

        auto timestamp = reply->config_timestamp;

        auto len          = xcb_randr_get_screen_resources_current_outputs_length(reply);
        auto randr_output = xcb_randr_get_screen_resources_current_outputs(reply);

        video_settings.reserve(len);
        for (auto i = 0; i < len; ++i) {
            auto output = xcb_randr_get_output_info_reply(display,
                                                          xcb_randr_get_output_info(display,
                                                                                    randr_output[i],
                                                                                    timestamp),
                                                          nullptr);
            if (output == nullptr) continue;

            auto crtc = xcb_randr_get_crtc_info_reply(display,
                                                      xcb_randr_get_crtc_info(display,
                                                                              output->crtc,
                                                                              timestamp),
                                                      nullptr);
            if (crtc == nullptr) continue;

            auto video_setting = storm::window::VideoSettings { crtc->width, crtc->height, 32u };
            video_settings.emplace_back(std::move(video_setting));

            // free
        }

        xcb_disconnect(display);
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
        auto size   = storm::core::ArraySize { 0u };
        auto *modes = getDesktopModes(size);

        for (auto i = 0u; i < size; ++i) {
            video_setting.size.w = std::max(video_setting.size.w, modes[i].size.w);
            video_setting.size.h = std::max(video_setting.size.h, modes[i].size.h);
            video_setting.size.h = std::max(video_setting.size.d, modes[i].size.d);
        }

        init = true;
    }

    std::cout << video_setting.size.width << ':' << video_setting.size.height << std::endl;
    return &video_setting;
}
