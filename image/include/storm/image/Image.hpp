// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <filesystem>

#include <storm/core/Assert.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/image/Fwd.hpp>

// based on https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-on-write
namespace storm::image {
    class STORM_PUBLIC Image {
      public:
        using data_type = core::Byte;
        using size_type = core::ArraySize;
        using span      = storm::core::span<data_type>;
        using ConstSpan = storm::core::span<const data_type>;

        enum class Format { RGB24, ARGB32, RGB16 };
        enum class Codec { AUTODETECT, JPEG, PNG, TARGA, PPM, HDR, KTX, RAW, UNKNOW };
        enum class CodecArgs {
            BINARY,
            ASCII // for ppm
        };

        explicit Image();
        Image(core::UInt32 width, core::UInt32 height, core::UInt8 channel_count = 3u);
        Image(const std::filesystem::path &filepath, Codec codec = Codec::AUTODETECT);
        Image(ConstSpan data, Codec codec = Codec::AUTODETECT);
        ~Image();

        void loadFromFile(const std::filesystem::path &filepath, Codec codec = Codec::AUTODETECT);
        void loadFromMemory(ConstSpan data, Codec codec = Codec::AUTODETECT);
        void create(core::UInt32 width, core::UInt32 height, core::UInt8 channel_count = 3u);

        span operator[](size_type index);
        ConstSpan operator[](size_type index) const noexcept;
        span operator()(XOffset x_offset, YOffset offset);
        ConstSpan operator()(XOffset x, YOffset offset) const noexcept;

        core::Extentu extent() const noexcept;
        core::UInt8 channels() const noexcept;

        size_type size() const noexcept;
        ConstSpan data() const noexcept;
        span data();

        static Image scale(const Image &src, const core::Extentu &new_extent);
        static Image flipX(const Image &src);
        static Image flipY(const Image &src);
        static Image rotate90(const Image &src);
        static Image rotate180(const Image &src);
        static Image rotate270(const Image &src);

        void saveToFile(const std::filesystem::path &filename,
                        Codec codec    = Codec::AUTODETECT,
                        CodecArgs args = CodecArgs::BINARY);

        void addChannels(core::UInt8 count = 1);

        inline Codec codec() const noexcept { return m_codec; }

      private:
        void detach();

        Codec m_codec;

        private_::ImageDataSharedPtr m_data = nullptr;
    };
} // namespace storm::image
