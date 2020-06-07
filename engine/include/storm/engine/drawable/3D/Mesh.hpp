// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/drawable/Drawable.hpp>

namespace storm::engine {
    struct MeshFlag;
    class STORM_PUBLIC Mesh final: public Drawable {
      public:
        static constexpr auto MAX_NUM_JOINTS = 128u;

        Mesh(Engine &engine, const Material &material, std::string name = "");
        ~Mesh() override;

        Mesh(Mesh &&);
        Mesh &operator=(Mesh &&);

        [[nodiscard]] inline const Material &material() const noexcept;

        [[nodiscard]] inline Transform &transform() noexcept;
        [[nodiscard]] inline const Transform &transform() const noexcept;

        void render(render::CommandBuffer &cmb,
                    const render::RenderPass &pass,
                    std::vector<BindableBaseConstObserverPtr> bindables,
                    render::GraphicsPipelineState state) override;

      protected:
        void recomputeBoundingBox() const noexcept override;

      private:
        std::string m_name;

        MaterialConstObserverPtr m_material;
        TransformOwnedPtr m_transform;

        render::HardwareBufferOwnedPtr m_vertex_buffer;
        render::HardwareBufferOwnedPtr m_index_buffer;
    };
} // namespace storm::engine

#include "Mesh.inl"
