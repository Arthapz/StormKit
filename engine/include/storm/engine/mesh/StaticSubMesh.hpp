// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/DeferredStackAllocation.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/core/Math.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>

#include <storm/render/resource/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/core/Vertex.hpp>

#include <storm/engine/mesh/Fwd.hpp>

#include <storm/engine/mesh/BoundingBox.hpp>

namespace storm::engine {
    class STORM_PUBLIC StaticSubMesh: public core::NonCopyable {
      public:
        explicit StaticSubMesh(core::UInt32 vertex_count,
                               core::UInt32 first_index,
                               core::UInt32 index_count) noexcept;
        ~StaticSubMesh();

        StaticSubMesh(StaticSubMesh &&);
        StaticSubMesh &operator=(StaticSubMesh &&);

        inline void setMaterialID(core::UInt32 index) noexcept;
        inline void setBoundingBox(core::Vector3f min, core::Vector3f max) noexcept;

        [[nodiscard]] inline core::UInt32 materialID() const noexcept;
        [[nodiscard]] inline const BoundingBox &boundingBox() const noexcept;
        [[nodiscard]] inline core::UInt32 vertexCount() const noexcept;
        [[nodiscard]] inline core::UInt32 indexCount() const noexcept;
        [[nodiscard]] inline core::UInt32 firstIndex() const noexcept;

        [[nodiscard]] inline bool isIndexed() const noexcept;

      private:
        core::UInt32 m_material_id = 0;

        core::UInt32 m_first_index  = 0;
        core::UInt32 m_index_count  = 0;
        core::UInt32 m_vertex_count = 0;

        BoundingBox m_bounding_box;
        friend class StaticMesh;
    };
} // namespace storm::engine

#include "StaticSubMesh.inl"
