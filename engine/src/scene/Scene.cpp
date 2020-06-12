/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Queue.hpp>
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

    auto &blank_texture_2d = m_texture_pool.create("StormKit:BlankTexture:2D",
                                                   device,
                                                   render::TextureType::T2D,
                                                   render::TextureCreateFlag::None);
    blank_texture_2d.createTextureData({ .width = 1u, .height = 1u },
                                       render::PixelFormat::RGBA8_UNorm);
    device.setObjectName(blank_texture_2d, "StormKit:BlankTexture:2D");

    auto &blank_texture_cube = m_texture_pool.create("StormKit:BlankTexture:Cube",
                                                     device,
                                                     render::TextureType::T2D,
                                                     render::TextureCreateFlag::Cube_Compatible);
    blank_texture_cube.createTextureData({ .width = 1u, .height = 1u },
                                         render::PixelFormat::RGBA8_UNorm,
                                         render::Texture::CreateOperation { .layers = 6u });
    device.setObjectName(blank_texture_cube, "StormKit:BlankTexture:Cube");

    auto &black_texture_2d = m_texture_pool.create("StormKit:BlackTexture:2D",
                                                   device,
                                                   render::TextureType::T2D,
                                                   render::TextureCreateFlag::None);
    black_texture_2d.createTextureData({ .width = 1u, .height = 1u },
                                       render::PixelFormat::RGBA8_UNorm);
    device.setObjectName(black_texture_2d, "StormKit:BlackTexture:2D");

    auto &black_texture_cube = m_texture_pool.create("StormKit:BlackTexture:Cube",
                                                     device,
                                                     render::TextureType::T2D,
                                                     render::TextureCreateFlag::Cube_Compatible);
    black_texture_cube.createTextureData({ .width = 1u, .height = 1u },
                                         render::PixelFormat::RGBA8_UNorm,
                                         render::Texture::CreateOperation { .layers = 6u });
    device.setObjectName(black_texture_cube, "StormKit:BlackTexture:Cube");

    constexpr auto white  = core::RGBColorDef::White<core::UInt8>.toVector4();
    constexpr auto black  = core::RGBColorDef::Black<core::UInt8>.toVector4();
    constexpr auto whites = std::array { white, white, white, white, white, white };
    constexpr auto blacks = std::array { black, black, black, black, black, black };

    constexpr auto white_size = sizeof(white);

    auto offset         = 0u;
    auto staging_buffer = device.createStagingBuffer(white_size * 2u + white_size * 12);
    staging_buffer.upload<core::Byte>(core::toConstSpan<core::Byte>(&white, sizeof(white)), offset);
    offset += white_size;
    staging_buffer.upload<core::Byte>(core::toConstSpan<core::Byte>(whites), offset);
    offset += white_size * 6u;
    staging_buffer.upload<core::Byte>(core::toConstSpan<core::Byte>(&black, sizeof(white)), offset);
    offset += white_size;
    staging_buffer.upload<core::Byte>(core::toConstSpan<core::Byte>(blacks), offset);

    auto fence = device.createFence();
    auto command_buffer =
        device.graphicsQueue().createCommandBuffer(render::CommandBufferLevel::Primary);

    command_buffer.begin(true);

    offset = 0u;
    blank_texture_2d.fillMemory(white_size,
                                { .width = 1u, .height = 1u },
                                0u,
                                1u,
                                command_buffer,
                                staging_buffer,
                                offset);
    offset += white_size;

    blank_texture_cube.fillMemory(white_size,
                                  { .width = 1u, .height = 1u },
                                  0u,
                                  6u,
                                  command_buffer,
                                  staging_buffer,
                                  offset);
    offset += white_size * 6u;

    black_texture_2d.fillMemory(white_size,
                                { .width = 1u, .height = 1u },
                                0u,
                                1u,
                                command_buffer,
                                staging_buffer,
                                offset);
    offset += white_size;

    black_texture_cube.fillMemory(white_size,
                                  { .width = 1u, .height = 1u },
                                  0u,
                                  6u,
                                  command_buffer,
                                  staging_buffer,
                                  offset);

    command_buffer.end();
    command_buffer.build();
    command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
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
