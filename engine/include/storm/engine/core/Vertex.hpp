// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <variant>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/VertexBindingDescription.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>

namespace storm::engine {
    class STORM_PUBLIC VertexArray {
      public:
        using VertexData = std::vector<float>;

        explicit VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray &);
        VertexArray &operator=(const VertexArray &);

        VertexArray(VertexArray &&);
        VertexArray &operator=(VertexArray &&);

        template<typename VertexType>
        inline core::span<VertexType> iteratorProxy() noexcept;
        template<typename VertexType>
        inline core::span<const VertexType> iteratorProxy() const noexcept;

        inline VertexData::iterator begin() noexcept;
        inline VertexData::const_iterator begin() const noexcept;
        inline VertexData::const_iterator cbegin() const noexcept;

        inline VertexData::iterator end() noexcept;
        inline VertexData::const_iterator end() const noexcept;
        inline VertexData::const_iterator cend() const noexcept;

        inline void clear();
        inline void resize(core::ArraySize size);
        inline void reserve(core::ArraySize size);

        inline void push_back(float value);
        inline void pop_back();

        inline float &operator[](core::ArraySize index) noexcept;
        inline const float &operator[](core::ArraySize index) const noexcept;

        template<typename VertexType>
        inline VertexType &at(core::ArraySize index);
        template<typename VertexType>
        inline const VertexType &at(core::ArraySize index) const;

        template<typename VertexType>
        inline core::ArraySize count() const noexcept;

        inline core::ArraySize size() const noexcept;
        inline bool empty() const noexcept;

        inline core::span<float> data() noexcept;
        inline core::span<const float> data() const noexcept;

        inline core::span<std::byte> bytes() noexcept;
        inline core::span<const std::byte> bytes() const noexcept;

        // TODO raw access to bytes
        inline render::VertexBindingDescriptionConstSpan bindingDescriptions() const noexcept;
        inline render::VertexInputAttributeDescriptionConstSpan
            inputAttributeDescriptions() const noexcept;

        inline core::Hash64 hash() const noexcept;

        inline bool operator==(const VertexArray &other) const noexcept;

      private:
        render::VertexBindingDescriptionArray m_binding_descriptions;
        render::VertexInputAttributeDescriptionArray m_attribute_descriptions;

        VertexData m_data;

        mutable core::Hash64 m_hash;
        mutable bool m_need_update_hash = true;
    };

    using IndexArray     = std::vector<core::UInt16>;
    using IndexSpan      = core::span<core::UInt16>;
    using IndexConstSpan = core::span<const core::UInt16>;

    using LargeIndexArray     = std::vector<core::UInt32>;
    using LargeIndexSpan      = core::span<core::UInt32>;
    using LargeIndexConstSpan = core::span<const core::UInt32>;

    using IndexArrayProxy     = std::variant<IndexArray, LargeIndexArray>;
    using IndexSpanProxy      = std::variant<IndexSpan, LargeIndexSpan>;
    using IndexConstSpanProxy = std::variant<IndexConstSpan, LargeIndexConstSpan>;

} // namespace storm::engine

HASH_FUNC(storm::engine::VertexArray)

#include "Vertex.inl"
