// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/Assert.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::image - ///////////
#include <storm/image/Fwd.hpp>

namespace storm::image {
    class STORM_PUBLIC Image {
      public:
        enum class Format { R, RG, RGB, RGBA, BGR, BGRA, UNKNOW };
        enum class Codec { AUTODETECT, JPEG, PNG, TARGA, PPM, HDR, KTX, UNKNOW };
        enum class CodecArgs {
            BINARY,
            ASCII // for ppm
        };

        explicit Image() noexcept;
        Image(core::Extentu extent, Format format) noexcept;
        Image(const std::filesystem::path &filepath, Codec codec = Codec::AUTODETECT) noexcept;
        Image(core::ByteConstSpan data, Codec codec = Codec::AUTODETECT) noexcept;
        ~Image() noexcept;

        bool loadFromFile(const std::filesystem::path &filepath,
                          Codec codec = Codec::AUTODETECT) noexcept;
        bool loadFromMemory(core::ByteConstSpan data, Codec codec = Codec::AUTODETECT) noexcept;
        bool saveToFile(const std::filesystem::path &filename,
                        Codec codec,
                        CodecArgs args = CodecArgs::BINARY) const noexcept;

        void create(core::Extentu extent, Format format) noexcept;

        Image toFormat(Format format) noexcept;
        Image scale(const core::Extentu &scale_to) noexcept;
        Image flipX() noexcept;
        Image flipY() noexcept;
        Image flipZ() noexcept;
        Image rotate90() noexcept;
        Image rotate180() noexcept;
        Image rotate270() noexcept;

        [[nodiscard]] inline core::ByteSpan operator[](core::ArraySize index) noexcept;
        [[nodiscard]] inline core::ByteConstSpan operator[](core::ArraySize index) const noexcept;
        [[nodiscard]] inline core::ByteSpan operator()(core::Offset3u offset);
        [[nodiscard]] inline core::ByteConstSpan operator()(core::Offset3u offset) const noexcept;

        [[nodiscard]] inline const core::Extentu &extent() const noexcept;
        [[nodiscard]] inline core::UInt8 channelCount() const noexcept;
        [[nodiscard]] inline Format format() const noexcept;

        [[nodiscard]] inline core::ArraySize size() const noexcept;
        [[nodiscard]] inline core::ByteSpan data() noexcept;
        [[nodiscard]] inline core::ByteConstSpan data() const noexcept;

        [[nodiscard]] inline core::ByteArray::iterator begin() noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator begin() const noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator cbegin() const noexcept;

        [[nodiscard]] inline core::ByteArray::iterator end() noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator end() const noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator cend() const noexcept;

      private:
        std::optional<std::string> loadJPEG(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadPNG(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadTARGA(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadPPM(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadHDR(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadKTX(core::ByteConstSpan data) noexcept;

        std::optional<std::string> saveJPEG(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> saveTGA(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> savePNG(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> savePPM(const std::filesystem::path &filepath, CodecArgs args) const noexcept;
        std::optional<std::string> saveTARGA(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> saveHDR(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> saveKTX(const std::filesystem::path &filepath) const noexcept;

        core::Extentu m_extent      = { .width = 0u, .height = 0u, .depth = 1u };
        core::UInt8 m_channel_count = 0u;
        Format m_format             = Format::UNKNOW;
        core::ByteArray m_data;
    };
} // namespace storm::image
