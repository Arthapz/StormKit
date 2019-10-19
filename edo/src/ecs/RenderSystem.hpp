#pragma once

#include <storm/core/Memory.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>
#include <storm/render/resource/Fwd.hpp>

#include <storm/graphics/core/Fwd.hpp>
#include <storm/graphics/core/Renderer.hpp>

#include <storm/entities/System.hpp>

class RenderSystem : public storm::entities::System {
  public:
    explicit RenderSystem(storm::entities::EntityManager &manager,
                          const storm::render::Device &device,
                          storm::render::Surface &surface);
    ~RenderSystem() override;

    RenderSystem(RenderSystem &&);
    RenderSystem &operator=(RenderSystem &&);

    void update(std::uint64_t delta) override;
    void postUpdate() override;

    inline auto layout() const noexcept {
        return m_renderer->defaultLayout();
    }

    void present();

  protected:
    void onMessageReceived(const storm::entities::Message &message) override;

  private:
    void initFrameGraph();

    storm::render::DeviceConstObserverPtr m_device;
    storm::render::SurfaceObserverPtr m_surface;

    storm::graphics::RendererOwnedPtr m_renderer;

    std::unordered_map<storm::entities::Entity, storm::graphics::MeshOwnedPtr>
        m_meshes;
};

DECLARE_PTR_AND_REF(RenderSystem)
