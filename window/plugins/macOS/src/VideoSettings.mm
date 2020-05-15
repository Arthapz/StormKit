#include <storm/window/VideoSettings.hpp>

#include <storm/core/Types.hpp>

#import <AppKit/AppKit.h>

using namespace storm;
using namespace storm::window;

core::UInt8 modeBitsPerPixel(CGDisplayModeRef mode)
{
    auto bpp = core::UInt8{0};

    CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
    if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        bpp = 32;
    else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        bpp = 16;
    else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        bpp = 8;

    CFRelease(pixEnc);

    return bpp;
}

std::vector<VideoSettings> VideoSettings::getDesktopModes() {
    NSArray* modes  =  (__bridge NSArray *)CGDisplayCopyAllDisplayModes(CGMainDisplayID(), nullptr);

    const auto mode_count = [modes count];

    auto video_settings = std::vector<VideoSettings>{};
    video_settings.reserve(mode_count);

    for (auto i = CFIndex{0}; i < mode_count; ++i) {
        auto mode = (CGDisplayModeRef)[modes objectAtIndex: i];

        const auto video_setting = VideoSettings {
            .size = {
                .width = static_cast<core::UInt16>(CGDisplayModeGetWidth(mode)),
                .height = static_cast<core::UInt16>(CGDisplayModeGetHeight(mode))
            },
            .bpp = modeBitsPerPixel(mode)
        };

        if (std::find(std::cbegin(video_settings), std::cend(video_settings), video_setting) == std::cend(video_settings))
            video_settings.emplace_back(std::move(video_setting));
    }

    std::sort(std::begin(video_settings), std::end(video_settings), std::greater<VideoSettings>{});

    return video_settings;
}

VideoSettings VideoSettings::getDesktopFullscreenSize() {
    return {};
}