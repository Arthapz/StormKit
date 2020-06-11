// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Image.hpp"

namespace storm::image {
    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::operator[](core::ArraySize index) noexcept {
        STORM_EXPECTS(index < m_extent.width * m_extent.height);

        return { std::data(m_data) + index, std::data(m_data) + index + channelCount() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::operator[](core::ArraySize index) const noexcept {
        STORM_EXPECTS(index < m_extent.width * m_extent.height);

        return { std::data(data()) + index, std::data(data()) + index + channelCount() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::operator()(core::Offset3u offset) {
        STORM_EXPECTS(offset.x < m_extent.width && offset.y < m_extent.height &&
                      offset.z < m_extent.depth);

        const auto id = offset.x + m_extent.width * (offset.y + m_extent.height * offset.z);

        auto pointer = &m_data[id * m_channel_count];

        return { pointer, pointer + m_channel_count };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::operator()(core::Offset3u offset) const noexcept {
        STORM_EXPECTS(offset.x < m_extent.width && offset.y < m_extent.height &&
                      offset.z < m_extent.depth);

        const auto id = offset.x + m_extent.width * (offset.y + m_extent.height * offset.z);

        const auto pointer = &m_data[id * m_channel_count];

        return { pointer, pointer + m_channel_count };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const core::Extentu &Image::extent() const noexcept { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt8 Image::channelCount() const noexcept { return m_channel_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Format Image::Image::format() const noexcept { return m_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize Image::size() const noexcept { return std::size(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteSpan Image::data() noexcept { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteConstSpan Image::data() const noexcept { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::begin() noexcept { return std::begin(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::begin() const noexcept { return std::begin(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cbegin() const noexcept { return std::cbegin(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::iterator Image::end() noexcept { return std::end(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::end() const noexcept { return std::end(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ByteArray::const_iterator Image::cend() const noexcept { return std::cend(m_data); }
} // namespace storm::image
