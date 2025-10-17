// Copywidth (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the y-level of this distribution

import std;

import stormkit.core;
import stormkit.log;
import stormkit.wsi;

#include <stormkit/log/log_macro.hpp>
#include <stormkit/main/main_macro.hpp>

#include <experimental/simd>

LOGGER("Framebuffer");

using namespace stormkit;
using namespace std::literals;

namespace stdr = std::ranges;

constexpr auto WIN_WIDTH  = 400;
constexpr auto WIN_HEIGHT = 400;

auto update_pixels(stormkit::ThreadPool& pool, const auto& extent) noexcept
  -> std::vector<rgbcolor<u8>> {
    const auto rect_width  = extent.width / 5;
    const auto rect_height = extent.height / 5;
    const auto cell_width  = rect_width / 5;

    const auto rect = math::rect {
        .x      = (extent.width / 2) - (rect_width / 2),
        .y      = (extent.height / 2) - (rect_height / 2),
        .width  = rect_width,
        .height = rect_height,
    };

    auto out = std::vector<rgbcolor<u8>> {};
    out.resize(extent.height * extent.width, colors::RED<u8>);

    auto data = std::mdspan { stdr::data(out), extent.height, extent.width };
    parallel_for(pool,
                 range(extent.width * extent.height),
                 [&rect, data, &pool, &extent](auto x_y) mutable noexcept {
                     const auto x = as<u32>(x_y % extent.width);
                     const auto y = as<u32>(x_y / extent.width);

                     if (math::AABB({ as<u32>(x), as<u32>(y) }, rect))
                         data[y, x] = colors::BLACK<u8>;
                     else {
                         const auto color_id = as<f32>(x) / extent.width;
                         if (color_id >= 0.8) data[y, x] = colors::BLUE<u8>;
                         else if (color_id >= 0.6)
                             data[y, x] = colors::GREEN<u8>;
                         else if (color_id >= 0.4)
                             data[y, x] = colors::WHITE<u8>;
                         else if (color_id >= 0.2)
                             data[y, x] = colors::YELLOW<u8>;
                         else
                             data[y, x] = colors::RED<u8>;
                     }
                 });

    return out;
}

auto main(std::span<const std::string_view> args) -> int {
    wsi::parse_args(args);

    auto logger = log::Logger::create_logger_instance<log::ConsoleLogger>();

    const auto monitors = wsi::get_monitors();
    ilog("--- Monitors ---");
    ilog("{}", monitors);

    auto window = wsi::Window::open("Hello world",
                                    { .width = WIN_WIDTH, .height = WIN_HEIGHT },
                                    wsi::WindowFlag::RESIZEABLE);
    ilog("wm: {}", window.wm());

    auto pool   = core::ThreadPool {};
    auto pixels = update_pixels(pool, window.framebuffer_extent());
    auto active = true;
    window
      .on(wsi::ClosedEventFunc { [&window] mutable noexcept { window.close(); } },
          wsi::MouseButtonDownEventFunc {
            [](u8 /*id*/, wsi::MouseButton button, const math::vec2i& position) noexcept {
                ilog("Mouse button down event: {} {}", button, position);
            } },
          wsi::ResizedEventFunc { [&](const math::Extent2<u32>&) mutable noexcept {
              pixels = update_pixels(pool, window.framebuffer_extent());
              window.fill_framebuffer(pixels);
          } },
          wsi::MonitorChangedEventFunc { [&](const wsi::Monitor&) mutable noexcept {
              pixels = update_pixels(pool, window.framebuffer_extent());
              window.fill_framebuffer(pixels);
          } },
          wsi::ActivateEventFunc { [&active] mutable noexcept { active = true; } },
          wsi::DeactivateEventFunc { [&active] mutable noexcept { active = false; } },
          wsi::KeyDownEventFunc { [&window](u8 /*id*/, wsi::Key key, char /*c*/) mutable noexcept {
              if (key == wsi::Key::ESCAPE) window.close();
          } });

    window.event_loop([&] noexcept {
        if (not active) std::this_thread::yield();
        window.fill_framebuffer(pixels);
    });

    return 0;
}
