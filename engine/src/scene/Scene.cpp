/////////// - StormKit::render - ///////////
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>

#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Camera.hpp>
#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/material/PBRMaterial.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Scene::Scene(Engine &engine) : m_engine { &engine } {
    auto &device = m_engine->device();

    m_default_camera =
        std::make_unique<Camera>(engine,
                                 Camera::Type::Perspective,
                                 m_engine->surface().extent().convertTo<core::Extentf>());
    m_camera = core::makeObserver(m_default_camera);

    auto &texture = m_texture_pool.create("BlankTexture",
                                          device,
                                          render::TextureType::T2D,
                                          render::TextureCreateFlag::None);

    auto image = std::vector<float> { 1.f, 1.f, 1.f, 1.f };

    texture.loadFromMemory({ reinterpret_cast<const std::byte *>(std::data(image)),
                             std::size(image) },
                           { 1, 1 },
                           render::PixelFormat::RGBA8_UNorm);

    const auto &extent = m_engine->surface().extent();

    m_pipeline_state.viewport_state.viewports =
        std::vector { render::Viewport { .position = { 0, 0 },
                                         .extent   = extent.convertTo<core::Extentf>(),
                                         .depth    = { 0.f, 1.f } } };
    m_pipeline_state.viewport_state.scissors =
        std::vector { render::Scissor { .offset = { 0, 0 }, .extent = extent } };

    materialPool().create(DEFAULT_PBR_MATERIAL_NAME, std::make_unique<PBRMaterial>(*this));
    auto &material =
        materialPool().create(CUBEMAP_MATERIAL_NAME, std::make_unique<Material>(*this));
    material->addSampler(0, "cubemap");
    material->finalize();
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
