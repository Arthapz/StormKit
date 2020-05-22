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
#include <storm/engine/core/Transform.hpp>

#include <storm/engine/scene/Camera.hpp>
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

static constexpr auto initTransformLayout =
    [](const render::Device &device, render::DescriptorSetLayoutOwnedPtr &layout) -> void {
    layout = device.createDescriptorSetLayoutPtr();
    layout->addBinding(
        { 0, render::DescriptorType::Uniform_Buffer_Dynamic, render::ShaderStage::Vertex, 1 });
    layout->bake();
};

static constexpr auto initCameraLayout = [](const render::Device &device,
                                            render::DescriptorSetLayoutOwnedPtr &layout) -> void {
    layout = device.createDescriptorSetLayoutPtr();
    layout->addBinding({ 0,
                         render::DescriptorType::Uniform_Buffer_Dynamic,
                         render::ShaderStage::Vertex | render::ShaderStage::Fragment,
                         1 });
    layout->bake();
};

#ifdef STORM_OS_WINDOWS
static constexpr auto PIPELINE_CACHE_PATH = std::string_view { "%LOCALAPPDATA%/{}/" };
#else
static constexpr auto PIPELINE_CACHE_PATH = std::string_view { "~/.cache/{}/" };
#endif

////////////////////////////////////////
////////////////////////////////////////
Engine::Engine(const window::Window &window, std::string app_name) {
    m_instance = std::make_unique<render::Instance>(app_name);
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

    m_pipeline_cache = m_device->createPipelineCachePtr(fmt::format(PIPELINE_CACHE_PATH, app_name));

    m_last_tp = Clock::now();

    m_profiler  = std::make_unique<Profiler>(*this);
    m_debug_gui = std::make_unique<DebugGUI>(*this);

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

    Camera::initDescriptorLayout(*m_device, initCameraLayout);
    Transform::initDescriptorLayout(*m_device, initTransformLayout);
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
    m_scene->render(framegraph, backbuffer);

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
