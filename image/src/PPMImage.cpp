/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

using namespace storm;
using namespace storm::image;

std::optional<std::string> Image::loadPPM(core::ByteConstSpan data) noexcept {
    return std::nullopt;
}

std::optional<std::string> Image::savePPM(const std::filesystem::path &filepath,
                                          CodecArgs args) const noexcept {
    auto _filename = filepath;

    return std::nullopt;
}

std::optional<std::string> Image::savePPM(core::ByteArray &output,
                                          CodecArgs args) const noexcept {
    return std::nullopt;
}
