#include "ImageLoader.hpp"

#include <cstddef>
#include <fstream>

#include <storm/core/Strings.hpp>

#include <gsl/gsl_util>

extern "C" {
#include <jerror.h>
#include <jpeglib.h>
#include <png.h>

struct PNGReadParam {
    storm::core::ArraySize readed;
    storm::image::Image::ConstSpan &data;
};

void jpeg_error_callback(jpeg_common_struct *st) noexcept {
    STORM_EXPECTS(st != nullptr);

    auto message = std::string {};
    message.resize(JMSG_STR_PARM_MAX);
    (*st->err->format_message)(st, std::data(message));

    if (st->client_data) {
        // const auto &filename = reinterpret_cast<const std::filesystem::path *>(st->client_data);
        // TODO LOG
        // throw std::runtime_error(fmt::format("Failed to load JPEG image from file %{1}, reason:
        // {2}", filename->string(), message));
    } else {
        //	throw std::runtime_error(fmt::format("Failed to load JPEG image from memory, reason:
        //{1}", message));
    }
}

void png_read_func(png_struct *ps, png_byte *d, png_size_t length) {
    auto &param = *reinterpret_cast<PNGReadParam *>(png_get_io_ptr(ps));

    auto *_d = reinterpret_cast<storm::core::Byte *>(d);

    std::copy(std::begin(param.data) + param.readed,
              std::begin(param.data) + param.readed + length,
              _d);

    param.readed += length;
}
}

using namespace storm;
using namespace storm::image;
using namespace private_;

/*void _png_readFromStream(png_structp png_ptr, core::UInt8* data, core::ArraySize
length) { std::ifstream* stream =
reinterpret_cast<std::ifstream*>(png_get_io_ptr(png_ptr));
    stream->read(reinterpret_cast<char*>(data), length);
}*/

ImageDataSharedPtr ImageLoader::loadPng(const std::filesystem::path &filename) {
    STORM_EXPECTS(!std::empty(filename));

    auto data = core::ByteArray {};

    {
        auto file = std::ifstream { filename, std::ios::binary };

        file.seekg(0u, std::ios::end);

        data.resize(file.tellg());

        file.seekg(0);
        file.read(reinterpret_cast<char *>(std::data(data)), std::size(data));
    }

    return loadPng(data);
}

ImageDataSharedPtr ImageLoader::loadPng(Image::ConstSpan data) {
    STORM_EXPECTS(!std::empty(data));

    auto image_data = std::make_shared<ImageData>();

    auto read_param = PNGReadParam { 8u, data };

    {
        auto sig = reinterpret_cast<png_const_bytep>(std::data(data));
        if (!png_check_sig(sig, 8u))
            throw std::runtime_error { "[libpng] Failed to validate PNG signature" };

        auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr)
            throw std::runtime_error { "[libpng] Failed to init (png_create_read_struct)" };

        auto info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
            throw std::runtime_error { "[libpng] Failed to init (png_create_info_struct)" };

        png_set_read_fn(png_ptr, &read_param, png_read_func);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        auto bit_depth  = 0;
        auto color_type = 0;

        png_get_IHDR(png_ptr,
                     info_ptr,
                     &image_data->extent.width,
                     &image_data->extent.height,
                     &bit_depth,
                     &color_type,
                     nullptr,
                     nullptr,
                     nullptr);
        image_data->channel = png_get_channels(png_ptr, info_ptr);

        switch (color_type) {
            case PNG_COLOR_TYPE_GRAY:
                png_set_expand_gray_1_2_4_to_8(png_ptr);
                image_data->channel = 1;
                break;

            case PNG_COLOR_TYPE_GRAY_ALPHA:
                png_set_gray_to_rgb(png_ptr);
                image_data->channel = 2;
                break;

            case PNG_COLOR_TYPE_RGB: image_data->channel = 3; break;

            case PNG_COLOR_TYPE_RGB_ALPHA: image_data->channel = 4; break;
            case PNG_COLOR_TYPE_PALETTE:
                png_set_palette_to_rgb(png_ptr);
                if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
                png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
                image_data->channel = 4;
                break;

            default: break;
        }

        if (bit_depth == 16) png_set_strip_16(png_ptr);

        else if (bit_depth < 8)
            png_set_packing(png_ptr);

        png_read_update_info(png_ptr, info_ptr);

        const auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);
        image_data->data.resize(image_data->extent.height * row_bytes);

        auto row_pointers = std::vector<core::Byte *> { image_data->extent.height, nullptr };

        auto buff_pos = std::data(image_data->data);

        for (auto i = 0u; i < image_data->extent.height; ++i)
            row_pointers[i] = &buff_pos[row_bytes * i];

        png_read_image(png_ptr, reinterpret_cast<png_bytepp>(std::data(row_pointers)));
        png_read_end(png_ptr, info_ptr);
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    }

    return image_data;
}

ImageDataSharedPtr ImageLoader::loadJpeg(const std::filesystem::path &filename) {
    STORM_EXPECTS(!std::empty(filename));

    auto data = core::ByteArray {};

    {
        auto file = std::ifstream { filename, std::ios::binary };

        file.seekg(0u, std::ios::end);

        data.resize(file.tellg());

        file.seekg(0);
        file.read(reinterpret_cast<char *>(std::data(data)), std::size(data));
    }

    return loadJpeg(data);
}

ImageDataSharedPtr ImageLoader::loadJpeg(Image::ConstSpan data) {
    STORM_EXPECTS(!std::empty(data));

    auto image_data = std::make_shared<ImageData>();

    {
        struct jpeg_decompress_struct info;
        struct jpeg_error_mgr error_mgr;

        info.err             = jpeg_std_error(&error_mgr);
        info.client_data     = nullptr;
        error_mgr.error_exit = jpeg_error_callback;

        jpeg_create_decompress(&info);

        jpeg_mem_src(&info,
                     reinterpret_cast<const unsigned char *>(std::data(data)),
                     std::size(data));
        jpeg_read_header(&info, TRUE);

        jpeg_start_decompress(&info);

        image_data->extent.width  = info.output_width;
        image_data->extent.height = info.output_height;
        image_data->channel       = gsl::narrow_cast<core::UInt8>(info.num_components);

        image_data->data.resize(image_data->extent.width * image_data->extent.height *
                                image_data->channel);

        auto rowptr = std::array<core::Byte *, 1> { nullptr };
        while (info.output_scanline < info.output_height) {
            const auto index =
                image_data->extent.width * image_data->channel * info.output_scanline;
            rowptr[0] = image_data->data.data() + index;
            jpeg_read_scanlines(&info,
                                reinterpret_cast<JSAMPARRAY>(std::data(rowptr)),
                                static_cast<JDIMENSION>(std::size(rowptr)));
        }

        jpeg_finish_decompress(&info);
    }

    return image_data;
}

ImageDataSharedPtr ImageLoader::loadTga(const std::filesystem::path &filename) {
    STORM_EXPECTS(!std::empty(filename));

    auto data = std::make_shared<ImageData>();

    {
        std::ifstream stream(filename.string(), std::ios::binary);

        struct {
            core::Byte magic[12];
            core::Byte header[6];
        } header;

        stream.read(reinterpret_cast<char *>(&header), sizeof(header));

        data->channel = 3u;
        if (std::to_integer<core::UInt8>(header.header[4u]) > 24u) data->channel = 4u;

        data->extent.width = std::to_integer<core::UInt8>(header.header[1u]) * 256u +
                             std::to_integer<core::UInt8>(header.header[0u]);
        data->extent.height = std::to_integer<core::UInt8>(header.header[3u]) * 256u +
                              std::to_integer<core::UInt8>(header.header[2u]);

        const auto bytes = std::to_integer<core::UInt8>(header.header[4u]) / 8u;
        const auto memory_size =
            data->extent.width * data->extent.height * gsl::narrow_cast<unsigned long>(bytes);

        if (std::to_integer<core::UInt8>(header.magic[2u]) == 2u) {
            data->data.resize(memory_size);

            stream.read(reinterpret_cast<char *>(&(data->data[0])),
                        gsl::narrow_cast<std::streamsize>(memory_size));

            for (auto swap = 0u; swap < memory_size; swap += bytes) {
                data->data[swap] ^= data->data[swap + 2] ^= data->data[swap] ^=
                    data->data[swap + 2];
            }
        } else if (std::to_integer<core::UInt8>(header.magic[2]) == 10) {
            auto pixel_count   = data->extent.width * data->extent.height;
            auto current_pixel = 0u;
            auto current_byte  = 0u;

            // Storage For 1 Pixel
            auto color_buffer = std::vector<core::Byte> { bytes };
            do {
                auto chunk_header = 0u;
                ;
                stream.read(reinterpret_cast<char *>(&chunk_header), sizeof(chunk_header));

                if (chunk_header < 128u) {
                    ++chunk_header;
                    for (auto i = 0u; i < chunk_header; ++i) {
                        stream.read(reinterpret_cast<char *>(std::data(color_buffer)), bytes);

                        data->data[current_byte]      = color_buffer[2u];
                        data->data[current_byte + 1u] = color_buffer[1u];
                        data->data[current_byte + 2u] = color_buffer[0u];

                        if (bytes == 4u) data->data[current_byte + 3u] = color_buffer[3u];

                        current_byte += bytes;
                        ++current_pixel;
                    }
                } else {
                    chunk_header -= 127u;

                    stream.read(reinterpret_cast<char *>(std::data(color_buffer)), bytes);

                    for (auto i = 0u; i < chunk_header; ++i) {
                        data->data[current_byte]      = color_buffer[2u];
                        data->data[current_byte + 1u] = color_buffer[1u];
                        data->data[current_byte + 2u] = color_buffer[0u];

                        if (bytes == 4u) data->data[current_byte + 3u] = color_buffer[3u];

                        current_byte += bytes;
                        ++current_pixel;
                    }
                }
            } while (current_pixel < pixel_count);
        }
    }

    return data;
}

ImageDataSharedPtr ImageLoader::loadTga([[maybe_unused]] Image::ConstSpan) {
    // STORM_EXPECTS(!std::empty(data));

    ASSERT(false, "Not implemented yet !");

    return nullptr;
}

ImageDataSharedPtr ImageLoader::loadPPM([[maybe_unused]] const std::filesystem::path &) {
    // STORM_EXPECTS(!std::empty(filename));

    ASSERT(false, "Not implemented yet !");

    return nullptr;
}

ImageDataSharedPtr ImageLoader::loadPPM([[maybe_unused]] Image::ConstSpan) {
    // STORM_EXPECTS(!std::empty(data));

    ASSERT(false, "Not implemented yet !");

    return nullptr;
}

ImageDataSharedPtr ImageLoader::loadHDR(const std::filesystem::path &path) {
    STORM_EXPECTS(!std::empty(path));

    ASSERT(false, "Not implemented yet !");

    return nullptr;
}

ImageDataSharedPtr ImageLoader::loadHDR([[maybe_unused]] Image::ConstSpan) {
    // STORM_EXPECTS(!std::empty(data));

    ASSERT(false, "Not implemented yet !");

    return nullptr;
}

void ImageLoader::savePng([[maybe_unused]] const std::filesystem::path &,
                          [[maybe_unused]] ImageDataSharedPtr &,
                          [[maybe_unused]] Image::CodecArgs) {
    // STORM_EXPECTS(!std::empty(filename));

    ASSERT(false, "Not implemented yet !");
}

void ImageLoader::saveJpeg(const std::filesystem::path &, ImageDataSharedPtr &, Image::CodecArgs) {
    // STORM_EXPECTS(!std::empty(filename));

    ASSERT(false, "Not implemented yet !");
}

void ImageLoader::saveTga([[maybe_unused]] const std::filesystem::path &,
                          [[maybe_unused]] ImageDataSharedPtr &,
                          [[maybe_unused]] Image::CodecArgs) {
    // STORM_EXPECTS(!std::empty(filename));

    ASSERT(false, "Not implemented yet !");
}

void ImageLoader::savePPM(const std::filesystem::path &filename,
                          ImageDataSharedPtr &data,
                          Image::CodecArgs args) {
    STORM_EXPECTS(!std::empty(filename));

    auto stream = std::ofstream {};

    if (args == Image::CodecArgs::BINARY) {
        stream.open(filename.string(), std::ios::out | std::ios::binary);
        if (stream.is_open()) {
            stream << "P6\n";
            stream << data->extent.width;
            stream << " ";
            stream << data->extent.height << "\n";
            stream << 255u << "\n";

            auto array = data->data;
            for (auto i = 0u; i < data->extent.height; ++i) {
                for (auto j = 0u; j < data->extent.width; ++j) {
                    auto base = (i * data->extent.width + j) * data->channel;

                    auto r = array[base];
                    auto g = array[base + 1u];
                    auto b = array[base + 2u];

                    stream.write(reinterpret_cast<char *>(&r), 1u);
                    stream.write(reinterpret_cast<char *>(&g), 1u);
                    stream.write(reinterpret_cast<char *>(&b), 1u);
                }
            }
        }
    } else if (args == Image::CodecArgs::ASCII) {
        stream.open(filename.string(), std::ios::out);
        if (stream.is_open()) {
            stream << "P3\n";
            stream << data->extent.width;
            stream << " ";
            stream << data->extent.height << "\n";
            stream << 255u << "\n";

            auto array = data->data;
            for (auto i = 0u; i < data->extent.height; ++i) {
                for (auto j = 0u; j < data->extent.width; ++j) {
                    auto base = (i * data->extent.width + j) * data->channel;

                    auto r = array[base];
                    auto g = array[base + 1];
                    auto b = array[base + 2];

                    stream << gsl::narrow_cast<core::UInt16>(r) << " ";
                    stream << gsl::narrow_cast<core::UInt16>(g) << " ";
                    stream << gsl::narrow_cast<core::UInt16>(b) << "   " << std::flush;
                }
                stream << "\n";
            }
        }
    }

    stream.close();
}

void ImageLoader::saveHDR([[maybe_unused]] const std::filesystem::path &,
                          [[maybe_unused]] ImageDataSharedPtr &,
                          [[maybe_unused]] Image::CodecArgs) {
    // STORM_EXPECTS(!std::empty(filename));

    ASSERT(false, "Not implemented yet !");
}
