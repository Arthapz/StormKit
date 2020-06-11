/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - STL - ///////////
#include <csetjmp>

/////////// - libJPEG - ///////////
#include <jerror.h>
#include <jpeglib.h>

using namespace storm;
using namespace storm::image;

namespace storm::image {
    extern constexpr core::UInt8 getChannelCountFor(Image::Format format) noexcept;
}

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
    STORM_EXPECTS(!std::empty(data));

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
    if (info.output_components == 1) format = Format::R;
    if (info.output_components == 2) format = Format::RG;
    if (info.output_components == 3) format = Format::RGB;

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

    m_extent        = std::move(extent);
    m_channel_count = getChannelCountFor(m_format);
    m_data          = std::move(image_data);
    m_format        = format;

    return std::nullopt;

    if (setjmp(error_data.setjmp_buffer)) {
        jpeg_destroy_decompress(&info);
        return error_data.msg;
    }
}
