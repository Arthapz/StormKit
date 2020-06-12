/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

using namespace storm;
using namespace storm::image;

namespace storm::image {
    extern constexpr core::UInt8 getChannelCountFor(Image::Format format) noexcept;
}

std::optional<std::string> Image::loadTARGA(core::ByteConstSpan data) noexcept {
    return std::nullopt;
}

std::optional<std::string> Image::saveTARGA(const std::filesystem::path &filepath) const noexcept {
    auto _filename = filepath;

    return std::nullopt;
}
