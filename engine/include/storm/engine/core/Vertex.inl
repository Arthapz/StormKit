// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
     core::span<VertexType> VertexArray::iteratorProxy() noexcept {
        return { reinterpret_cast<VertexType *>(std::data(m_data)), count<VertexType>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
     core::span<const VertexType> VertexArray::iteratorProxy() const noexcept {
        return { reinterpret_cast<const VertexType *>(std::data(m_data)), count<VertexType>() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
     VertexArray::VertexData::iterator VertexArray::begin() noexcept {
        return core::ranges::begin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     VertexArray::VertexData::const_iterator VertexArray::begin() const noexcept {
        return std::cbegin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     VertexArray::VertexData::const_iterator VertexArray::cbegin() const noexcept {
        return std::cbegin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     VertexArray::VertexData::iterator VertexArray::end() noexcept {
        return core::ranges::end(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     VertexArray::VertexData::const_iterator VertexArray::end() const noexcept {
        return std::cend(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     VertexArray::VertexData::const_iterator VertexArray::cend() const noexcept {
        return std::cend(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void VertexArray::clear() {
        m_need_update_hash = true;
        m_data.clear();
    }

    /////////////////////////////////////
    /////////////////////////////////////
     void VertexArray::resize(core::ArraySize size) { m_data.resize(size); }

    /////////////////////////////////////
    /////////////////////////////////////
     void VertexArray::reserve(core::ArraySize size) { m_data.reserve(size); }

    /////////////////////////////////////
    /////////////////////////////////////
     void VertexArray::push_back(float v) { m_data.push_back(v); }

    /////////////////////////////////////
    /////////////////////////////////////
     void VertexArray::pop_back() { m_data.pop_back(); }

    /////////////////////////////////////
    /////////////////////////////////////
     float &VertexArray::operator[](core::ArraySize index) noexcept {
        STORM_EXPECTS(index < size());
        m_need_update_hash = true;
        return m_data[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
     const float &VertexArray::operator[](core::ArraySize index) const noexcept {
        STORM_EXPECTS(index < size());
        return m_data[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
     VertexType &VertexArray::at(core::ArraySize index) {
        m_need_update_hash = true;
        const auto size    = count<VertexType>();
        STORM_EXPECTS(index < size);

        return iteratorProxy<VertexType>()[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
     const VertexType &VertexArray::at(core::ArraySize index) const {
        const auto size = count<VertexType>();
        STORM_EXPECTS(index < size);

        return iteratorProxy<VertexType>()[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename VertexType>
     core::ArraySize VertexArray::count() const noexcept {
        static constexpr auto VERTEX_SIZE = sizeof(VertexType);
        return size() / VERTEX_SIZE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::ArraySize VertexArray::size() const noexcept { return std::size(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
     bool VertexArray::empty() const noexcept { return std::empty(m_data); }

    /////////////////////////////////////
    /////////////////////////////////////
     core::span<float> VertexArray::data() noexcept {
        m_need_update_hash = true;
        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::span<const float> VertexArray::data() const noexcept { return m_data; }

    /////////////////////////////////////
    /////////////////////////////////////
     core::span<std::byte> VertexArray::bytes() noexcept {
        m_need_update_hash = true;
        return { reinterpret_cast<std::byte *>(std::data(m_data)),
                 std::size(m_data) * sizeof(float) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::ByteConstSpan VertexArray::bytes() const noexcept {
        return { reinterpret_cast<const std::byte *>(std::data(m_data)),
                 std::size(m_data) * sizeof(float) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
     render::VertexBindingDescriptionConstSpan VertexArray::bindingDescriptions() const
        noexcept {
        return m_binding_descriptions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     render::VertexInputAttributeDescriptionConstSpan
        VertexArray::inputAttributeDescriptions() const noexcept {
        return m_attribute_descriptions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     core::Hash64 VertexArray::hash() const noexcept {
        if (m_need_update_hash) {
            m_hash = 0u;
            for (auto component : m_data) core::hash_combine(m_hash, component);

            m_need_update_hash = false;
        }

        return m_hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
     bool VertexArray::operator==(const VertexArray &other) const noexcept {
        return hash() == other.hash();
    }
} // namespace storm::engine
