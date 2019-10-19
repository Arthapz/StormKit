/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/core/DebugGUI.hpp>
#include <storm/engine/core/Profiler.hpp>

#include <storm/engine/mesh/StaticMeshRenderSystem.hpp>

#include <storm/engine/scene/Scene.hpp>

#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassBuilder.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

/////////// - StormKit::log - ///////////
#include <storm/log/LogHandler.hpp>

using namespace storm;
using namespace storm::engine;

using storm::log::operator""_module;
static constexpr auto LOG_MODULE = "engine"_module;

static constexpr auto DESCRIPTOR_COUNT = 1000;

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(const window::Window &window) {
    m_instance = std::make_unique<render::Instance>();
    log::LogHandler::ilog(LOG_MODULE, "Render backend successfully initialized");

    m_surface = m_instance->createSurfacePtr(window);

    const auto &physical_device      = m_instance->pickPhysicalDevice(*m_surface);
    const auto &physical_device_info = physical_device.info();
    log::LogHandler::dlog(LOG_MODULE, "Using physical device {}", physical_device_info.device_name);
    log::LogHandler::dlog(LOG_MODULE, "{}", physical_device_info);

    m_device = physical_device.createLogicalDevicePtr();
    log::LogHandler::dlog(LOG_MODULE, "Device successfully created");

    m_surface->initialize(*m_device);
    const auto buffering_count = m_surface->bufferingCount();
    log::LogHandler::dlog(LOG_MODULE,
                          "Surface successfully initialized with {} image(s)",
                          buffering_count);

    m_queue = core::makeConstObserver(m_device->graphicsQueue());

    for (auto i = 0u; i < buffering_count; ++i)
        m_framegraphs.emplace_back(std::make_unique<FrameGraph>(*this));

    m_command_buffers = m_queue->createCommandBuffers(buffering_count);
    for (auto i = 0u; i < m_surface->bufferingCount(); ++i)
        m_device->setObjectName<render::CommandBuffer>(m_command_buffers[i],
                                                       fmt::format("Engine command buffer {}", i));

    m_descriptor_pool = m_device->createDescriptorPoolPtr(
        {
            { storm::render::DescriptorType::Sampler, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Combined_Texture_Sampler, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Sampled_Image, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Storage_Image, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Uniform_Texel_Buffer, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Storage_Texel_Buffer, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Uniform_Buffer, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Storage_Buffer, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Uniform_Buffer_Dynamic, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Storage_Buffer_Dynamic, DESCRIPTOR_COUNT },
            { storm::render::DescriptorType::Input_Attachment, DESCRIPTOR_COUNT },
        },
        DESCRIPTOR_COUNT);

    m_pipeline_cache = m_device->createPipelineCachePtr();

    auto &texture = m_texture_pool.create("BlankTexture",
                                          *m_device,
                                          render::TextureType::T2D,
                                          render::TextureCreateFlag::None);

    auto image = std::vector<float> { 1.f, 1.f, 1.f, 1.f };

    texture.loadFromMemory({ reinterpret_cast<const std::byte *>(std::data(image)),
                             std::size(image) },
                           { 1, 1 },
                           render::PixelFormat::RGBA8_UNorm);

    m_last_tp = Clock::now();

    m_profiler         = std::make_unique<Profiler>(*this);
    m_debug_gui        = std::make_unique<DebugGUI>(*this);
    m_pipeline_builder = std::make_unique<PipelineBuilder>(*this);

    auto count = m_device->physicalDevice().capabilities().limits.framebuffer_color_sample_counts;
    count &= m_device->physicalDevice().capabilities().limits.framebuffer_depth_sample_counts;

    if ((count & render::SampleCountFlag::C2_BIT) == render::SampleCountFlag::C2_BIT)
        m_max_sample_count = render::SampleCountFlag::C2_BIT;
    if ((count & render::SampleCountFlag::C4_BIT) == render::SampleCountFlag::C4_BIT)
        m_max_sample_count = render::SampleCountFlag::C4_BIT;
    if ((count & render::SampleCountFlag::C8_BIT) == render::SampleCountFlag::C8_BIT)
        m_max_sample_count = render::SampleCountFlag::C8_BIT;
    if ((count & render::SampleCountFlag::C16_BIT) == render::SampleCountFlag::C16_BIT)
        m_max_sample_count = render::SampleCountFlag::C16_BIT;
    if ((count & render::SampleCountFlag::C32_BIT) == render::SampleCountFlag::C32_BIT)
        m_max_sample_count = render::SampleCountFlag::C32_BIT;
    if ((count & render::SampleCountFlag::C64_BIT) == render::SampleCountFlag::C64_BIT)
        m_max_sample_count = render::SampleCountFlag::C64_BIT;
}

////////////////////////////////////////
////////////////////////////////////////
Engine::~Engine() {
    m_device->waitIdle();

    m_framegraphs.clear();
}

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(Engine &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Engine &Engine::operator=(Engine &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Engine::render() {
    namespace Chrono = std::chrono;

    m_profiler->newFrame();

    const auto now = Clock::now();
    m_cpu_time     = Chrono::duration<float, Chrono::seconds::period> { now - m_last_tp }.count();
    m_last_tp      = now;

    m_profiler->beginStage("Frame acquire");
    auto frame = m_surface->acquireNextFrame();
    m_profiler->endStage("Frame acquire");

    m_profiler->beginStage("Scene update");
    m_scene->update(m_cpu_time);
    m_profiler->endStage("Scene update");

    m_profiler->beginStage("FrameGraph setup");
    auto &framegraph = *m_framegraphs[frame.texture_index];
    framegraph.reset();

    auto backbuffer = framegraph.presentTo("Backbuffer",
                                           m_surface->textures()[frame.texture_index],
                                           *frame.texture_available,
                                           *frame.render_finished,
                                           *frame.in_flight);

    m_init_framegraph_callback(backbuffer, framegraph);
    m_profiler->endStage("FrameGraph setup");

    m_profiler->beginStage("FrameGraph compilation");
    framegraph.compile();
    m_profiler->endStage("FrameGraph compilation");

    m_profiler->beginStage("FrameGraph execution");
    framegraph.execute();
    m_profiler->endStage("FrameGraph execution");

    m_profiler->beginStage("Frame present");
    m_surface->present(frame);
    m_profiler->endStage("Frame present");
}

FramePassTextureID Engine::doInitPBRPasses(FramePassTextureID output, FrameGraph &frame_graph) {
    struct ColorPassData {
        engine::FramePassTextureID depth;
        engine::FramePassTextureID msaa;
        engine::FramePassTextureID output;
    };

    const auto sample_count = maxSampleCount();

    const auto depth_descriptor = FrameGraphTexture::Descriptor {
        .type    = render::TextureType::T2D,
        .format  = render::PixelFormat::Depth32F_Stencil8,
        .extent  = m_surface->extent(),
        .samples = (isMSAAEnabled()) ? sample_count : render::SampleCountFlag::C1_BIT,
        .usage   = render::TextureUsage::Depth_Stencil_Attachment
    };
    const auto msaa_descriptor =
        FrameGraphTexture::Descriptor { .type    = render::TextureType::T2D,
                                        .format  = m_surface->pixelFormat(),
                                        .extent  = m_surface->extent(),
                                        .samples = sample_count,
                                        .usage   = render::TextureUsage::Color_Attachment };

    const auto setup = [&output,
                        &depth_descriptor,
                        &msaa_descriptor,
                        this](FramePassBuilder &builder, ColorPassData &pass_data) {
        pass_data.depth = builder.create<FrameGraphTexture>("Depth", std::move(depth_descriptor));

        if (isMSAAEnabled()) {
            pass_data.msaa  = builder.create<FrameGraphTexture>("MSAA", std::move(msaa_descriptor));
            pass_data.msaa  = builder.write(pass_data.msaa);
            pass_data.depth = builder.write(pass_data.depth);

            pass_data.output = builder.resolve(output);
        } else {
            pass_data.output = builder.write(output);
            pass_data.depth  = builder.write(pass_data.depth);
        }
    };

    const auto execute = [this]([[maybe_unused]] const ColorPassData &pass_data,
                                const FramePassResources &resources,
                                render::CommandBuffer &cmb) {
        auto &mesh_render_system = m_scene->entities().getSystem<StaticMeshRenderSystem>();

        mesh_render_system.render(cmb,
                                  "ColorPass",
                                  resources.renderPass(),
                                  { m_scene->cameraDescriptorSet() },
                                  { m_scene->cameraOffset() });
    };

    auto &color_pass =
        frame_graph.addPass<ColorPassData>("ColorPass", std::move(setup), std::move(execute));

    color_pass.setCullImune(true);

    return color_pass.data().output;
}

FramePassTextureID Engine::doInitDebugGUIPasses(FramePassTextureID output,
                                                FrameGraph &frame_graph) {
    struct DebugGuiPassData {
        engine::FramePassTextureID output;
    };

    auto &pass = frame_graph.addPass<DebugGuiPassData>(
        "DebugGuiPass",
        [&output](auto &builder, auto &pass_data) {
            pass_data.output = builder.read(output);
            pass_data.output = builder.write(output);
        },
        [this]([[maybe_unused]] const auto &pass_data, const auto &resources, auto &cmb) {
            static auto is_init = false;

            if (!is_init) {
                m_debug_gui->init(resources.renderPass());
                is_init = true;
            }

            m_debug_gui->render(cmb, resources.renderPass());
        });

    pass.useSubCommandBuffer(true);

    return pass.data().output;
}
