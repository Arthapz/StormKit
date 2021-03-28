// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::indexCount() const noexcept -> core::ArraySize {
        return std::size(m_indices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::vertices() const noexcept -> core::ByteConstSpan { return m_vertices.data(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::indices() const noexcept -> core::ByteConstSpan {
        return core::toConstByteSpan(m_indices);
    }
} // namespace storm::engine
