/////////// - StormKit::render - ///////////
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>

#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Camera.hpp>
#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/material/Material.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Scene::Scene(Engine &engine) : m_engine { &engine } {
    m_default_camera =
        std::make_unique<Camera>(engine,
                                 Camera::Type::Perspective,
                                 m_engine->surface().extent().convertTo<core::Extentf>());
    m_camera = core::makeObserver(m_default_camera);

    const auto &extent = m_engine->surface().extent();

    auto viewport_extent = extent.convertTo<core::Extentf>();
    viewport_extent.h    = -viewport_extent.h;

    m_pipeline_state.viewport_state.viewports =
        std::vector { render::Viewport { .position = { 0, extent.h },
                                         .extent   = viewport_extent,
                                         .depth    = { 0.f, 1.f } } };
    m_pipeline_state.viewport_state.scissors =
        std::vector { render::Scissor { .offset = { 0, 0 }, .extent = extent } };
}

////////////////////////////////////////
////////////////////////////////////////
Scene::~Scene() = default;

////////////////////////////////////////
////////////////////////////////////////
Scene::Scene(Scene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Scene &Scene::operator=(Scene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Scene::update(float delta) noexcept {
    m_camera->update(delta);
}

////////////////////////////////////////
////////////////////////////////////////
void Scene::render(FrameGraph &framegraph, storm::engine::FramePassTextureID backbuffer) noexcept {
    m_camera->flush();

    doRenderScene(framegraph, backbuffer, { core::makeConstObserver(*m_camera) }, m_pipeline_state);
}
