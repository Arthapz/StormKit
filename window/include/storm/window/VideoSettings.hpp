// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <cstdint>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>

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

        inline auto operator==(const VideoSettings &other) const noexcept -> bool {
            return size.width == other.size.width && size.height == other.size.height &&
                   bpp == other.bpp;
        }

        inline auto operator>(const VideoSettings &other) const noexcept -> bool {
            return size.width > other.size.width;
        }
    };

} // namespace storm::window
