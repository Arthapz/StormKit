#include "InputHandlerImpl.h"
#include "WindowImpl.h"

/////////// - STL - ///////////
#include <memory>
#include <vector>

/////////// - StormKit::window - ///////////
#include <storm/window/VideoSettings.hpp>
#include <storm/window/Window.hpp>

/////////// - Cocoa - ///////////
#import <AppKit/AppKit.h>

DEFINE_WINDOW_PLUGIN(storm::window::WindowImpl, storm::window::InputHandlerImpl)

storm::core::UInt8 modeBitsPerPixel(CGDisplayModeRef mode) {
    auto bpp = storm::core::UInt8 { 0 };

    CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
    if (CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) ==
        kCFCompareEqualTo)
        bpp = 32;
    else if (CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) ==
             kCFCompareEqualTo)
        bpp = 16;
    else if (CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) ==
             kCFCompareEqualTo)
        bpp = 8;

    CFRelease(pixEnc);

    return bpp;
}

const storm::window::VideoSettings *getDesktopModes(storm::core::ArraySize &size) {
    static auto video_settings = std::vector<storm::window::VideoSettings> {};
    static auto init           = false;

    if (!init) {
        NSArray *modes =
            (__bridge NSArray *)CGDisplayCopyAllDisplayModes(CGMainDisplayID(), nullptr);

        const auto mode_count = [modes count];

        video_settings.reserve(mode_count);

        for (auto i = CFIndex { 0 }; i < mode_count; ++i) {
            auto mode = (CGDisplayModeRef)[modes objectAtIndex:i];

            const auto video_setting = storm::window::VideoSettings {
                .size = { .width = static_cast<storm::core::UInt16>(CGDisplayModeGetWidth(mode)),
                          .height =
                              static_cast<storm::core::UInt16>(CGDisplayModeGetHeight(mode)) },
                .bpp  = modeBitsPerPixel(mode)
            };

            if (std::find(std::cbegin(video_settings), std::cend(video_settings), video_setting) ==
                std::cend(video_settings))
                video_settings.emplace_back(std::move(video_setting));
        }

        std::sort(std::begin(video_settings),
                  std::end(video_settings),
                  std::greater<storm::window::VideoSettings> {});
        init = true;
    }

    std::size(video_settings);

    return std::data(video_settings);
}

const storm::window::VideoSettings *getDesktopFullscreenSize() {
    static auto video_setting = storm::window::VideoSettings {};
    static auto init          = false;

    return &video_setting;
}
