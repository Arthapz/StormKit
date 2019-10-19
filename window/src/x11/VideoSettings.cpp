// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/window/VideoSettings.hpp>
#include <xcb/randr.h>
#include <xcb/xcb.h>

using namespace storm;
using namespace storm::window;

/////////////////////////////////////
/////////////////////////////////////
std::vector<VideoSettings> VideoSettings::getDesktopModes() {
	auto display = xcb_connect(nullptr, nullptr);
	auto screen  = xcb_setup_roots_iterator(xcb_get_setup(display)).data;
	auto root	= screen->root;

	auto reply = xcb_randr_get_screen_resources_current_reply(
		display, xcb_randr_get_screen_resources_current(display, root),
		nullptr);

	auto timestamp = reply->config_timestamp;

	auto len = xcb_randr_get_screen_resources_current_outputs_length(reply);
	auto randr_output = xcb_randr_get_screen_resources_current_outputs(reply);

	auto video_settings = std::vector<VideoSettings>{};
	video_settings.reserve(len);
	for (auto i = 0; i < len; ++i) {
		auto output = xcb_randr_get_output_info_reply(
			display,
			xcb_randr_get_output_info(display, randr_output[i], timestamp),
			nullptr);
		if (output == nullptr)
			continue;

		auto crtc = xcb_randr_get_crtc_info_reply(
			display, xcb_randr_get_crtc_info(display, output->crtc, timestamp),
			nullptr);
		if (crtc == nullptr)
			continue;

		auto video_setting = VideoSettings{crtc->width, crtc->height};
		video_settings.emplace_back(std::move(video_setting));

		// free
	}

	return video_settings;
}
