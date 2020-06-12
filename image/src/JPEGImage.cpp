/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - STL - ///////////
#include <csetjmp>
#include <cstring>

/////////// - libJPEG - ///////////
#include <jerror.h>
#include <jpeglib.h>

using namespace storm;
using namespace storm::image;

struct JpegErrorData {
    std::jmp_buf setjmp_buffer;
    std::string msg;
};

static void jpeg_error_callback(jpeg_common_struct *st) noexcept {
    STORM_EXPECTS(st != nullptr);

    auto error_data = reinterpret_cast<JpegErrorData *>(st->client_data);

    auto message = std::string {};
    message.resize(JMSG_STR_PARM_MAX);
    (*st->err->format_message)(st, std::data(message));

    error_data->msg = message;

    std::longjmp(error_data->setjmp_buffer, 1);
}

std::optional<std::string> Image::loadJPEG(core::ByteConstSpan data) noexcept {
    auto image_data = core::ByteArray {};
    auto format     = Format {};
    auto extent     = core::Extentu {};

    auto info      = jpeg_decompress_struct {};
    auto error_mgr = jpeg_error_mgr {};

    auto error_data = JpegErrorData {};

    info.err             = jpeg_std_error(&error_mgr);
    info.client_data     = &error_data;
    error_mgr.error_exit = jpeg_error_callback;

    jpeg_create_decompress(&info);
    if (setjmp(error_data.setjmp_buffer)) { return error_data.msg; }

    jpeg_mem_src(&info, reinterpret_cast<const unsigned char *>(std::data(data)), std::size(data));
    jpeg_read_header(&info, TRUE);

    jpeg_start_decompress(&info);
    extent.width  = info.output_width;
    extent.height = info.output_height;
    extent.depth  = 1;
    if (info.output_components == 1) format = Format::R8_UNorm;
    if (info.output_components == 2) format = Format::RG8_UNorm;
    if (info.output_components == 3) format = Format::RGB8_UNorm;

    image_data.resize(extent.width * extent.height * extent.depth * info.out_color_components);

    auto row_ptr = std::array<core::Byte *, 1> { nullptr };
    while (info.output_scanline < info.output_height) {
        const auto index = extent.width * info.output_components * info.output_scanline;
        row_ptr[0]       = std::data(image_data) + index;
        jpeg_read_scanlines(&info,
                            reinterpret_cast<JSAMPARRAY>(std::data(row_ptr)),
                            static_cast<JDIMENSION>(std::size(row_ptr)));
    }

    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);

    auto mip = MipLevel { .extent = std::move(extent), .data = std::move(image_data) };

    m_data.clear();
    m_data.emplace_back(std::move(mip));
    m_channel_count     = getChannelCountFor(m_format);
    m_format            = format;
    m_bytes_per_channel = 1u;

    return std::nullopt;

    if (setjmp(error_data.setjmp_buffer)) {
        jpeg_destroy_decompress(&info);
        return error_data.msg;
    }
}

std::optional<std::string> Image::saveJPEG(const std::filesystem::path &filepath) const noexcept {
    auto _filename = filepath;

    auto this_rgb = toFormat(Format::RGB8_UNorm);

    auto info      = jpeg_compress_struct {};
    auto error_mgr = jpeg_error_mgr {};

    auto error_data = JpegErrorData {};
    for (auto i = 0u; i < this_rgb.mipLevels(); ++i) {
        if (i >= 1u) _filename += fmt::format("_mip{}", i);

        auto file = std::fopen(_filename.string().c_str(), "w");
        if (file == nullptr) {
            auto error = std::string { 95 };
            strerror_s(std::data(error), std::size(error), errno);
            error.shrink_to_fit();

            return error;
        }

        auto data          = this_rgb.data(i);
        const auto &extent = this_rgb.extent(i);

        info.err             = jpeg_std_error(&error_mgr);
        info.client_data     = &error_data;
        error_mgr.error_exit = jpeg_error_callback;

        jpeg_create_compress(&info);
        jpeg_stdio_dest(&info, file);

        info.image_width      = extent.width;
        info.image_height     = extent.height;
        info.input_components = getChannelCountFor(m_format);
        info.in_color_space   = JCS_RGB;

        using JpegBuffer = unsigned char *;
        using JpegSize   = unsigned long;

        auto buffer      = JpegBuffer { nullptr };
        auto buffer_size = JpegSize {};
        jpeg_mem_dest(&info, &buffer, &buffer_size);

        jpeg_set_defaults(&info);
        jpeg_set_quality(&info, 70, true);

        jpeg_start_compress(&info, true);

        auto row_ptr = std::array<core::Byte *, 1> { nullptr };
        while (info.next_scanline < info.image_height) {
            const auto index = info.next_scanline * 4u * info.image_width;
            row_ptr[0]       = std::data(data) + index;

            jpeg_write_scanlines(&info, reinterpret_cast<JSAMPARRAY>(std::data(row_ptr)), 1);
        }

        jpeg_finish_compress(&info);
        jpeg_destroy_compress(&info);

        std::fclose(file);
    }

    if (setjmp(error_data.setjmp_buffer)) {
        jpeg_destroy_compress(&info);
        return error_data.msg;
    }

    return std::nullopt;
}
