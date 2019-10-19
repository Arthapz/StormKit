#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/System.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/core/Transform.hpp>

namespace storm::engine {
    class STORM_PUBLIC StaticMeshRenderSystem: public entities::System {
      public:
        StaticMeshRenderSystem(entities::EntityManager &manager, Engine &engine);
        ~StaticMeshRenderSystem() override;

        StaticMeshRenderSystem(StaticMeshRenderSystem &&);
        StaticMeshRenderSystem &operator=(StaticMeshRenderSystem &&);

        void update(core::UInt64 delta) noexcept override;

        void render(render::CommandBuffer &cmb,
                    std::string_view pass_name,
                    const render::RenderPass &render_pass,
                    std::vector<render::DescriptorSetCRef> descriptor_sets = {},
                    std::vector<core::UOffset> dynamic_offsets             = {});

      protected:
        void onMessageReceived(const entities::Message &message) override;

      private:
        EngineObserverPtr m_engine;

        TransformOwnedPtr m_default_transform;
        render::DescriptorSetOwnedPtr m_default_transform_descriptor_set;
    };
} // namespace storm::engine