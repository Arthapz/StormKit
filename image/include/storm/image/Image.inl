// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::image {
    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::pixel(core::ArraySize index, core::UInt8 mip_level) noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);

        auto &mip = m_data[mip_level];

        STORM_EXPECTS(index < mip.extent.width * mip.extent.height * mip.extent.depth);

        return { std::data(mip.data) + index,
                 std::data(mip.data) + index + m_channel_count + m_bytes_per_channel };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::pixel(core::ArraySize index, core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);

        const auto &mip = m_data[mip_level];

        STORM_EXPECTS(index < mip.extent.width * mip.extent.height * mip.extent.depth);

        return { std::data(mip.data) + index,
                 std::data(mip.data) + index + m_channel_count + m_bytes_per_channel };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::pixel(core::Offset3u offset, core::UInt8 mip_level) {
        const auto &mip = m_data[mip_level];

        const auto id = offset.x + mip.extent.width * (offset.y + mip.extent.height * offset.z);

        return pixel(id, mip_level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::pixel(core::Offset3u offset, core::UInt8 mip_level) const noexcept {
        const auto &mip = m_data[mip_level];

        const auto id = offset.x + mip.extent.width * (offset.y + mip.extent.height * offset.z);

        return pixel(id, mip_level);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Extentu &Image::extent(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return m_data[mip_level].extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt8 Image::channelCount() const noexcept { return m_channel_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt8 Image::bytesPerChannel() const noexcept { return m_bytes_per_channel; }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Format Image::Image::format() const noexcept { return m_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::size(m_data[mip_level].data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::data(core::UInt8 mip_level) noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return m_data[mip_level].data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::data(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return m_data[mip_level].data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::begin(core::UInt8 mip_level) noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::begin(m_data[mip_level].data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::begin(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::begin(m_data[mip_level].data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cbegin(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::cbegin(m_data[mip_level].data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::end(core::UInt8 mip_level) noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::end(m_data[mip_level].data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::end(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::end(m_data[mip_level].data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cend(core::UInt8 mip_level) const noexcept {
        STORM_EXPECTS(m_mip_levels > mip_level);
        return std::cend(m_data[mip_level].data);
    }
} // namespace storm::image
