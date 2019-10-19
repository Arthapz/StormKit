#pragma once

#include <storm/image/Image.hpp>
#include <vector>

namespace storm::image::private_ {
	struct ImageData {
		Image::Extent extent			   = {0, 0};
		std::uint8_t channel			   = 0;
		std::vector<Image::data_type> data = {};
	};
} // namespace storm::image::private_
