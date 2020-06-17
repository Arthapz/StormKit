/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

#include <png.h>

using namespace storm;
using namespace storm::image;

struct PNGReadParam {
    core::ArraySize readed;
    core::ByteConstSpan &data;
};

void png_read_func(png_struct *ps, png_byte *d, png_size_t length) noexcept {
    auto &param = *reinterpret_cast<PNGReadParam *>(png_get_io_ptr(ps));

    auto *_d = reinterpret_cast<storm::core::Byte *>(d);

    std::copy(std::begin(param.data) + param.readed,
              std::begin(param.data) + param.readed + length,
              _d);

    param.readed += length;
}

std::optional<std::string> Image::loadPNG(core::ByteConstSpan data) noexcept {
    auto image_data = core::ByteArray {};
    auto format     = Format {};
    auto extent     = core::Extentu {};

    auto read_param = PNGReadParam { 8u, data };

    auto sig = reinterpret_cast<png_const_bytep>(std::data(data));
    if (!png_check_sig(sig, 8u)) return "[libpng] Failed to validate PNG signature";

    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return "[libpng] Failed to init (png_create_read_struct)";

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        return "[libpng] Failed to init (png_create_info_struct)";
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    }

    png_set_read_fn(png_ptr, &read_param, png_read_func);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    auto bit_depth  = 0;
    auto color_type = 0;

    png_get_IHDR(png_ptr,
                 info_ptr,
                 &extent.width,
                 &extent.height,
                 &bit_depth,
                 &color_type,
                 nullptr,
                 nullptr,
                 nullptr);

    if (color_type == PNG_COLOR_TYPE_GRAY) png_set_expand_gray_1_2_4_to_8(png_ptr);
    else if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }
    if (bit_depth < 8) png_set_packing(png_ptr);

    png_get_IHDR(png_ptr,
                 info_ptr,
                 &extent.width,
                 &extent.height,
                 &bit_depth,
                 &color_type,
                 nullptr,
                 nullptr,
                 nullptr);

    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY: {
            if (bit_depth == 8) format = Format::R8_UNorm;
            else if (bit_depth == 16)
                format = Format::R16_UNorm;

            break;
        }
        case PNG_COLOR_TYPE_GRAY_ALPHA: {
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
            if (bit_depth == 8) format = Format::RG8_UNorm;
            else if (bit_depth == 16)
                format = Format::RG16_UNorm;

            break;
        }
        case PNG_COLOR_TYPE_RGB: {
            if (bit_depth == 8) format = Format::RGB8_UNorm;
            else if (bit_depth == 16)
                format = Format::RGB16_UNorm;

            break;
        }
        case PNG_COLOR_TYPE_RGB_ALPHA: {
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
            if (bit_depth == 8) format = Format::RGBA8_UNorm;
            else if (bit_depth == 16)
                format = Format::RGBA16_UNorm;

            break;
        }
        case PNG_COLOR_TYPE_PALETTE: {
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
            if (bit_depth == 8) format = Format::RGBA8_UNorm;
            else if (bit_depth == 16)
                format = Format::RGBA16_UNorm;

            break;
        }

        default: break;
    }

    png_read_update_info(png_ptr, info_ptr);

    const auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    image_data.resize(extent.height * row_bytes);

    auto row_pointers = std::vector<core::Byte *> { extent.height, nullptr };

    auto buff_pos = std::data(image_data);

    for (auto i = 0u; i < extent.height; ++i) row_pointers[i] = &buff_pos[row_bytes * i];

    png_read_image(png_ptr, reinterpret_cast<png_bytepp>(std::data(row_pointers)));
    png_read_end(png_ptr, info_ptr);

    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);

    m_extent            = std::move(extent);
    m_channel_count     = getChannelCountFor(format);
    m_bytes_per_channel = getByteCountByChannelFor(format);
    m_mip_levels        = 1u;
    m_faces             = 1u;
    m_layers            = 1u;
    m_data              = std::move(image_data);
    m_format            = format;

    return std::nullopt;
}

std::optional<std::string> Image::savePNG(const std::filesystem::path &filepath) const noexcept {
    auto _filename = filepath;

    return std::nullopt;
}
