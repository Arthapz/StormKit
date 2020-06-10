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
    const auto &device = m_engine->device();

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

    const auto white = core::RGBColorDef::White<core::UInt8>.toVector4();
    const auto black = core::RGBColorDef::Black<core::UInt8>.toVector4();

    const auto white_data  = core::makeSpan<const std::byte>(white);
    const auto black_data  = core::makeSpan<const std::byte>(black);
    auto &blank_texture_2d = m_texture_pool.create("StormKit:BlankTexture:2D",
                                                   device,
                                                   render::TextureType::T2D,
                                                   render::TextureCreateFlag::None);
    blank_texture_2d.loadFromMemory(white_data, { 1, 1 }, render::PixelFormat::RGBA8_UNorm);
    device.setObjectName(blank_texture_2d, "StormKit:BlankTexture:2D");

    auto &blank_texture_cube = m_texture_pool.create("StormKit:BlankTexture:Cube",
                                                     device,
                                                     render::TextureType::T2D,
                                                     render::TextureCreateFlag::Cube_Compatible);
    blank_texture_cube.loadLayersFromMemory(
        { white_data, white_data, white_data, white_data, white_data, white_data },
        { 1, 1 });

    device.setObjectName(blank_texture_cube, "StormKit:BlankTexture:Cube");

    auto &black_texture_2d = m_texture_pool.create("StormKit:BlackTexture:2D",
                                                   device,
                                                   render::TextureType::T2D,
                                                   render::TextureCreateFlag::None);

    black_texture_2d.loadFromMemory(black_data, { 1, 1 }, render::PixelFormat::RGBA8_UNorm);
    device.setObjectName(black_texture_2d, "StormKit:BlackTexture:2D");

    auto &black_texture_cube = m_texture_pool.create("StormKit:BlackTexture:Cube",
                                                     device,
                                                     render::TextureType::T2D,
                                                     render::TextureCreateFlag::Cube_Compatible);

    black_texture_cube.loadLayersFromMemory(
        { black_data, black_data, black_data, black_data, black_data, black_data },
        { 1, 1 });
    device.setObjectName(black_texture_cube, "StormKit:BlackTexture:Cube");
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
