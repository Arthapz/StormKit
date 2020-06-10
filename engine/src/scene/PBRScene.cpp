/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/resource/Texture.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/drawable/3D/CubeMap.hpp>

#include <storm/engine/scene/FPSCamera.hpp>
#include <storm/engine/scene/PBRScene.hpp>

#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassBuilder.hpp>
#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

#include <storm/engine/material/CubeMapMaterial.hpp>
#include <storm/engine/material/PBRMaterial.hpp>

using namespace storm;
using namespace storm::engine;

static const auto FILTER_CUBE_MAP_VERT_SHADER_DATA = std::vector<core::UInt32> {
#include "FilterCubeMap.vert.spv.hpp"
};
static const auto IRRADIENCE_FRAG_SHADER_DATA = std::vector<core::UInt32> {
#include "GenIrradienceCubeMap.frag.spv.hpp"
};
static const auto PREFILTERED_ENV_FRAG_SHADER_DATA = std::vector<core::UInt32> {
#include "GenPrefilteredEnvCubeMap.frag.spv.hpp"
};

static const auto BRDF_VERT_SHADER_DATA = std::vector<core::UInt32> {
#include "GenBRDF.vert.spv.hpp"
};

static const auto BRDF_FRAG_SHADER_DATA = std::vector<core::UInt32> {
#include "GenBRDF.frag.spv.hpp"
};

static const auto MATRICES = std::array {
    core::rotate(core::rotate(core::Matrixf(1.f),
                              core::radians(90.f),
                              core::Vector3f(0.f, 1.f, 0.f)),
                 core::radians(180.f),
                 core::Vector3f(1.f, 0.f, 0.f)),
    core::rotate(core::rotate(core::Matrixf(1.f),
                              core::radians(-90.f),
                              core::Vector3f(0.f, 1.f, 0.f)),
                 core::radians(180.f),
                 core::Vector3f(1.f, 0.f, 0.f)),
    core::rotate(core::Matrixf(1.f), core::radians(-90.f), core::Vector3f(1.f, 0.f, 0.f)),
    core::rotate(core::Matrixf(1.f), core::radians(90.f), core::Vector3f(1.f, 0.f, 0.f)),
    core::rotate(core::Matrixf(1.f), core::radians(180.f), core::Vector3f(1.f, 0.f, 0.f)),
    core::rotate(core::Matrixf(1.f), core::radians(180.f), core::Vector3f(0.f, 0.f, 1.f)),
};

PBRScene::SceneData::SceneData(const Engine &engine)
    : Bindable { [&engine](Bindable::DescriptorSetVariant &layout) {
          auto l = engine.device().createDescriptorSetLayoutPtr();
          l->addBinding({ 0,
                          render::DescriptorType::Combined_Texture_Sampler,
                          render::ShaderStage::Fragment,
                          1 });
          l->addBinding({ 1,
                          render::DescriptorType::Combined_Texture_Sampler,
                          render::ShaderStage::Fragment,
                          1 });
          l->addBinding({ 2,
                          render::DescriptorType::Combined_Texture_Sampler,
                          render::ShaderStage::Fragment,
                          1 });
          l->addBinding(
              { 3, render::DescriptorType::Uniform_Buffer, render::ShaderStage::Fragment, 1 });
          l->bake();

          layout = std::move(l);
      } },
      m_engine { &engine } {
    const auto &pool = m_engine->descriptorPool();

    m_descriptor_sets = pool.allocateDescriptorSetsPtr(1, descriptorLayout());
    for (auto &set : m_descriptor_sets)
        m_engine->device().setObjectName(*set, "StormKit:SceneDescriptorSet");
}

void PBRScene::SceneData::flush(const render::TextureView &brdf,
                                const render::TextureView &irradience,
                                const render::TextureView &prefiltered_env,
                                const render::HardwareBuffer &buffer) {
    m_brdf_sampler = m_engine->device().createSamplerPtr(
        { .address_mode_u = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_v = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_w = render::SamplerAddressMode::Clamp_To_Edge,
          .max_anisotropy = 1.f,
          .border_color   = render::BorderColor::Float_Opaque_Black,
          .max_lod        = 1.f });
    m_irradience_sampler = m_engine->device().createSamplerPtr(
        { .address_mode_u = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_v = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_w = render::SamplerAddressMode::Clamp_To_Edge,
          .max_anisotropy = 1.f,
          .border_color   = render::BorderColor::Float_Opaque_Black,
          .max_lod        = static_cast<float>(irradience.texture().mipLevels()) });
    m_prefiltered_env_sampler = m_engine->device().createSamplerPtr(
        { .address_mode_u = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_v = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_w = render::SamplerAddressMode::Clamp_To_Edge,
          .max_anisotropy = 1.f,
          .border_color   = render::BorderColor::Float_Opaque_Black,
          .max_lod        = static_cast<float>(irradience.texture().mipLevels()) });

    const auto descriptors = render::DescriptorStaticArray<4> {
        render::Descriptor {
            render::TextureDescriptor {
                .binding      = 0,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(brdf),
                .sampler      = core::makeConstObserver(m_brdf_sampler),
            },
        },
        render::Descriptor {
            render::TextureDescriptor {
                .binding      = 1,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(irradience),
                .sampler      = core::makeConstObserver(m_irradience_sampler),
            },
        },
        render::Descriptor {
            render::TextureDescriptor {
                .binding      = 2,
                .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
                .texture_view = core::makeConstObserver(prefiltered_env),
                .sampler      = core::makeConstObserver(m_prefiltered_env_sampler),
            },
        },
        render::Descriptor {
            render::BufferDescriptor {
                .binding = 3,
                .buffer  = core::makeConstObserver(buffer),
                .range   = sizeof(SceneRawData),
                .offset  = 0u,
            },
        },
    };

    descriptorSet().update(descriptors);
}

PBRScene::PBRScene(Engine &engine) : Scene { engine } {
    m_default_camera =
        std::make_unique<FPSCamera>(engine,
                                    m_engine->surface().extent().convertTo<core::Extentf>());

    const auto &device = m_engine->device();

    m_material_pool.create(DEFAULT_PBR_MATERIAL_NAME, std::make_unique<PBRMaterial>(*this));
    m_material_pool.create(CUBEMAP_MATERIAL_NAME, std::make_unique<CubeMapMaterial>(*this));

    auto &filter_shader = m_shader_pool.create(FILTER_CUBE_MAP_VERT_SHADER_NAME,
                                               FILTER_CUBE_MAP_VERT_SHADER_DATA,
                                               render::ShaderStage::Vertex,
                                               device);
    device.setObjectName(filter_shader, FILTER_CUBE_MAP_VERT_SHADER_NAME);
    auto &irradience_shader = m_shader_pool.create(IRRADIENCE_FRAG_SHADER_NAME,
                                                   IRRADIENCE_FRAG_SHADER_DATA,
                                                   render::ShaderStage::Fragment,
                                                   device);
    device.setObjectName(irradience_shader, IRRADIENCE_FRAG_SHADER_NAME);
    auto &prefiltered_env_shader = m_shader_pool.create(PREFILTERED_ENV_FRAG_SHADER_NAME,
                                                        PREFILTERED_ENV_FRAG_SHADER_DATA,
                                                        render::ShaderStage::Fragment,
                                                        device);
    device.setObjectName(prefiltered_env_shader, PREFILTERED_ENV_FRAG_SHADER_NAME);

    auto &brdf_vert_shader = m_shader_pool.create(BRDF_VERT_SHADER_NAME,
                                                  BRDF_VERT_SHADER_DATA,
                                                  render::ShaderStage::Vertex,
                                                  device);
    device.setObjectName(brdf_vert_shader, BRDF_VERT_SHADER_NAME);
    auto &brdf_frag_shader = m_shader_pool.create(BRDF_FRAG_SHADER_NAME,
                                                  BRDF_FRAG_SHADER_DATA,
                                                  render::ShaderStage::Fragment,
                                                  device);
    device.setObjectName(brdf_frag_shader, BRDF_FRAG_SHADER_NAME);

    m_gen_cube_descriptor_layout = device.createDescriptorSetLayoutPtr();
    m_gen_cube_descriptor_layout->addBinding(
        { 0, render::DescriptorType::Combined_Texture_Sampler, render::ShaderStage::Fragment, 1 });
    m_gen_cube_descriptor_layout->bake();
    device.setObjectName(irradience_shader, "StormKit:GenCubeMapDescriptorLayout");

    m_gen_cube_descriptor_set =
        m_engine->descriptorPool().allocateDescriptorSetPtr(*m_gen_cube_descriptor_layout);
    device.setObjectName(*m_gen_cube_descriptor_set, "StormKit:GenCubeMapDescriptorSet");

    m_gen_cube_sampler = device.createSamplerPtr();
    device.setObjectName(*m_gen_cube_sampler, "StormKit:GenCubeMapDescriptorSampler");

    m_data   = std::make_unique<SceneData>(*m_engine);
    m_buffer = std::make_unique<RingHardwareBuffer>(1,
                                                    device,
                                                    render::HardwareBufferUsage::Uniform,
                                                    sizeof(SceneRawData));
}

////////////////////////////////////////
////////////////////////////////////////
PBRScene::~PBRScene() = default;

////////////////////////////////////////
////////////////////////////////////////
PBRScene::PBRScene(PBRScene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
PBRScene &PBRScene::operator=(PBRScene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void PBRScene::insertGenerateCubeMapPass(FrameGraph &framegraph, CubeMap &cube_map) {
    const auto &texture = cube_map.texture();

    m_gen_cube_view =
        texture.createViewPtr(render::TextureViewType::Cube,
                              { .level_count = texture.mipLevels(), .layer_count = 6 });

    auto hash = core::Hash64 {};
    core::hash_combine(hash, &texture);

    auto irradience_name      = fmt::format("StormKit:{:x}:irradience", hash);
    auto prefiltered_env_name = fmt::format("StormKit:{:x}:prefilteredenv", hash);

    if (m_texture_pool.has("StormKit:BRDF")) {
        m_brdf = core::makeObserver(m_texture_pool.get("StormKit:BRDF"));
    } else {
        generateBRDFLUT(framegraph, "StormKit:BRDF");
    }

    if (m_texture_pool.has(irradience_name) && m_texture_pool.has(prefiltered_env_name)) {
        m_irradience      = core::makeObserver(m_texture_pool.get(irradience_name));
        m_prefiltered_env = core::makeObserver(m_texture_pool.get(prefiltered_env_name));

        m_brdf_view       = m_brdf->createViewPtr(render::TextureViewType::T2D,
                                            { .level_count = m_brdf->mipLevels() });
        m_irradience_view = m_irradience->createViewPtr(render::TextureViewType::Cube,
                                                        { .level_count = m_irradience->mipLevels(),
                                                          .layer_count = 6 });
        m_prefiltered_env_view =
            m_prefiltered_env->createViewPtr(render::TextureViewType::Cube,
                                             { .level_count = m_prefiltered_env->mipLevels(),
                                               .layer_count = 6 });

        const auto mip = static_cast<float>(m_prefiltered_env->mipLevels());

        auto data =
            SceneRawData { .exposure = 4.5f, .gamma = 2.2f, .prefiltered_cube_mip_levels = mip };
        m_buffer->upload<SceneRawData>({ &data, 1 });

        m_data->flush(*m_brdf_view,
                      *m_irradience_view,
                      *m_prefiltered_env_view,
                      m_buffer->buffer());

        return;
    }

    generateIrradience(framegraph, irradience_name);
    generatePrefiltereredEnv(framegraph, prefiltered_env_name);

    m_brdf_view = m_brdf->createViewPtr();
    m_irradience_view =
        m_irradience->createViewPtr(render::TextureViewType::Cube,
                                    { .level_count = m_irradience->mipLevels(), .layer_count = 6 });
    m_prefiltered_env_view =
        m_prefiltered_env->createViewPtr(render::TextureViewType::Cube,
                                         { .level_count = m_prefiltered_env->mipLevels(),
                                           .layer_count = 6 });

    const auto mip = static_cast<float>(render::computeMipLevel(m_prefiltered_env->extent()));

    auto data =
        SceneRawData { .exposure = 4.5f, .gamma = 2.2f, .prefiltered_cube_mip_levels = mip };
    m_buffer->upload<SceneRawData>({ &data, 1 });

    m_data->flush(*m_brdf_view, *m_irradience_view, *m_prefiltered_env_view, m_buffer->buffer());
}

////////////////////////////////////////
////////////////////////////////////////
void PBRScene::generateBRDFLUT(FrameGraph &framegraph, std::string name) {
    const auto &device = m_engine->device();

    const auto brdf_format = render::PixelFormat::RGBA16F;
    const auto brdf_dim    = core::Extentu { 512u, 512u };
    const auto brdf_dimf   = brdf_dim.convertTo<core::Extentf>();

    auto &brdf = m_texture_pool.create(name,
                                       device,
                                       render::TextureType::T2D,
                                       render::TextureCreateFlag::None);

    m_brdf = core::makeObserver(brdf);
    m_brdf->createTextureData(brdf_dim,
                              brdf_format,
                              render::Texture::CreateOperation {
                                  .usage = render::TextureUsage::Transfert_Dst |
                                           render::TextureUsage::Sampled });
    device.setObjectName(brdf, name);

    struct GenerateBRDFPassData {
        engine::FramePassTextureID brdf;
    };

    const auto descriptor =
        FrameGraphTexture::Descriptor { .type   = render::TextureType::T2D,
                                        .format = brdf_format,
                                        .extent = brdf_dim,
                                        .usage  = render::TextureUsage::Color_Attachment |
                                                 render::TextureUsage::Transfert_Src };

    auto &brdf_vert_shader = m_shader_pool.get(BRDF_VERT_SHADER_NAME);
    auto &brdf_frag_shader = m_shader_pool.get(BRDF_FRAG_SHADER_NAME);

    auto &pass = framegraph.addPass<GenerateBRDFPassData>(
        fmt::format("StormKit:Generate{}Pass", name),
        [descriptor, name](auto &builder, auto &pass_data) {
            pass_data.brdf = builder.template create<engine::FrameGraphTexture>(name, descriptor);
            pass_data.brdf = builder.write(pass_data.brdf);
        },
        [this,
         &brdf_vert_shader,
         &brdf_frag_shader,
         brdf_dim,
         brdf_dimf]([[maybe_unused]] const auto &pass_data, const auto &resources, auto &cmb) {
            auto pipeline_state                          = render::GraphicsPipelineState {};
            pipeline_state.rasterization_state.cull_mode = render::CullMode::None;

            pipeline_state.viewport_state.viewports =
                std::vector { render::Viewport { .position = { 0.f, 0.f },
                                                 .extent   = brdf_dimf,
                                                 .depth    = { 0.f, 1.f } } };
            pipeline_state.viewport_state.scissors =
                std::vector { render::Scissor { .offset = { 0, 0 }, .extent = brdf_dim } };

            pipeline_state.color_blend_state.attachments = {
                render::GraphicsPipelineColorBlendAttachmentState {},
            };

            pipeline_state.shader_state.shaders =
                core::makeConstObservers(brdf_vert_shader, brdf_frag_shader);

            const auto &pipeline =
                m_engine->pipelineCache().getPipeline(pipeline_state, resources.renderPass());

            cmb.bindGraphicsPipeline(pipeline);

            cmb.draw(6);
        });

    pass.setPostExecuteCallback([this, &pass, &framegraph, brdf_dim](auto &cmb) {
        const auto &pass_data = pass.data();

        const auto &offscreen = framegraph.get<FrameGraphTexture>(pass_data.brdf);
        cmb.transitionTextureLayout(offscreen.texture(),
                                    render::TextureLayout::Color_Attachment_Optimal,
                                    render::TextureLayout::Transfer_Src_Optimal);
        cmb.transitionTextureLayout(*m_brdf,
                                    render::TextureLayout::Undefined,
                                    render::TextureLayout::Transfer_Dst_Optimal);

        cmb.copyTexture(offscreen.texture(),
                        *m_brdf,
                        render::TextureLayout::Transfer_Src_Optimal,
                        render::TextureLayout::Transfer_Dst_Optimal,
                        {},
                        {},
                        brdf_dim);

        cmb.transitionTextureLayout(*m_brdf,
                                    render::TextureLayout::Transfer_Dst_Optimal,
                                    render::TextureLayout::Shader_Read_Only_Optimal);
    });

    pass.setCullImune(true);
}

////////////////////////////////////////
////////////////////////////////////////
void PBRScene::generateIrradience(FrameGraph &framegraph, std::string name) {
    const auto &device = m_engine->device();

    const auto irradience_format    = render::PixelFormat::RGBA32F;
    const auto irradience_dim       = core::Extentu { 64u, 64u };
    const auto irradience_dimf      = irradience_dim.convertTo<core::Extentf>();
    const auto irradience_mip_level = render::computeMipLevel(irradience_dim);

    m_gen_cube_sampler = m_engine->device().createSamplerPtr(
        { .address_mode_u = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_v = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_w = render::SamplerAddressMode::Clamp_To_Edge,
          .max_anisotropy = 1.f,
          .border_color   = render::BorderColor::Float_Opaque_Black,
          .max_lod        = static_cast<float>(irradience_mip_level) });

    const auto descriptors = render::DescriptorArray { render::TextureDescriptor {
        .binding      = 0u,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_gen_cube_view),
        .sampler      = core::makeConstObserver(m_gen_cube_sampler) } };

    m_gen_cube_descriptor_set->update(descriptors);

    auto &irradience = m_texture_pool.create(name,
                                             device,
                                             render::TextureType::T2D,
                                             render::TextureCreateFlag::Cube_Compatible);

    m_irradience = core::makeObserver(irradience);
    m_irradience->createTextureData(irradience_dim,
                                    irradience_format,
                                    render::Texture::CreateOperation {
                                        .mip_levels = irradience_mip_level,
                                        .layers     = 6,
                                        .usage      = render::TextureUsage::Transfert_Dst |
                                                 render::TextureUsage::Sampled,
                                    });
    device.setObjectName(irradience, name);

    const auto descriptor =
        FrameGraphTexture::Descriptor { .type   = render::TextureType::T2D,
                                        .format = irradience_format,
                                        .extent = irradience_dim,
                                        .usage  = render::TextureUsage::Color_Attachment |
                                                 render::TextureUsage::Transfert_Src };

    struct GenerateIrradiencePassData {
        engine::FramePassTextureID irradience;
    };

    auto &filter_shader     = m_shader_pool.get(FILTER_CUBE_MAP_VERT_SHADER_NAME);
    auto &irradience_shader = m_shader_pool.get(IRRADIENCE_FRAG_SHADER_NAME);

    for (auto mip = 0u; mip < irradience_mip_level; ++mip) {
        auto f = 0u;
        for (auto name : { std::string { "Irradience_Right" },
                           std::string { "Irradience_left" },
                           std::string { "Irradience_Top" },
                           std::string { "Irradience_Bottom" },
                           std::string { "Irradience_Front" },
                           std::string { "Irradience_Back" } }) {
            const auto extent = core::Extentf {
                .width  = static_cast<float>(irradience_dimf.width * std::pow(0.5f, mip)),
                .height = static_cast<float>(irradience_dimf.height * std::pow(0.5f, mip)),
            };

            auto &pass = framegraph.addPass<GenerateIrradiencePassData>(
                fmt::format("StormKit:Generate{}Pass:Mip{}", name, mip),
                [descriptor, name, mip](auto &builder, auto &pass_data) {
                    pass_data.irradience =
                        builder.template create<engine::FrameGraphTexture>(fmt::format("{}:Mip{}",
                                                                                       name,
                                                                                       mip),
                                                                           descriptor);
                    pass_data.irradience = builder.write(pass_data.irradience);
                },
                [this,
                 &filter_shader,
                 &irradience_shader,
                 irradience_dim,
                 extent,
                 f]([[maybe_unused]] const auto &pass_data, const auto &resources, auto &cmb) {
                    auto pipeline_state                          = render::GraphicsPipelineState {};
                    pipeline_state.rasterization_state.cull_mode = render::CullMode::None;

                    pipeline_state.viewport_state.viewports =
                        std::vector { render::Viewport { .position = { 0.f, 0.f },
                                                         .extent   = extent,
                                                         .depth    = { 0.f, 1.f } } };
                    pipeline_state.viewport_state.scissors =
                        std::vector { render::Scissor { .offset = { 0, 0 },
                                                        .extent = irradience_dim } };

                    pipeline_state.color_blend_state.attachments = {
                        render::GraphicsPipelineColorBlendAttachmentState {},
                    };

                    pipeline_state.shader_state.shaders =
                        core::makeConstObservers(filter_shader, irradience_shader);

                    struct PushBlockIrradiance {
                        core::Matrixf mvp;
                        float delta_phi   = (2.f * core::pi<float>) / 180.f;
                        float delta_theta = (0.5f * core::pi<float>) / 64.f;
                    } push_block_irradiance;

                    pipeline_state.layout.descriptor_set_layouts =
                        core::makeConstObservers(m_gen_cube_descriptor_layout);
                    pipeline_state.layout.push_constant_ranges = { render::PushConstantRange {
                        .stages = render::ShaderStage::Vertex | render::ShaderStage::Fragment,
                        .size   = sizeof(PushBlockIrradiance) } };

                    const auto &pipeline =
                        m_engine->pipelineCache().getPipeline(pipeline_state,
                                                              resources.renderPass());

                    auto descriptors =
                        std::vector<render::DescriptorSetCRef> { *m_gen_cube_descriptor_set };
                    cmb.bindGraphicsPipeline(pipeline);
                    cmb.bindDescriptorSets(pipeline, std::move(descriptors), {});

                    push_block_irradiance.mvp =
                        core::perspective(core::pi<float> / 2.f, 1.f, 0.1f, 512.f) * MATRICES[f];

                    const auto push_block_span = core::ByteConstSpan {
                        reinterpret_cast<const core::Byte *>(&push_block_irradiance),
                        sizeof(push_block_irradiance),
                    };
                    auto push_block_data = core::ByteArray {
                        std::cbegin(push_block_span),
                        std::cend(push_block_span),
                    };
                    cmb.pushConstants(pipeline,
                                      render::ShaderStage::Vertex | render::ShaderStage::Fragment,
                                      std::move(push_block_data));
                    cmb.draw(36);
                });

            pass.setPreExecuteCallback([this, f, mip](auto &cmb) {
                const auto subresource_range = render::TextureSubresourceRange {
                    .base_mip_level   = mip,
                    .base_array_layer = f,
                };
                cmb.transitionTextureLayout(*m_irradience,
                                            render::TextureLayout::Undefined,
                                            render::TextureLayout::Transfer_Dst_Optimal,
                                            std::move(subresource_range));
            });

            pass.setPostExecuteCallback([this, &pass, &framegraph, f, mip, extent](auto &cmb) {
                const auto &pass_data = pass.data();

                const auto dst_subresource_range = render::TextureSubresourceRange {
                    .base_mip_level   = mip,
                    .base_array_layer = f,
                };

                const auto &offscreen = framegraph.get<FrameGraphTexture>(pass_data.irradience);
                cmb.transitionTextureLayout(offscreen.texture(),
                                            render::TextureLayout::Color_Attachment_Optimal,
                                            render::TextureLayout::Transfer_Src_Optimal);

                cmb.copyTexture(offscreen.texture(),
                                *m_irradience,
                                render::TextureLayout::Transfer_Src_Optimal,
                                render::TextureLayout::Transfer_Dst_Optimal,
                                {},
                                { .mip_level = mip, .base_array_layer = f },
                                extent.convertTo<core::Extentu>());

                cmb.transitionTextureLayout(*m_irradience,
                                            render::TextureLayout::Transfer_Dst_Optimal,
                                            render::TextureLayout::Shader_Read_Only_Optimal,
                                            std::move(dst_subresource_range));
            });

            pass.setCullImune(true);

            f++;
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
void PBRScene::generatePrefiltereredEnv(FrameGraph &framegraph, std::string name) {
    const auto &device = m_engine->device();

    const auto prefiltered_env_format    = render::PixelFormat::RGBA16F;
    const auto prefiltered_env_dim       = core::Extentu { 512u, 512u };
    const auto prefiltered_env_dimf      = prefiltered_env_dim.convertTo<core::Extentf>();
    const auto prefiltered_env_mip_level = render::computeMipLevel(prefiltered_env_dim);

    m_gen_cube_sampler = m_engine->device().createSamplerPtr(
        { .address_mode_u = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_v = render::SamplerAddressMode::Clamp_To_Edge,
          .address_mode_w = render::SamplerAddressMode::Clamp_To_Edge,
          .max_anisotropy = 1.f,
          .border_color   = render::BorderColor::Float_Opaque_Black,
          .max_lod        = static_cast<float>(prefiltered_env_mip_level) });

    const auto descriptors = render::DescriptorArray { render::TextureDescriptor {
        .binding      = 0u,
        .layout       = render::TextureLayout::Shader_Read_Only_Optimal,
        .texture_view = core::makeConstObserver(m_gen_cube_view),
        .sampler      = core::makeConstObserver(m_gen_cube_sampler) } };

    m_gen_cube_descriptor_set->update(descriptors);

    auto &prefiltered_env = m_texture_pool.create(name,
                                                  device,
                                                  render::TextureType::T2D,
                                                  render::TextureCreateFlag::Cube_Compatible);
    m_prefiltered_env     = core::makeObserver(prefiltered_env);
    m_prefiltered_env->createTextureData(prefiltered_env_dim,
                                         prefiltered_env_format,
                                         render::Texture::CreateOperation {
                                             .mip_levels = prefiltered_env_mip_level,
                                             .layers     = 6,
                                             .usage      = render::TextureUsage::Transfert_Dst |
                                                      render::TextureUsage::Sampled,
                                         });
    device.setObjectName(*m_prefiltered_env, name);

    const auto descriptor =
        FrameGraphTexture::Descriptor { .type   = render::TextureType::T2D,
                                        .format = prefiltered_env_format,
                                        .extent = prefiltered_env_dim,
                                        .usage  = render::TextureUsage::Color_Attachment |
                                                 render::TextureUsage::Transfert_Src };

    struct GeneratePrefilteredEnvPassData {
        engine::FramePassTextureID prefiltered_env;
    };

    auto &filter_shader          = m_shader_pool.get(FILTER_CUBE_MAP_VERT_SHADER_NAME);
    auto &prefiltered_env_shader = m_shader_pool.get(PREFILTERED_ENV_FRAG_SHADER_NAME);

    for (auto mip = 0u; mip < prefiltered_env_mip_level; ++mip) {
        auto f = 0u;
        for (auto name : { std::string { "Prefiltered_Env_Right" },
                           std::string { "Prefiltered_Env_left" },
                           std::string { "Prefiltered_Env_Top" },
                           std::string { "Prefiltered_Env_Bottom" },
                           std::string { "Prefiltered_Env_Front" },
                           std::string { "Prefiltered_Env_Back" } }) {
            const auto extent = core::Extentf {
                .width  = static_cast<float>(prefiltered_env_dimf.width * std::pow(0.5f, mip)),
                .height = static_cast<float>(prefiltered_env_dimf.height * std::pow(0.5f, mip)),
            };

            auto &pass = framegraph.addPass<GeneratePrefilteredEnvPassData>(
                fmt::format("StormKit:Generate{}Pass:{}", name, mip),
                [descriptor, name, mip](auto &builder, auto &pass_data) {
                    pass_data.prefiltered_env =
                        builder.template create<engine::FrameGraphTexture>(fmt::format("{}:Mip{}",
                                                                                       name,
                                                                                       mip),
                                                                           descriptor);
                    pass_data.prefiltered_env = builder.write(pass_data.prefiltered_env);
                },
                [this,
                 &filter_shader,
                 &prefiltered_env_shader,
                 prefiltered_env_dim,
                 f,
                 mip,
                 extent]([[maybe_unused]] const auto &pass_data, const auto &resources, auto &cmb) {
                    auto pipeline_state                          = render::GraphicsPipelineState {};
                    pipeline_state.rasterization_state.cull_mode = render::CullMode::None;

                    pipeline_state.viewport_state.viewports =
                        std::vector { render::Viewport { .position = { 0.f, 0.f },
                                                         .extent   = extent,
                                                         .depth    = { 0.f, 1.f } } };
                    pipeline_state.viewport_state.scissors =
                        std::vector { render::Scissor { .offset = { 0, 0 },
                                                        .extent = prefiltered_env_dim } };

                    pipeline_state.color_blend_state.attachments = {
                        render::GraphicsPipelineColorBlendAttachmentState {},
                    };

                    pipeline_state.shader_state.shaders =
                        core::makeConstObservers(filter_shader, prefiltered_env_shader);

                    auto mip_level = render::computeMipLevel(prefiltered_env_dim);
                    auto mipf      = static_cast<float>(mip);
                    struct PushBlockPrefilteredEnv {
                        core::Matrixf mvp;
                        float roughness;
                        core::UInt32 num_samples = 32u;
                    } push_block_prefiltered_env;
                    push_block_prefiltered_env.roughness = mipf / static_cast<float>(mip_level);

                    pipeline_state.layout.descriptor_set_layouts =
                        core::makeConstObservers(m_gen_cube_descriptor_layout);
                    pipeline_state.layout.push_constant_ranges = { render::PushConstantRange {
                        .stages = render::ShaderStage::Vertex | render::ShaderStage::Fragment,
                        .size   = sizeof(PushBlockPrefilteredEnv) } };

                    const auto &pipeline =
                        m_engine->pipelineCache().getPipeline(pipeline_state,
                                                              resources.renderPass());

                    auto descriptors =
                        std::vector<render::DescriptorSetCRef> { *m_gen_cube_descriptor_set };
                    cmb.bindGraphicsPipeline(pipeline);
                    cmb.bindDescriptorSets(pipeline, std::move(descriptors), {});

                    push_block_prefiltered_env.mvp =
                        core::perspective(core::pi<float> / 2.f, 1.f, 0.1f, 512.f) * MATRICES[f];

                    const auto push_block_span = core::ByteConstSpan {
                        reinterpret_cast<const core::Byte *>(&push_block_prefiltered_env),
                        sizeof(push_block_prefiltered_env),
                    };
                    auto push_block_data = core::ByteArray {
                        std::cbegin(push_block_span),
                        std::cend(push_block_span),
                    };
                    cmb.pushConstants(pipeline,
                                      render::ShaderStage::Vertex | render::ShaderStage::Fragment,
                                      std::move(push_block_data));
                    cmb.draw(36);
                });

            pass.setPreExecuteCallback([this, f, mip](auto &cmb) {
                const auto subresource_range = render::TextureSubresourceRange {
                    .base_mip_level   = mip,
                    .base_array_layer = f,
                };
                cmb.transitionTextureLayout(*m_prefiltered_env,
                                            render::TextureLayout::Undefined,
                                            render::TextureLayout::Transfer_Dst_Optimal,
                                            std::move(subresource_range));
            });

            pass.setPostExecuteCallback([this, &pass, &framegraph, f, mip, extent](auto &cmb) {
                const auto &pass_data = pass.data();

                const auto dst_subresource_range = render::TextureSubresourceRange {
                    .base_mip_level   = mip,
                    .base_array_layer = f,
                };

                const auto &offscreen =
                    framegraph.get<FrameGraphTexture>(pass_data.prefiltered_env);
                cmb.transitionTextureLayout(offscreen.texture(),
                                            render::TextureLayout::Color_Attachment_Optimal,
                                            render::TextureLayout::Transfer_Src_Optimal);

                cmb.copyTexture(offscreen.texture(),
                                *m_prefiltered_env,
                                render::TextureLayout::Transfer_Src_Optimal,
                                render::TextureLayout::Transfer_Dst_Optimal,
                                {},
                                { .mip_level = mip, .base_array_layer = f },
                                extent.convertTo<core::Extentu>());

                cmb.transitionTextureLayout(*m_prefiltered_env,
                                            render::TextureLayout::Transfer_Dst_Optimal,
                                            render::TextureLayout::Shader_Read_Only_Optimal,
                                            std::move(dst_subresource_range));
            });

            pass.setCullImune(true);

            f++;
        }
    }
}
