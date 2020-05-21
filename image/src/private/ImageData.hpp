#pragma once

#include <storm/image/Image.hpp>
#include <vector>

namespace storm::image::private_ {
    struct ImageData {
        core::Extentu extent               = { 0u, 0u, 1u };
        core::UInt8 channel                = 0u;
        std::vector<Image::data_type> data = {};
    };
} // namespace storm::image::private_
