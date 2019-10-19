// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <filesystem>

#include <storm/core/Assert.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/image/Fwd.hpp>

// based on https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-on-write
namespace storm::image {
    class STORM_PUBLIC Image {
      public:
        using data_type  = core::Byte;
        using size_type  = core::ArraySize;
        using span       = storm::core::span<data_type>;
        using const_span = storm::core::span<const data_type>;

        enum class Format { RGB24, ARGB32, RGB16 };
        enum class Codec { AUTODETECT, JPEG, PNG, TARGA, PPM, UNKNOW };
        enum class CodecArgs {
            BINARY,
            ASCII // for ppm
        };

        struct Extent {
            core::UInt32 width;
            core::UInt32 height;
        };

        explicit Image();
        Image(core::UInt32 width, core::UInt32 height, core::UInt8 channel_count = 3u);
        Image(const std::filesystem::path &filepath, Codec codec = Codec::AUTODETECT);
        Image(const_span data, Codec codec = Codec::AUTODETECT);
        ~Image();

        void loadFromFile(const std::filesystem::path &filepath, Codec codec = Codec::AUTODETECT);
        void loadFromMemory(const_span data, Codec codec = Codec::AUTODETECT);
        void create(core::UInt32 width, core::UInt32 height, core::UInt8 channel_count = 3u);

        span operator[](size_type index);
        const_span operator[](size_type index) const noexcept;
        span operator()(XOffset x_offset, YOffset offset);
        const_span operator()(XOffset x, YOffset offset) const noexcept;

        Extent extent() const noexcept;
        core::UInt8 channels() const noexcept;

        size_type size() const noexcept;
        const_span data() const noexcept;
        span data();

        static Image scale(const Image &src, const Extent &new_extent);
        static Image flipX(const Image &src);
        static Image flipY(const Image &src);
        static Image rotate90(const Image &src);
        static Image rotate180(const Image &src);
        static Image rotate270(const Image &src);

        void saveToFile(const std::filesystem::path &filename,
                        Codec codec    = Codec::AUTODETECT,
                        CodecArgs args = CodecArgs::BINARY);

        void addChannels(core::UInt8 count = 1);

      private:
        void detach();

        private_::ImageDataSharedPtr m_data = nullptr;
    };
} // namespace storm::image
