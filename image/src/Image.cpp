/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Numerics.hpp>
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
            case Image::Format::R8_SNorm:
            case Image::Format::R8_UNorm:
            case Image::Format::R16_SNorm:
            case Image::Format::R16_UNorm:
            case Image::Format::R8I:
            case Image::Format::R8U:
            case Image::Format::R16I:
            case Image::Format::R16U:
            case Image::Format::R32I:
            case Image::Format::R32U:
            case Image::Format::R16F:
            case Image::Format::R32F: return 1;

            case Image::Format::RG8_SNorm:
            case Image::Format::RG8_UNorm:
            case Image::Format::RG16_SNorm:
            case Image::Format::RG16_UNorm:
            case Image::Format::RG8I:
            case Image::Format::RG8U:
            case Image::Format::RG16I:
            case Image::Format::RG16U:
            case Image::Format::RG32I:
            case Image::Format::RG32U:
            case Image::Format::RG16F:
            case Image::Format::RG32F: return 2;

            case Image::Format::RGB8_SNorm:
            case Image::Format::RGB8_UNorm:
            case Image::Format::RGB16_SNorm:
            case Image::Format::RGB16_UNorm:
            case Image::Format::BGR8_UNorm:
            case Image::Format::RGB8I:
            case Image::Format::RGB8U:
            case Image::Format::RGB16I:
            case Image::Format::RGB16U:
            case Image::Format::RGB32I:
            case Image::Format::RGB32U:
            case Image::Format::RGB16F:
            case Image::Format::RGB32F:
            case Image::Format::sRGB8:
            case Image::Format::sBGR8: return 3;

            case Image::Format::RGBA8_SNorm:
            case Image::Format::RGBA8_UNorm:
            case Image::Format::RGBA16_SNorm:
            case Image::Format::RGBA16_UNorm:
            case Image::Format::BGRA8_UNorm:
            case Image::Format::RGBA8I:
            case Image::Format::RGBA8U:
            case Image::Format::RGBA16I:
            case Image::Format::RGBA16U:
            case Image::Format::RGBA32I:
            case Image::Format::RGBA32U:
            case Image::Format::RGBA16F:
            case Image::Format::RGBA32F:
            case Image::Format::sRGBA8:
            case Image::Format::sBGRA8: return 4;

            default: return 0u;
        }

        return 0u;
    }

    constexpr core::UInt8 getByteCountByChannelFor(Image::Format format) noexcept {
        switch (format) {
            case Image::Format::R8_SNorm:
            case Image::Format::R8_UNorm:
            case Image::Format::RG8_SNorm:
            case Image::Format::RG8_UNorm:
            case Image::Format::R8I:
            case Image::Format::R8U:
            case Image::Format::RG8I:
            case Image::Format::RG8U:
            case Image::Format::RGB8_SNorm:
            case Image::Format::RGB8_UNorm:
            case Image::Format::BGR8_UNorm:
            case Image::Format::RGB8I:
            case Image::Format::RGB8U:
            case Image::Format::RGBA8_SNorm:
            case Image::Format::RGBA8_UNorm:
            case Image::Format::RGBA16_SNorm:
            case Image::Format::BGRA8_UNorm:
            case Image::Format::sRGB8:
            case Image::Format::sBGR8:
            case Image::Format::sRGBA8:
            case Image::Format::sBGRA8: return 1u;

            case Image::Format::R16_SNorm:
            case Image::Format::R16_UNorm:
            case Image::Format::R16I:
            case Image::Format::R16U:
            case Image::Format::RG16_SNorm:
            case Image::Format::RG16_UNorm:
            case Image::Format::RG16I:
            case Image::Format::RG16U:
            case Image::Format::RG16F:
            case Image::Format::RGB16I:
            case Image::Format::RGB16U:
            case Image::Format::RGB16F:
            case Image::Format::RGBA16I:
            case Image::Format::RGBA16U:
            case Image::Format::RGBA16F:
            case Image::Format::R16F: return 2u;

            case Image::Format::R32I:
            case Image::Format::R32U:
            case Image::Format::R32F:
            case Image::Format::RG32I:
            case Image::Format::RG32U:
            case Image::Format::RG32F:
            case Image::Format::RGB16_SNorm:
            case Image::Format::RGB32I:
            case Image::Format::RGB32U:
            case Image::Format::RGB32F:
            case Image::Format::RGBA8I:
            case Image::Format::RGBA8U:
            case Image::Format::RGBA32I:
            case Image::Format::RGBA32U:
            case Image::Format::RGBA32F: return 4u;

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

        return Image::Codec::Unknown;
    }

    Image::Codec headerToCodec(core::ByteConstSpan data) noexcept {
        STORM_EXPECTS(std::size(data) >= 12);

        if (std::memcmp(std::data(data), std::data(KTX_HEADER), std::size(KTX_HEADER)) == 0)
            return Image::Codec::KTX;
        else if (std::memcmp(std::data(data), std::data(PNG_HEADER), std::size(PNG_HEADER)) == 0)
            return Image::Codec::PNG;
        else if (std::memcmp(std::data(data), std::data(JPEG_HEADER), std::size(JPEG_HEADER)) == 0)
            return Image::Codec::JPEG;

        return Image::Codec::Unknown;
    }

    core::ByteArray map(core::ByteConstSpan bytes,
                        core::UInt8 source_count,
                        core::UInt8 destination_count) noexcept {
        STORM_EXPECTS(source_count <= 4u && source_count > 0u && destination_count <= 4u &&
                      destination_count > 0u);

        static constexpr auto byte_1_min = std::numeric_limits<core::UInt8>::min();
        static constexpr auto byte_1_max = std::numeric_limits<core::UInt8>::max();
        static constexpr auto byte_2_min = std::numeric_limits<core::UInt16>::min();
        static constexpr auto byte_2_max = std::numeric_limits<core::UInt16>::max();
        static constexpr auto byte_4_min = std::numeric_limits<core::UInt32>::min();
        static constexpr auto byte_4_max = std::numeric_limits<core::UInt32>::max();

        auto data = core::ByteArray {};
        data.resize(std::size(bytes) * destination_count);

        if (source_count == 1u && destination_count == 2u) {
            const auto input_it = reinterpret_cast<const core::UInt8 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt16 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt16>(input_it[i],
                                                       byte_1_min,
                                                       byte_1_max,
                                                       byte_2_min,
                                                       byte_2_max);
        } else if (source_count == 1u && destination_count == 4u) {
            const auto input_it = reinterpret_cast<const core::UInt8 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt32 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt32>(input_it[i],
                                                       byte_1_min,
                                                       byte_1_max,
                                                       byte_4_min,
                                                       byte_4_max);
        } else if (source_count == 2u && destination_count == 1u) {
            const auto input_it = reinterpret_cast<const core::UInt16 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt8 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt8>(input_it[i],
                                                      byte_2_min,
                                                      byte_2_max,
                                                      byte_1_min,
                                                      byte_1_max);
        } else if (source_count == 2u && destination_count == 4u) {
            const auto input_it = reinterpret_cast<const core::UInt16 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt32 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt32>(input_it[i],
                                                       byte_2_min,
                                                       byte_2_max,
                                                       byte_4_min,
                                                       byte_4_max);
        } else if (source_count == 4u && destination_count == 1u) {
            const auto input_it = reinterpret_cast<const core::UInt32 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt8 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt8>(input_it[i],
                                                      byte_4_min,
                                                      byte_4_max,
                                                      byte_1_min,
                                                      byte_1_max);
        } else if (source_count == 4u && destination_count == 2u) {
            const auto input_it = reinterpret_cast<const core::UInt32 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt16 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt16>(input_it[i],
                                                       byte_4_min,
                                                       byte_4_max,
                                                       byte_2_min,
                                                       byte_2_max);
        } else
            data = { core::ranges::begin(bytes), core::ranges::end(bytes) };

        return data;
    }
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

////////////////////////////////////////
////////////////////////////////////////
Image::Image(const Image &rhs) noexcept
    : m_channel_count { rhs.m_channel_count }, m_bytes_per_channel { rhs.m_bytes_per_channel },
      m_mip_levels { rhs.m_mip_levels }, m_format { rhs.m_format }, m_data { rhs.m_data } {
}

////////////////////////////////////////
////////////////////////////////////////
Image &Image::operator=(const Image &rhs) noexcept {
    if (&rhs == this) return *this;

    m_channel_count     = rhs.m_channel_count;
    m_bytes_per_channel = rhs.m_bytes_per_channel;
    m_mip_levels        = rhs.m_mip_levels;
    m_format            = rhs.m_format;
    m_data              = rhs.m_data;

    return *this;
};

////////////////////////////////////////
////////////////////////////////////////
Image::Image(Image &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Image &Image::operator=(Image &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
bool Image::loadFromFile(const std::filesystem::path &filepath, Image::Codec codec) noexcept {
    STORM_EXPECTS(codec != Image::Codec::Unknown);

    if (!std::filesystem::exists(filepath)) {
        elog("Failed to open file\n    file: {}\n    reason: Incorrect path", filepath.string());

        return false;
    }

    STORM_EXPECTS(std::filesystem::exists(filepath));

    const auto data = [&filepath]() {
        auto file = std::ifstream { filepath, std::ios::binary };

        file.seekg(0, std::ios::end);
        auto file_size = static_cast<std::size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        auto data = core::ByteArray { file_size };
        file.read(reinterpret_cast<char *>(std::data(data)), file_size);

        return data;
    }();

    if (codec == Image::Codec::Autodetect) codec = filenameToCodec(filepath);
    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = loadJPEG(data); result.has_value()) {
                elog("Failed to open JPEG file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PNG: {
            if (auto result = loadPNG(data); result.has_value()) {
                elog("Failed to open PNG file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::TARGA: {
            if (auto result = loadTARGA(data); result.has_value()) {
                elog("Failed to open TARGA file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PPM: {
            if (auto result = loadPPM(data); result.has_value()) {
                elog("Failed to open PPM file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::HDR: {
            if (auto result = loadHDR(data); result.has_value()) {
                elog("Failed to open HDR file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::KTX: {
            if (auto result = loadKTX(data); result.has_value()) {
                elog("Failed to open KTX file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }

        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool Image::loadFromMemory(core::ByteConstSpan data, Image::Codec codec) noexcept {
    STORM_EXPECTS(codec != Image::Codec::Unknown);
    STORM_EXPECTS(!std::empty(data));

    if (codec == Image::Codec::Autodetect) codec = headerToCodec(data);
    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = loadJPEG(data); result.has_value()) {
                elog("Failed to parse JPEG data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PNG: {
            if (auto result = loadPNG(data); result.has_value()) {
                elog("Failed to parse PNG data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::TARGA: {
            if (auto result = loadTARGA(data); result.has_value()) {
                elog("Failed to open TARGA file\n    file: {}\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PPM: {
            if (auto result = loadPPM(data); result.has_value()) {
                elog("Failed to parse PPM data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::HDR: {
            if (auto result = loadHDR(data); result.has_value()) {
                elog("Failed to parse HDR data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::KTX: {
            if (auto result = loadKTX(data); result.has_value()) {
                elog("Failed to parse KTX data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool Image::saveToFile(const std::filesystem::path &filepath,
                       Codec codec,
                       CodecArgs args) const noexcept {
    STORM_EXPECTS(codec != Image::Codec::Unknown);
    STORM_EXPECTS(codec != Image::Codec::Autodetect);
    STORM_EXPECTS(!std::empty(filepath));
    STORM_EXPECTS(!std::empty(m_data));
    STORM_EXPECTS(std::filesystem::exists(filepath.root_directory()));

    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = saveJPEG(filepath); result.has_value()) {
                elog("Failed to parse JPEG data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PNG: {
            if (auto result = savePNG(filepath); result.has_value()) {
                elog("Failed to save PNG file\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::TARGA: {
            if (auto result = saveTARGA(filepath); result.has_value()) {
                elog("Failed to open TARGA file\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PPM: {
            if (auto result = savePPM(filepath, args); result.has_value()) {
                elog("Failed to parse PPM data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::HDR: {
            if (auto result = saveHDR(filepath); result.has_value()) {
                elog("Failed to parse HDR data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::KTX: {
            if (auto result = saveKTX(filepath); result.has_value()) {
                elog("Failed to parse KTX data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
void Image::create(core::Extentu extent, Format format) noexcept {
    STORM_EXPECTS(extent.width > 0u && extent.height > 0u && extent.depth > 0u &&
                  format != Format::Undefined);
    m_data.clear();

    m_format            = format;
    m_channel_count     = getChannelCountFor(format);
    m_bytes_per_channel = getByteCountByChannelFor(format);

    auto &mip = m_data.emplace_back(MipLevel { .extent = std::move(extent) });
    mip.data.resize(extent.width * extent.height * extent.depth * m_channel_count *
                    m_bytes_per_channel);
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::toFormat(Format format) const noexcept {
    STORM_EXPECTS(!std::empty(m_data));
    STORM_EXPECTS(format != Format::Undefined);

    auto image                = Image {};
    image.m_channel_count     = getChannelCountFor(format);
    image.m_bytes_per_channel = getByteCountByChannelFor(format);
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = format;
    image.m_data.reserve(std::size(m_data));

    const auto channel_delta =
        static_cast<core::UInt8>(std::min(0,
                                          static_cast<core::Int8>(image.m_channel_count) -
                                              static_cast<core::Int8>(m_channel_count)));

    auto mip_level = 0;
    for (auto &mip : m_data) {
        auto &_mip  = image.m_data.emplace_back();
        _mip.extent = mip.extent;
        _mip.data.resize(_mip.extent.width * _mip.extent.height * _mip.extent.depth *
                         image.m_channel_count * image.m_bytes_per_channel);

        const auto pixel_count = mip.extent.width * mip.extent.height * mip.extent.depth;
        for (auto i = 0u; i < pixel_count; i += m_bytes_per_channel * m_channel_count) {
            const auto from_image =
                map(pixel(i, mip_level), m_bytes_per_channel, image.m_bytes_per_channel);
            auto to_image = image.pixel(i, mip_level);

            core::ranges::copy_n(core::ranges::begin(from_image),
                                 image.m_channel_count,
                                 core::ranges::begin(to_image));

            core::ranges::fill_n(core::ranges::begin(to_image) + m_channel_count,
                                 channel_delta,
                                 core::Byte { 0u });
        }

        mip_level++;
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::scale([[maybe_unused]] const core::Extentu &) const noexcept {
    return *this;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipX() const noexcept {
    auto image                = Image {};
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto i = 0u; i < m_mip_levels; ++i) {
        auto &extent = image.m_data[i].extent;
        extent       = m_data[i].extent;

        for (auto x = 0u; x < extent.width; ++x)
            for (auto y = 0u; y < extent.height; ++y)
                for (auto z = 0u; z < extent.depth; ++z) {
                    const auto inv_x = extent.width - x;

                    auto output = image.pixel(core::Offset3u { inv_x, y, z }, i);

                    core::ranges::copy(pixel(core::Offset3u { x, y, z }, i),
                                       core::ranges::begin(output));
                }
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipY() const noexcept {
    auto image                = Image {};
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto i = 0u; i < m_mip_levels; ++i) {
        auto &extent = image.m_data[i].extent;
        extent       = m_data[i].extent;

        for (auto x = 0u; x < extent.width; ++x)
            for (auto y = 0u; y < extent.height; ++y)
                for (auto z = 0u; z < extent.depth; ++z) {
                    const auto inv_y = extent.height - y;

                    auto output = image.pixel(core::Offset3u { x, inv_y, z }, i);

                    core::ranges::copy(pixel(core::Offset3u { x, y, z }, i),
                                       core::ranges::begin(output));
                }
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipZ() const noexcept {
    auto image                = Image {};
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto i = 0u; i < m_mip_levels; ++i) {
        auto &extent = image.m_data[i].extent;
        extent       = m_data[i].extent;

        for (auto x = 0u; x < extent.width; ++x)
            for (auto y = 0u; y < extent.height; ++y)
                for (auto z = 0u; z < extent.depth; ++z) {
                    const auto inv_z = extent.depth - z;

                    auto output = image.pixel(core::Offset3u { x, y, inv_z }, i);

                    core::ranges::copy(pixel(core::Offset3u { x, y, z }, i),
                                       core::ranges::begin(output));
                }
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate90() const noexcept {
    auto image                = Image {};
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto i = 0u; i < m_mip_levels; ++i) {
        auto &extent = image.m_data[i].extent;
        extent       = m_data[i].extent;
        std::swap(extent.width, extent.height);
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate180() const noexcept {
    auto image                = Image {};
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto i = 0u; i < m_mip_levels; ++i) {
        auto &extent = image.m_data[i].extent;
        extent       = m_data[i].extent;
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate270() const noexcept {
    auto image                = Image {};
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto i = 0u; i < m_mip_levels; ++i) {
        auto &extent = image.m_data[i].extent;
        extent       = m_data[i].extent;
        std::swap(extent.width, extent.height);
    }

    return image;
}
