#pragma once
#include "ImageData.hpp"

#include <storm/core/NonDefaultInstanciable.hpp>
#include <storm/core/Span.hpp>

namespace storm::image::private_ {
    class ImageLoader: public core::NonDefaultInstanciable {
      public:
        static ImageDataSharedPtr loadPng(const std::filesystem::path &filename);
        static ImageDataSharedPtr loadPng(Image::ConstSpan data);

        static ImageDataSharedPtr loadJpeg(const std::filesystem::path &filename);
        static ImageDataSharedPtr loadJpeg(Image::ConstSpan data);

        static ImageDataSharedPtr loadTga(const std::filesystem::path &filename);
        static ImageDataSharedPtr loadTga(Image::ConstSpan data);

        static ImageDataSharedPtr loadPPM(const std::filesystem::path &filename);
        static ImageDataSharedPtr loadPPM(Image::ConstSpan data);

        static ImageDataSharedPtr loadHDR(const std::filesystem::path &filename);
        static ImageDataSharedPtr loadHDR(Image::ConstSpan data);

        static void savePng(const std::filesystem::path &filename,
                            ImageDataSharedPtr &data,
                            Image::CodecArgs args);

        static void saveJpeg(const std::filesystem::path &filename,
                             ImageDataSharedPtr &data,
                             Image::CodecArgs args);

        static void saveTga(const std::filesystem::path &filename,
                            ImageDataSharedPtr &data,
                            Image::CodecArgs args);

        static void savePPM(const std::filesystem::path &filename,
                            ImageDataSharedPtr &data,
                            Image::CodecArgs args);

        static void saveHDR(const std::filesystem::path &filename,
                            ImageDataSharedPtr &data,
                            Image::CodecArgs args);
    };
} // namespace storm::image::private_
