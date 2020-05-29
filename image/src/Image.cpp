#include "private/ImageData.hpp"
#include "private/ImageLoader.hpp"

#include <gsl/gsl_util>

#include <storm/core/Ranges.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Strings.hpp>

#include <storm/image/Image.hpp>

using namespace storm;
using namespace storm::image;

namespace storm::image {
    Image::Codec filenameToCodec(const std::filesystem::path &filename) {
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

        return Image::Codec::UNKNOW;
    }

    Image::Codec headerToCodec(core::ByteConstSpan data) {
        namespace ranges = core::ranges;
        namespace views  = core::ranges::views;

        using namespace core::ranges;
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

        static constexpr auto JPEG_HEADER_1 = core::makeStaticByteArray(0xFF, 0xD8, 0xFF, 0xDB);
        static constexpr auto JPEG_HEADER_2 = core::makeStaticByteArray(0xFF,
                                                                        0xD8,
                                                                        0xFF,
                                                                        0xE0,
                                                                        0x00,
                                                                        0x10,
                                                                        0x4A,
                                                                        0x46,
                                                                        0x49,
                                                                        0x46,
                                                                        0x00,
                                                                        0x01);
        static constexpr auto JPEG_HEADER_3 = core::makeStaticByteArray(0xFF, 0xD8, 0xFF, 0xEE);

        if (auto range = data | views::take(ranges::size(KTX_HEADER));
            ranges::equal(KTX_HEADER, range))
            return Image::Codec::KTX;
        else if (auto range = data | views::take(ranges::size(PNG_HEADER));
                 ranges::equal(PNG_HEADER, range))
            return Image::Codec::PNG;
        else if (auto range = data | views::take(ranges::size(JPEG_HEADER_1));
                 ranges::equal(JPEG_HEADER_1, range) | ranges::equal(JPEG_HEADER_3, range))
            return Image::Codec::JPEG;
        else if (auto range = data | views::take(ranges::size(JPEG_HEADER_2));
                 ranges::equal(JPEG_HEADER_2, range))
            return Image::Codec::JPEG;

        return Image::Codec::UNKNOW;
    } // namespace storm::image
} // namespace storm::image

/////////////////////////////////////
/////////////////////////////////////
Image::Image() = default;

/////////////////////////////////////
/////////////////////////////////////
Image::Image(const std::filesystem::path &filepath, Image::Codec codec) : Image {} {
    loadFromFile(filepath, codec);
}

/////////////////////////////////////
/////////////////////////////////////
Image::Image(Image::ConstSpan data, Image::Codec codec) : Image {} {
    loadFromMemory(data, codec);
}

/////////////////////////////////////
/////////////////////////////////////
Image::~Image() = default;

/////////////////////////////////////
/////////////////////////////////////
void Image::loadFromFile(const std::filesystem::path &filepath, Image::Codec codec) {
    detach();

    if (codec == Image::Codec::AUTODETECT) codec = filenameToCodec(filepath);

    STORM_EXPECTS(codec != Image::Codec::UNKNOW);

    switch (codec) {
        case Image::Codec::PPM: m_data = private_::ImageLoader::loadPPM(filepath); break;
        case Image::Codec::JPEG: m_data = private_::ImageLoader::loadJpeg(filepath); break;
        case Image::Codec::PNG: m_data = private_::ImageLoader::loadPng(filepath); break;
        case Image::Codec::TARGA: m_data = private_::ImageLoader::loadTga(filepath); break;
        case Image::Codec::HDR: m_data = private_::ImageLoader::loadHDR(filepath); break;
        default: throw std::runtime_error("oops");
    }

    m_codec = codec;
}

/////////////////////////////////////
/////////////////////////////////////
void Image::loadFromMemory(Image::ConstSpan data, Image::Codec codec) {
    detach();

    if (codec == Image::Codec::AUTODETECT) codec = headerToCodec(data);

    STORM_EXPECTS(codec != Image::Codec::UNKNOW);

    switch (codec) {
        case Image::Codec::JPEG: m_data = private_::ImageLoader::loadJpeg(data); break;
        case Image::Codec::PNG: m_data = private_::ImageLoader::loadPng(data); break;
        case Image::Codec::TARGA: m_data = private_::ImageLoader::loadTga(data); break;
        case Image::Codec::PPM: m_data = private_::ImageLoader::loadPPM(data); break;
        case Image::Codec::HDR: m_data = private_::ImageLoader::loadHDR(data); break;
        default: throw std::runtime_error("oops");
    }

    m_codec = codec;
}

/////////////////////////////////////
/////////////////////////////////////
void Image::create(core::UInt32 width, core::UInt32 height, core::UInt8 channel_count) {
    STORM_EXPECTS(width > 0 && height > 0 && channel_count > 0);

    m_data          = std::make_shared<private_::ImageData>();
    m_data->extent  = { width, height };
    m_data->channel = channel_count;
    m_data->data.resize(width * height * m_data->channel);

    m_codec = Codec::UNKNOW;

    STORM_ENSURES(m_data->data.size() == width * height * channel_count);
}

/////////////////////////////////////
/////////////////////////////////////
Image::span Image::operator[](Image::size_type index) {
    STORM_EXPECTS(m_data != nullptr);
    STORM_EXPECTS(index < m_data->extent.width * m_data->extent.height);

    detach();

    return { std::data(data()) + index, std::data(data()) + index + channels() };
}

/////////////////////////////////////
/////////////////////////////////////
Image::ConstSpan Image::operator[](Image::size_type index) const noexcept {
    STORM_EXPECTS(m_data != nullptr);
    STORM_EXPECTS(index < m_data->extent.width * m_data->extent.height);

    return { std::data(data()) + index, std::data(data()) + index + channels() };
}

/////////////////////////////////////
/////////////////////////////////////
Image::span Image::operator()(XOffset x_offset, YOffset y_offset) {
    STORM_EXPECTS(m_data != nullptr);
    STORM_EXPECTS(x_offset < m_data->extent.width && y_offset < m_data->extent.height);

    detach();

    auto pointer = &m_data->data[(y_offset * extent().width + x_offset) * channels()];

    return { pointer, pointer + channels() };
}

/////////////////////////////////////
/////////////////////////////////////
Image::ConstSpan Image::operator()(XOffset x_offset, YOffset y_offset) const noexcept {
    STORM_EXPECTS(m_data != nullptr);
    STORM_EXPECTS(x_offset < m_data->extent.width && y_offset < m_data->extent.height);

    const auto pointer = &m_data->data[(y_offset * extent().width + x_offset) * channels()];

    return { pointer, pointer + channels() };
}

/////////////////////////////////////
/////////////////////////////////////
core::Extentu Image::extent() const noexcept {
    STORM_EXPECTS(m_data != nullptr);

    return m_data->extent;
}

/////////////////////////////////////
/////////////////////////////////////
core::UInt8 Image::channels() const noexcept {
    STORM_EXPECTS(m_data != nullptr);

    return m_data->channel;
}

/////////////////////////////////////
/////////////////////////////////////
Image::size_type Image::size() const noexcept {
    STORM_EXPECTS(m_data != nullptr);

    return std::size(m_data->data);
}

/////////////////////////////////////
/////////////////////////////////////
Image::ConstSpan Image::data() const noexcept {
    STORM_EXPECTS(m_data != nullptr);

    return m_data->data;
}

/////////////////////////////////////
/////////////////////////////////////
Image::span Image::data() {
    STORM_EXPECTS(m_data != nullptr);

    detach();

    return m_data->data;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::scale(const Image &src, [[maybe_unused]] const core::Extentu &) {
    return src;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipX(const Image &src) {
    Image out = src;
    out.detach();

    auto src_data = src.data();
    auto out_data = out.data();

    std::fill(core::ranges::begin(out_data), core::ranges::end(out_data), core::Byte { 0 });

    for (auto x = 0u; x < out.extent().width; ++x)
        for (auto y = 0u; y < out.extent().height; ++y)
            for (auto k = 0u; k < src.channels(); ++k) {
                const auto out_index = (x + y * out.extent().width) * src.channels() + k;
                const auto src_index =
                    (x + (out.extent().height) * out.extent().width - 1u - x) * src.channels() + k;

                out_data[out_index] = src_data[src_index];
            }

    return out;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipY(const Image &src) {
    Image out = src;
    out.detach();

    auto src_data = src.data();
    auto out_data = out.data();

    std::fill(core::ranges::begin(out_data), core::ranges::end(out_data), core::Byte { 0 });

    for (auto x = 0u; x < out.extent().width; ++x)
        for (auto y = 0u; y < out.extent().height; ++y)
            for (auto k = 0u; k < src.channels(); ++k) {
                const auto out_index = (x + y * out.extent().width) * src.channels() + k;
                const auto src_index =
                    (x + (out.extent().height - 1u - y) * out.extent().width) * src.channels() + k;

                out_data[out_index] = src_data[src_index];
            }
    return out;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate90(const Image &src) {
    return src;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate180(const Image &src) {
    return src;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate270(const Image &src) {
    return src;
}

/////////////////////////////////////
/////////////////////////////////////
void Image::saveToFile(const std::filesystem::path &filename, Codec codec, CodecArgs args) {
    STORM_EXPECTS(m_data != nullptr);
    STORM_EXPECTS(!std::empty(filename));

    if (codec == Image::Codec::AUTODETECT) codec = filenameToCodec(filename);

    switch (codec) {
        case Image::Codec::PPM: private_::ImageLoader::savePPM(filename, m_data, args); break;
        case Image::Codec::JPEG: private_::ImageLoader::saveJpeg(filename, m_data, args); break;
        case Image::Codec::PNG: private_::ImageLoader::savePng(filename, m_data, args); break;
        case Image::Codec::TARGA: private_::ImageLoader::saveTga(filename, m_data, args); break;
        case Image::Codec::HDR: private_::ImageLoader::saveHDR(filename, m_data, args); break;
        default: throw std::runtime_error("oops");
    }
}

void Image::addChannels(core::UInt8 count) {
    STORM_EXPECTS(m_data != nullptr);
    STORM_EXPECTS(count > 0u);

    detach();

    const auto pixel_count = m_data->extent.width * m_data->extent.height;

    auto image_data = std::vector<Image::data_type> {};
    image_data.reserve(pixel_count * (m_data->channel + count));

    for (auto i = 0u; i < pixel_count; ++i) {
        std::copy_n(core::ranges::begin(m_data->data) + (i * m_data->channel),
                    m_data->channel,
                    std::back_inserter(image_data));

        for (auto j = 0u; j < count; ++j) image_data.emplace_back(core::Byte {});
    }

    m_data->channel += count;
    m_data->data = std::move(image_data);
}

/////////////////////////////////////
/////////////////////////////////////
void Image::detach() {
    if (m_data && m_data.use_count() > 1) {
        private_::ImageData *tmp = m_data.get();
        m_data                   = std::make_shared<private_::ImageData>(*tmp);
    }
}
