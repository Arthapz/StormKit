#pragma once
#include "ImageData.hpp"

#include <storm/core/NonDefaultInstanciable.hpp>
#include <storm/core/Span.hpp>

namespace storm::image::private_ {
	class ImageLoader : public core::NonDefaultInstanciable {
	  public:
		static ImageDataSharedPtr
			loadPng(const core::filesystem::path &filename);
		static ImageDataSharedPtr loadPng(Image::const_span data);
		static ImageDataSharedPtr
			loadJpeg(const core::filesystem::path &filename);
		static ImageDataSharedPtr loadJpeg(Image::const_span data);
		static ImageDataSharedPtr
			loadTga(const core::filesystem::path &filename);
		static ImageDataSharedPtr loadTga(Image::const_span data);
		static ImageDataSharedPtr
			loadPPM(const core::filesystem::path &filename);
		static ImageDataSharedPtr loadPPM(Image::const_span data);

		static void savePng(const core::filesystem::path &filename,
							ImageDataSharedPtr &data, Image::CodecArgs args);
		static void saveJpeg(const core::filesystem::path &filename,
							 ImageDataSharedPtr &data, Image::CodecArgs args);
		static void saveTga(const core::filesystem::path &filename,
							ImageDataSharedPtr &data, Image::CodecArgs args);
		static void savePPM(const core::filesystem::path &filename,
							ImageDataSharedPtr &data, Image::CodecArgs args);
	};
} // namespace storm::image::private_
