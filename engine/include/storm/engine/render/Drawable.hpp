// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Vertex.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC Drawable : public core::NonCopyable {
      public:
        Drawable() noexcept;
        virtual ~Drawable();

        Drawable(Drawable &&) noexcept;
        Drawable &operator=(Drawable &&) noexcept;

        [[nodiscard]] core::ArraySize indexCount() const noexcept;

        [[nodiscard]] core::ByteConstSpan vertices() const noexcept;
        [[nodiscard]] core::ByteConstSpan indices() const noexcept;

        [[nodiscard]] virtual constexpr core::ArraySize vertexSize() const noexcept = 0;
      protected:
        VertexArray m_vertices;
        LargeIndexArray m_indices;
    };
}

HASH_FUNC(storm::engine::Drawable)

#include "Drawable.inl"
