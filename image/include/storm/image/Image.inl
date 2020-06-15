// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Image.hpp"

namespace storm::image {
    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::pixel(core::ArraySize index, core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        auto &data = data(layer, face, level);

        STORM_EXPECTS(index < mip.extent.width * mip.extent.height * mip.extent.depth);

        return { std::data(data) + index,
                 std::data(data) + index + m_channel_count + m_bytes_per_channel };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::pixel(core::ArraySize index, core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        auto &data = data(layer, face, level);

        STORM_EXPECTS(index < mip.extent.width * mip.extent.height * mip.extent.depth);

        return { std::data(data) + index,
                 std::data(data) + index + m_channel_count + m_bytes_per_channel };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::pixel(core::Offset3u offset, core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        const auto mip_extent = extent(level);

        const auto id = offset.x + mip_extent.width * (offset.y + mip_extent.height * offset.z);

        return pixel(id, layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::pixel(core::Offset3u offset, core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        const auto mip_extent = extent(level);

        const auto id = offset.x + mip_extent.width * (offset.y + mip_extent.height * offset.z);

        return pixel(id, layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Extentu &Image::extent(core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);

        return { .width = m_extent.width >> level, .height = m_extent.height >> level, .depth = m_extent.depth >> level };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt8 Image::channelCount() const noexcept { return m_channel_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt8 Image::bytesPerChannel() const noexcept { return m_bytes_per_channel; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt8 Image::mipLevels() const noexcept { return m_mip_levels; }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Format Image::layers() const noexcept { return m_layers; }
    .
    /////////////////////////////////////
    /////////////////////////////////////
    Image::Format Image::format() const noexcept { return m_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size() const noexcept {
        return std::size(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        return mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count * m_bytes_per_channel;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::data() const noexcept {
        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return {begin(layer, face, level), end(layer, face, level)};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::data() const noexcept {
        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        return {cbegin(layer, face, level), cend(layer, face, level)};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::begin() noexcept {
        return std::begin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        return std::begin(data(layer, face, level));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::begin() const noexcept {
        return std::begin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        const auto layer_size = m_extent.width * m_extent.height * m_extent.depth * m_channel_count * m_bytes_per_channel;
        const auto mip_size = mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count * m_bytes_per_channel;

        return std::begin(m_data) + layer * layer_size + face * layer_size + level * mip_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cbegin() const noexcept {
        return std::cbegin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cbegin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        const auto layer_size = m_extent.width * m_extent.height * m_extent.depth * m_channel_count * m_bytes_per_channel;
        const auto mip_size = mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count * m_bytes_per_channel;

        return std::begin(m_data) + layer * layer_size + face * layer_size + level * mip_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::end() noexcept {
        return std::end(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        const auto layer_size = m_extent.width * m_extent.height * m_extent.depth * m_channel_count * m_bytes_per_channel;
        const auto mip_size = mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count * m_bytes_per_channel;

        return std::begin(m_data) + layer * layer_size + face * layer_size + level * mip_size + size(layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::end() const noexcept {
        return std::end(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        const auto layer_size = m_extent.width * m_extent.height * m_extent.depth * m_channel_count * m_bytes_per_channel;
        const auto mip_size = mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count * m_bytes_per_channel;

        return std::begin(m_data) + layer * layer_size + face * layer_size + level * mip_size + size(layer, face, level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cend() const noexcept {
        return std::cend(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cend(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept {
        STORM_EXPECTS(m_mip_levels > level);
        STORM_EXPECTS(m_faces > face);
        STORM_EXPECTS(m_layers > layer);

        const auto mip_extent = extent(level);

        const auto layer_size = m_extent.width * m_extent.height * m_extent.depth * m_channel_count * m_bytes_per_channel;
        const auto mip_size = mip_extent.width * mip_extent.height * mip_extent.depth * m_channel_count * m_bytes_per_channel;

        return std::cbegin(m_data) + layer * layer_size + face * layer_size + level * mip_size + size(layer, face, level);
    }
} // namespace storm::image
