// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Drawable.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC PbrMesh : public Drawable {
      public:
        struct alignas(16) Vertex {
            core::Vector3f position;
            core::Vector3f normal;
            core::Vector2f uv;
        };

        PbrMesh() noexcept;
        ~PbrMesh() override;

        PbrMesh(PbrMesh &&) noexcept;
        PbrMesh &operator=(PbrMesh &&) noexcept;

        void setVertices(VertexArray vertices);
        void setIndices(LargeIndexArray indices);

        [[nodiscard]] constexpr core::ArraySize vertexSize() const noexcept override;

        ALLOCATE_HELPERS(PbrMesh)
      private:
    };
} // namespace storm::engine


#include "PbrMesh.inl"
