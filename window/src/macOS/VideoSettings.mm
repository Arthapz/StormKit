#include <storm/window/VideoSettings.hpp>

#import <AppKit/NSScreen.h>


using namespace storm;
using namespace storm::window;

std::vector<VideoSettings> VideoSettings::getDesktopModes() {
    auto screens = [NSScreen screens];
    auto screen_count = [screens count];
    
    auto video_settings = std::vector<VideoSettings>{};
    video_settings.reserve(screen_count);
    
    for(auto i = 0u; i < screen_count; ++i) {
        auto screen = [screens objectAtIndex: i];
        auto screen_rect = [screen visibleFrame];
        
        const auto video_setting = VideoSettings {
            .size = {
                .width = static_cast<std::uint16_t>(screen_rect.size.width),
                .height = static_cast<std::uint16_t>(screen_rect.size.height)
            }
        };
        
        video_settings.emplace_back(std::move(video_setting));
    }
    
    return video_settings;
}
