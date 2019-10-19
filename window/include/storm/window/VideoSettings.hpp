// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>
#include <vector>

namespace storm::window {
    struct VideoSettings {
        core::Extentu size;
        union {
            core::UInt8 bitsPerPixel;
            core::UInt8 bpp;
        };
        union {
            core::UInt16 dotsPerInch;
            core::UInt16 dpi;
        };

        STORM_PUBLIC static std::vector<VideoSettings> getDesktopModes();
        STORM_PUBLIC static VideoSettings getDesktopFullscreenSize();

        inline bool operator==(const VideoSettings &other) const noexcept {
            return size.width == other.size.width && size.height == other.size.height &&
                   bpp == other.bpp;
        }

        inline bool operator>(const VideoSettings &other) const noexcept {
            return size.width > other.size.width;
        }
    };

} // namespace storm::window
