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

            auto video_setting = storm::window::VideoSettings { crtc->width, crtc->height };
            video_settings.emplace_back(std::move(video_setting));

            // free
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
        auto display = xcb_connect(nullptr, nullptr);
        auto screen  = xcb_setup_roots_iterator(xcb_get_setup(display)).data;
        auto root    = screen->root;

        auto reply = xcb_get_geometry(display, root);
        auto attr  = xcb_get_geometry_reply(display, reply, nullptr);
        std::cout << attr->width << ':' << attr->height << std::endl;

        video_setting.size.width  = attr->width;
        video_setting.size.height = attr->height;
        video_setting.size.depth  = attr->depth;

        init = true;
    }

    std::cout << video_setting.size.width << ':' << video_setting.size.height << std::endl;
    return &video_setting;
}
