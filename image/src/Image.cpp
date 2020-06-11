/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Ranges.hpp>
#include <storm/core/Strings.hpp>

/////////// - STL - ///////////
#include <fstream>
#include <streambuf>

/////////// - GSL - ///////////
#include <gsl/gsl_util>

using namespace storm;
using namespace storm::image;

namespace storm::image {
    static constexpr auto KTX_HEADER = core::makeStaticByteArray(0xAB,
                                                                 0x4B,
                                                                 0x54,
                                                                 0x58,
                                                                 0x20,
                                                                 0x31,
                                                                 0x31,
                                                                 0xBB,
                                                                 0x0D,
                                                                 0x0A,
                                                                 0x1A,
                                                                 0x0A);

    static constexpr auto PNG_HEADER =
        core::makeStaticByteArray(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A);

    static constexpr auto JPEG_HEADER = core::makeStaticByteArray(0xFF, 0xD8);

    constexpr core::UInt8 getChannelCountFor(Image::Format format) noexcept {
        switch (format) {
            case Image::Format::R: return 1u;
            case Image::Format::RG: return 2u;
            case Image::Format::RGB:
            case Image::Format::BGR: return 3u;
            case Image::Format::RGBA:
            case Image::Format::BGRA: return 4u;
            default: return 0u;
        }

        return 0u;
    }

    Image::Codec filenameToCodec(const std::filesystem::path &filename) noexcept {
        STORM_EXPECTS(std::filesystem::exists(filename));
        STORM_EXPECTS(filename.has_extension());
        STORM_EXPECTS(!std::filesystem::is_directory(filename));
        STORM_EXPECTS(std::filesystem::is_regular_file(filename));

        const std::string ext = filename.extension().string();

        if (core::toLower(ext) == ".jpg" || core::toLower(ext) == ".jpeg")
            return Image::Codec::JPEG;
        else if (core::toLower(ext) == ".png")
            return Image::Codec::PNG;
        else if (core::toLower(ext) == ".tga" || core::toLower(ext) == ".targa")
            return Image::Codec::TARGA;
        else if (core::toLower(ext) == ".ppm")
            return Image::Codec::PPM;
        else if (core::toLower(ext) == ".hdr")
            return Image::Codec::HDR;
        else if (core::toLower(ext) == ".ktx")
            return Image::Codec::KTX;

        return Image::Codec::UNKNOW;
    }

    constexpr Image::Codec headerToCodec(core::ByteConstSpan data) noexcept {
        STORM_EXPECTS(std::size(data) >= 12);

        if (std::memcmp(std::data(data), std::data(KTX_HEADER), std::size(KTX_HEADER)) == 0)
            return Image::Codec::KTX;
        else if (std::memcmp(std::data(data), std::data(PNG_HEADER), std::size(PNG_HEADER)) == 0)
            return Image::Codec::PNG;
        else if (std::memcmp(std::data(data), std::data(JPEG_HEADER), std::size(JPEG_HEADER)) == 0)
            return Image::Codec::JPEG;

        return Image::Codec::UNKNOW;
    } // namespace storm::image
} // namespace storm::image

/////////////////////////////////////
/////////////////////////////////////
Image::Image() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Image::Image(const std::filesystem::path &filepath, Image::Codec codec) noexcept : Image {} {
    loadFromFile(filepath, codec);
}

/////////////////////////////////////
/////////////////////////////////////
Image::Image(core::ByteConstSpan data, Image::Codec codec) noexcept : Image {} {
    loadFromMemory(data, codec);
}

/////////////////////////////////////
/////////////////////////////////////
Image::~Image() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
bool Image::loadFromFile(const std::filesystem::path &filepath, Image::Codec codec) noexcept {
    STORM_EXPECTS(codec != Image::Codec::UNKNOW);

    STORM_EXPECTS(std::filesystem::exists(filepath));

    auto file = std::basic_ifstream<std::byte> { filepath, std::ios::binary };

    const auto data = core::ByteArray { std::istreambuf_iterator<core::Byte> { file },
                                        std::istreambuf_iterator<core::Byte> {} };

    if (codec == Image::Codec::AUTODETECT) codec = filenameToCodec(filepath);
    switch (codec) {
        case Image::Codec::JPEG: return loadJPEG(data);
        case Image::Codec::PNG: return loadPNG(data);
        case Image::Codec::TARGA: return loadTARGA(data);
        case Image::Codec::PPM: return loadPPM(data);
        case Image::Codec::HDR: return loadHDR(data);
        case Image::Codec::KTX: return loadKTX(data);
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool Image::loadFromMemory(core::ByteConstSpan data, Image::Codec codec) noexcept {
    STORM_EXPECTS(codec != Image::Codec::UNKNOW);
    STORM_EXPECTS(!std::empty(data));

    if (codec == Image::Codec::AUTODETECT) codec = headerToCodec(data);
    switch (codec) {
        case Image::Codec::JPEG: return loadJPEG(data);
        case Image::Codec::PNG: return loadPNG(data);
        case Image::Codec::TARGA: return loadTARGA(data);
        case Image::Codec::PPM: return loadPPM(data);
        case Image::Codec::HDR: return loadHDR(data);
        case Image::Codec::KTX: return loadKTX(data);
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool Image::saveToFile(const std::filesystem::path &filepath,
                       Codec codec,
                       CodecArgs args) const noexcept {
    STORM_EXPECTS(codec != Image::Codec::UNKNOW);
    STORM_EXPECTS(codec != Image::Codec::AUTODETECT);
    STORM_EXPECTS(!std::empty(filepath));

    switch (codec) {
        case Image::Codec::JPEG: return saveJPEG(filepath);
        case Image::Codec::PNG: return savePNG(filepath);
        case Image::Codec::TARGA: return saveTARGA(filepath);
        case Image::Codec::PPM: return savePPM(filepath, args);
        case Image::Codec::HDR: return saveHDR(filepath);
        case Image::Codec::KTX: return saveKTX(filepath);
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
void Image::create(core::Extentu extent, Format format) noexcept {
    STORM_EXPECTS(extent.width > 0u && extent.height > 0u && extent.depth > 0u &&
                  format != Format::UNKNOW);

    m_format        = format;
    m_extent        = extent;
    m_channel_count = getChannelCountFor(format);

    m_data.clear();
    m_data.resize(extent.width * extent.height * extent.depth * m_channel_count);

    STORM_ENSURES(size() == m_extent.width * m_extent.height * m_extent.depth * m_channel_count);
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::toFormat(Format format) noexcept {
    STORM_EXPECTS(!std::empty(m_data));
    STORM_EXPECTS(!std::empty(m_data));
    STORM_EXPECTS(!std::empty(m_data));
    STORM_EXPECTS(format != Format::UNKNOW);

    const auto pixel_count = m_extent.width * m_extent.height * m_extent.depth;

    const auto to_format_channel_count = getChannelCountFor(format);
    const auto channel_copy_count      = std::min(to_format_channel_count, m_channel_count);

    auto image = Image { m_extent, format };
    for (auto i = 0u; i < pixel_count; ++i) {
        const auto from_image = operator[](i);
        auto to_image         = image[i];

        core::ranges::copy_n(core::ranges::cbegin(from_image),
                             std::min(to_format_channel_count, m_channel_count),
                             core::ranges::begin(to_image));

        core::ranges::fill_n(core::ranges::begin(to_image) + channel_copy_count,
                             to_format_channel_count,
                             core::Byte { 0u });
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::scale([[maybe_unused]] const core::Extentu &) noexcept {
    auto image = Image { m_extent, m_format };

    core::ranges::copy(*this, core::ranges::begin(image));

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipX() noexcept {
    auto image = Image { m_extent, m_format };

    core::ranges::fill(image, core::Byte { 0 });

    for (auto x = 0u; x < m_extent.width; ++x)
        for (auto y = 0u; y < m_extent.height; ++y)
            for (auto z = 0u; z < m_extent.depth; ++z) {
                const auto inv_x = m_extent.width - x;
                const auto out_index =
                    x + y * m_extent.width + z * m_extent.width * m_extent.height;
                const auto src_index =
                    inv_x + y * m_extent.width + z * m_extent.width * m_extent.height;

                core::ranges::copy(operator[](src_index), core::ranges::begin(image) + out_index);
            }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipY() noexcept {
    auto image = Image { m_extent, m_format };

    core::ranges::fill(image, core::Byte { 0 });

    for (auto x = 0u; x < m_extent.width; ++x)
        for (auto y = 0u; y < m_extent.height; ++y)
            for (auto z = 0u; z < m_extent.depth; ++z) {
                const auto inv_y = m_extent.height - y;
                const auto out_index =
                    x + y * m_extent.width + z * m_extent.width * m_extent.height;
                const auto src_index =
                    x + inv_y * m_extent.width + z * m_extent.width * m_extent.height;

                core::ranges::copy(operator[](src_index), core::ranges::begin(image) + out_index);
            }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipZ() noexcept {
    auto image = Image { m_extent, m_format };

    core::ranges::fill(image, core::Byte { 0 });

    for (auto x = 0u; x < m_extent.width; ++x)
        for (auto y = 0u; y < m_extent.height; ++y)
            for (auto z = 0u; z < m_extent.depth; ++z) {
                const auto inv_z = m_extent.height - z;
                const auto out_index =
                    x + y * m_extent.width + z * m_extent.width * m_extent.height;
                const auto src_index =
                    x + y * m_extent.width + inv_z * m_extent.width * m_extent.height;

                core::ranges::copy(operator[](src_index), core::ranges::begin(image) + out_index);
            }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate90() noexcept {
    const auto new_extent = core::Extentu { .width = m_extent.height, .height = m_extent.width };

    auto image = Image { std::move(new_extent), m_format };

    core::ranges::fill(image, core::Byte { 0 });

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate180() noexcept {
    auto image = Image { m_extent, m_format };

    core::ranges::fill(image, core::Byte { 0 });

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate270() noexcept {
    const auto new_extent = core::Extentu { .width = m_extent.height, .height = m_extent.width };

    auto image = Image { std::move(new_extent), m_format };

    core::ranges::fill(image, core::Byte { 0 });

    return image;
}
