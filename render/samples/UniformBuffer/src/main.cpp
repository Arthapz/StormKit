/*#include <storm/core/Strings.hpp>
#include <storm/render/command/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/RenderPlugin.hpp>
#include <storm/render/core/Renderer.hpp>
#include <storm/render/renderpass/Pipeline.hpp>
#include <storm/render/renderpass/RenderPass.hpp>
#include <storm/render/shader/Program.hpp>
#include <storm/render/shader/ShaderModule.hpp>
#include <storm/render/surface/Surface.hpp>
#include <storm/render/surface/SwapChain.hpp>
#include <storm/render/sync/Fence.hpp>

static constexpr auto WINDOW_WIDTH  = 800u;
static constexpr auto WINDOW_HEIGHT = 600u;
static constexpr auto WINDOW_TITLE  = gsl::czstring<>{"StormKit Triangle"};

using namespace storm;

struct Data {
	render::RendererOwnedPtr renderer;
	render::DeviceOwnedPtr device;

	// WSI and Swapchain
	render::SurfaceOwnedPtr surface;
	render::SwapChainOwnedPtr swapchain;
	std::vector<render::ImageViewOwnedPtr> image_views;

	render::ShaderModuleOwnedPtr vertex_shader;
	render::ShaderModuleOwnedPtr fragment_shader;
	render::ProgramOwnedPtr program;

	render::PipelineState pipeline_state;
	render::PipelineOwnedPtr pipeline;

	render::RenderPassOwnedPtr render_pass;
	std::vector<render::FramebufferOwnedPtr> framebuffers;
};

void initRendererAndDevice(Data &data, const window::Window &window);
void initWSIAndSwapchain(Data &data, const window::Window &window);
void initShadersAndProgram(Data &data);
void initPipeline(Data &data);
void initRenderPass(Data &data);
void initFramebuffers(Data &data);

int main() {
	// Initialise Window, Base Render object and WSI
	auto window = window::Window{WINDOW_TITLE,
								 window::VideoSettings{WINDOW_WIDTH,
WINDOW_HEIGHT}, window::WindowStyle::Close | window::WindowStyle::Resizable};

	auto data = Data{};
	initRendererAndDevice(data, window);
	initWSIAndSwapchain(data, window);
	initShadersAndProgram(data);
	initPipeline(data);
	initRenderPass(data);
	initFramebuffers(data);

	// Render loop
	while(window.isOpen()) {
		auto event = window::Event{};

		while(window.pollEvent(event)) {
			if(event.type == window::EventType::Closed)
				window.close();
			else if(event.type == window::EventType::Resized) {
				data.device->waitIdle();

				data.swapchain->resizeSwapchain(
				  {event.resizedEvent.width, event.resizedEvent.height});
			}
		}

		auto frame = data.swapchain->nextFrame();

		// setup the command buffer
		frame.command_buffer.reset();
		frame.command_buffer.begin(true);
		frame.command_buffer.beginRenderPass(*data.render_pass,
*data.framebuffers[ frame.index ]);
		frame.command_buffer.bindPipeline(*data.pipeline);
		frame.command_buffer.draw(3);
		frame.command_buffer.endRenderPass();
		frame.command_buffer.end();

		frame.command_buffer.compile();

		// Submit and present
		auto image_available = std::array{std::cref(frame.image_available)};
		auto render_finished = std::array{std::cref(frame.render_finished)};

		frame.fence.reset();
		data.device->presentQueue().submit(
		  frame.command_buffer, image_available, render_finished, frame.fence);

		data.swapchain->present(std::move(frame));

		window.display();
	}

	data.device->waitIdle();

	return 0;
}

void initRendererAndDevice(Data &data, const window::Window &window) {
	auto settings              = render::RendererSettings{};
	settings.app.name          = "StormKit Triangle Example";
	settings.app.version.major = STORM_MAJOR_VERSION;
	settings.app.version.minor = STORM_MINOR_VERSION;
	settings.app.version.patch = STORM_PATCH_VERSION;
	settings.enable_validation = true;

	data.renderer = render::Renderer::createRenderer(
	  render::Renderer::availablePlugins()[ 0u ], std::move(settings));

	const auto &physical_device = data.renderer->pickBestPhysicalDevice();
	data.device =
	  data.renderer->createDevice(physical_device, window,
render::QueueFamilyFlag::Graphics);
}

void initWSIAndSwapchain(Data &data, const window::Window &window) {
	data.surface = data.device->createSurface(window);

	data.swapchain =
data.surface->createSwapchain(core::Extent2U{WINDOW_WIDTH, WINDOW_HEIGHT});
	data.swapchain->onResize = [&data]() {
		data.image_views.clear();
		data.framebuffers.clear();
		data.pipeline.reset();
		data.render_pass.reset();

		for(const auto &image : data.swapchain->images())
			data.image_views.emplace_back(data.device->createImageView(image));
		initPipeline(data);
		initRenderPass(data);
		initFramebuffers(data);
	};
	for(const auto &image : data.swapchain->images())
		data.image_views.emplace_back(data.device->createImageView(image));
}

void initShadersAndProgram(Data &data) {
	data.vertex_shader = data.device->createShaderModule(
	  render::ShaderStage::Vertex, core::filesystem::path{"shaders/vertex.spv"},
false);

	data.fragment_shader = data.device->createShaderModule(
	  render::ShaderStage::Fragment,
core::filesystem::path{"shaders/fragment.spv"}, false);

	data.program = data.device->createProgram(
	  std::array{std::cref(*data.vertex_shader),
std::cref(*data.fragment_shader)});
}

void initPipeline(Data &data) {
	const auto &extent = data.swapchain->extent();

	data.pipeline_state                            = render::PipelineState{};
	data.pipeline_state.viewport_state.viewport    = {0u, 0u, extent.width,
extent.height}; data.pipeline_state.viewport_state.scissor     = {{0u, 0u},
extent}; data.pipeline_state.shader_stage_state.program = data.program.get();

	data.pipeline = data.device->createPipeline(data.pipeline_state);
}

void initRenderPass(Data &data) {
	auto color_attachment         = render::AttachmentDescription{};
	color_attachment.format       = data.swapchain->surfaceFormat().format;
	color_attachment.load_op      = render::AttachmentLoadOperation::Clear;
	color_attachment.store_op     = render::AttachmentStoreOperation::Store;
	color_attachment.final_layout = render::ImageLayout::Present_Src;

	data.render_pass = data.device->createRenderPass({color_attachment});

	data.pipeline->initialize(*data.render_pass);
}

void initFramebuffers(Data &data) {
	const auto &extent = data.swapchain->extent();

	data.framebuffers = std::vector<render::FramebufferOwnedPtr>{};
	for(const auto &image_view : data.image_views) {
		data.framebuffers.emplace_back(
		  data.device->createFramebuffer(*data.render_pass, extent,
{*image_view}));
	}
}*/

#include <array>
#include <storm/core/Strings.hpp>
#include <storm/log/LogHandler.hpp>
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPass.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Shader.hpp>
#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>
#include <storm/window/Window.hpp>
#include <vector>

static constexpr auto WINDOW_WIDTH	= 1280u;
static constexpr auto WINDOW_HEIGHT = 768u;

#ifdef STORM_OS_MACOS
static const auto RESOURCE_DIR =
	std::filesystem::current_path() / "../Resources/";
#else
static const auto RESOURCE_DIR = std::filesystem::current_path();
#endif

struct Vertex {
	storm::core::Vector3 position;
	storm::core::Vector3 color;
};

static constexpr const auto VERTICES = std::array{
	Vertex{{-1.f, -1.f, -1.f}, {1.f, 0.f, 0.f}},
	Vertex{{-1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}},
	Vertex{{-1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}},

	Vertex{{1.f, 1.f, -1.f}, {1.f, 0.f, 0.f}},
	Vertex{{-1.f, -1.f, -1.f}, {1.f, 0.f, 0.f}},
	Vertex{{-1.f, 1.f, -1.f}, {1.f, 0.f, 0.f}},

	Vertex{{1.f, -1.f, 1.f}, {0.f, 1.f, 0.f}},
	Vertex{{-1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},
	Vertex{{1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},

	Vertex{{1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}},
	Vertex{{1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},
	Vertex{{-1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},

	Vertex{{-1.f, -1.f, -1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, 1.f, -1.f}, {0.f, 0.f, 1.f}},

	Vertex{{1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, -1.f, -1.f}, {0.f, 0.f, 1.f}},

	Vertex{{-1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}},
	Vertex{{-1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}},
	Vertex{{1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}},

	Vertex{{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}},
	Vertex{{1.f, -1.f, -1.f}, {1.f, 0.f, 0.f}},
	Vertex{{1.f, 1.f, -1.f}, {1.f, 0.f, 0.f}},

	Vertex{{1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},
	Vertex{{1.f, 1.f, 1.f}, {0.f, 1.f, 0.f}},
	Vertex{{1.f, -1.f, 1.f}, {0.f, 1.f, 0.f}},

	Vertex{{1.f, 1.f, 1.f}, {0.f, 1.f, 0.f}},
	Vertex{{1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}},
	Vertex{{-1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}},

	Vertex{{1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, 1.f, -1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}},

	Vertex{{1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}},
	Vertex{{-1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}},
	Vertex{{1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}},
};
static constexpr auto VERTEX_BUFFER_SIZE = sizeof(Vertex) * std::size(VERTICES);

struct Matrices {
	storm::core::Matrix projection;
	storm::core::Matrix view;
	storm::core::Matrix model;
};
static constexpr auto MATRIX_BUFFER_SIZE = sizeof(Matrices);
static constexpr auto MATRIX_SIZE		 = sizeof(storm::core::Matrix);
static constexpr auto MODEL_OFFSET		 = offsetof(Matrices, model);

void initGraphicsPipeline(
	storm::core::Extent window_extent, storm::render::Device &device,
	storm::render::Surface &surface,
	storm::render::RenderPassOwnedPtr &render_pass,
	storm::render::GraphicsPipelineOwnedPtr &pipeline,
	std::vector<storm::render::FramebufferOwnedPtr> &framebuffers,
	std::vector<storm::render::CommandBufferOwnedPtr> &command_buffers,
	storm::render::Shader &vertex_shader,
	storm::render::Shader &fragment_shader,
	storm::render::HardwareBuffer &vertex_buffer,
	storm::render::DescriptorSetLayout &layout,
	const std::vector<storm::render::DescriptorSetOwnedPtr> &sets) {
	using namespace storm;

	const auto image_count = surface.imageCount();

	render_pass				 = device.createRenderPass();
	const auto attachment_id = render_pass->addAttachment(
		render::RenderPass::Attachment{.format = surface.pixelFormat()});
	render_pass->addSubpass(render::RenderPass::Subpass{
		.bind_point		 = render::PipelineBindPoint::Graphics,
		.attachment_refs = {{attachment_id}}});
	render_pass->build();

	pipeline = device.createGraphicsPipeline();
	pipeline->vertexInputState().binding_descriptions = {{0, sizeof(Vertex)}};
	pipeline->vertexInputState().input_attribute_descriptions = {
		{0, 0, render::Format::Float3, offsetof(Vertex, position)},
		{0, 1, render::Format::Float3, offsetof(Vertex, color)}};
	pipeline->colorBlendState().attachments = {{}};
	pipeline->shaderState().shaders			= {vertex_shader, fragment_shader};
	pipeline->viewportState().viewports		= {
		render::Viewport{{0.f, 0.f},
						 {static_cast<float>(window_extent.w),
						  static_cast<float>(window_extent.h)},
						 {0.f, 1.f}}};
	pipeline->viewportState().scissors = {
		render::Scissor{{0, 0}, window_extent}};
	pipeline->rasterizationState().cull_mode = render::CullMode::Front;
	pipeline->setRenderPass(*render_pass);
	pipeline->layout().descriptor_set_layouts.emplace_back(layout);
	pipeline->build();

	framebuffers.reserve(image_count);

	device.graphicsQueue().waitIdle();

	auto i = 0u;
	for (const auto &texture : surface.textureOutput()) {
		auto &command_buffer = *command_buffers[i];
		command_buffer.reset();

		auto &framebuffer =
			framebuffers.emplace_back(render_pass->createFramebuffer(
				window_extent, {core::makeConstObserver(texture)}));

		command_buffer.begin();
		command_buffer.beginRenderPass(*render_pass, *framebuffer);
		command_buffer.bindGraphicsPipeline(*pipeline);
		command_buffer.bindVertexBuffers({vertex_buffer}, {0});
		command_buffer.bindDescriptorSets(*pipeline, {*sets[i]});

		command_buffer.draw(
			gsl::narrow_cast<std::uint32_t>(std::size(VERTICES)));

		command_buffer.endRenderPass();
		command_buffer.end();

		command_buffer.build();
		++i;
	}
}

#ifdef STORM_OS_WINDOWS
#include <WinBase.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	auto argc  = __argc;
	auto &argv = __argv;
#else
int main(int argc, char **argv) {
#endif
	using namespace storm;
	using Clock = std::chrono::high_resolution_clock;

	log::LogHandler::setupDefaultLogger();

	auto context = render::Instance{};
	if (!context)
		return EXIT_FAILURE;

	auto window_style =
		window::WindowStyle::Close | window::WindowStyle::Resizable;
	auto video_settings =
		window::VideoSettings{.size = {WINDOW_WIDTH, WINDOW_HEIGHT}};

	if (argc > 1 && std::strcmp(argv[1], "--fullscreen") == 0) {
		window_style   = window::WindowStyle::Fullscreen;
		video_settings = window::VideoSettings::getDesktopModes()[0];
	}

	auto window = window::Window{"StormKit UniformBuffer", video_settings, window_style};

	auto surface = context.createSurface(window);
	auto device	 = context.pickPhysicalDevice(*surface).createLogicalDevice();
	surface->initialize(*device);
	const auto image_count = surface->imageCount();

	auto vertex_shader = device->createShader(
        RESOURCE_DIR / "shaders/vertex.glsl", render::Shader::Language::GLSL,
		render::ShaderType::Vertex);
	auto fragment_shader = device->createShader(
        RESOURCE_DIR / "shaders/fragment.glsl", render::Shader::Language::GLSL,
		render::ShaderType::Fragment);

	auto vertex_buffer = device->createVertexBuffer(VERTEX_BUFFER_SIZE);
	vertex_buffer->upload<Vertex>(VERTICES);

	auto descriptor_set_layout = device->createDescriptorSetLayout();
    descriptor_set_layout->addBinding(
        {.binding		   = 0,
         .type			   = render::DescriptorType::UniformBuffer,
         .stages		   = render::ShaderType::Vertex,
         .descriptor_count = 1});
	descriptor_set_layout->build();

	auto descriptor_pool = device->createDescriptorPool(
		*descriptor_set_layout,
		{{render::DescriptorType::UniformBuffer, image_count}}, image_count);

	auto descriptor_sets = descriptor_pool->allocateDescriptorSets(image_count);

	auto matrices = Matrices{
		.projection = core::perspective(core::radians(45.f),
										static_cast<float>(WINDOW_WIDTH) /
											static_cast<float>(WINDOW_HEIGHT),
										0.1f, 10.f),
		.view  = core::lookAt(core::Vector3{4.f, 3.f, 3.f}, {0.f, 0.f, 0.f},
							  {0.f, 1.f, 0.f}),
		.model = core::Matrix{1.f}};

	auto matrix_staging_buffer =
		device->createStagingBuffer(MATRIX_BUFFER_SIZE);
	matrix_staging_buffer->upload<std::byte>(
		{reinterpret_cast<std::byte *>(&matrices), sizeof(Matrices)});

	auto matrix_buffers =
		std::vector<render::HardwareBufferOwnedPtr>{image_count};

	{
		auto transfert_cmb = device->transfertQueue().createCommandBuffer();
		transfert_cmb->begin(true);

		auto i = 0u;
		for (auto &matrix_buffer : matrix_buffers) {
			matrix_buffer = device->createUniformBuffer(
				MATRIX_BUFFER_SIZE, render::MemoryProperty::Device_Local, true);

			const auto matrix_descriptor =
				storm::render::BufferDescriptor{.binding = 0,
												.buffer	 = core::makeConstObserver(matrix_buffer),
                                                .range	 = MATRIX_BUFFER_SIZE};

			descriptor_sets[i++]->update({matrix_descriptor});

			transfert_cmb->copyBuffer(*matrix_staging_buffer, *matrix_buffer,
									  MATRIX_BUFFER_SIZE);
		}

		transfert_cmb->end();
		transfert_cmb->build();

		auto fence = device->createFence();
		device->transfertQueue().submit(
			core::makeConstObserverArray(transfert_cmb), {}, {},
			core::makeObserver(fence));

		auto fences = std::array{std::cref(*fence)};
		device->waitForFences(fences);
	}

	auto render_pass	 = render::RenderPassOwnedPtr{};
	auto pipeline		 = render::GraphicsPipelineOwnedPtr{};
    auto framebuffers	 = std::vector<render::FramebufferOwnedPtr>{};
	auto command_buffers = std::vector<render::CommandBufferOwnedPtr>{};
	command_buffers = device->graphicsQueue().createCommandBuffers(image_count);

	initGraphicsPipeline(surface->extent(), *device, *surface, render_pass,
						 pipeline, framebuffers, command_buffers,
						 *vertex_shader, *fragment_shader, *vertex_buffer,
						 *descriptor_set_layout, descriptor_sets);

	auto transfert_cmds =
		device->transfertQueue().createCommandBuffers(image_count);

	auto i = 0u;
	for (auto &transfert_cmb : transfert_cmds) {
		transfert_cmb->begin();
		transfert_cmb->copyBuffer(*matrix_staging_buffer, *matrix_buffers[i++],
								  MATRIX_SIZE, MODEL_OFFSET, MODEL_OFFSET);
		transfert_cmb->end();
		transfert_cmb->build();
	}

	auto need_recreate = false;
    auto start_time	   = Clock::now();
	while (window.isOpen()) {
		auto event = window::Event{};

		while (window.pollEvent(event)) {
			if (event.type == window::EventType::Closed)
				window.close();
			else if (event.type == window::EventType::Resized) {
				need_recreate = true;
			}
		}

		if (need_recreate || surface->needRecreate()) {
			need_recreate = false;
			device->waitIdle();

			framebuffers.clear();

			pipeline.reset();
			render_pass.reset();

			surface->recreate();

			initGraphicsPipeline(
				surface->extent(), *device, *surface, render_pass, pipeline,
				framebuffers, command_buffers, *vertex_shader, *fragment_shader,
				*vertex_buffer, *descriptor_set_layout, descriptor_sets);
		}

		auto now_timepoint = Clock::now();
		const auto time =
			std::chrono::duration<float, std::chrono::seconds::period>{
				now_timepoint - start_time}
				.count();

		matrices.model =
			core::rotate(core::Matrix{1.f}, time * core::radians(90.f),
						 core::Vector3{0.f, 0.f, 1.f});
		matrix_staging_buffer->upload<std::byte>(
			{reinterpret_cast<std::byte *>(&matrices.model), MATRIX_SIZE},
			MODEL_OFFSET);

		auto frame = surface->acquireNextFrame();

		auto &transfert_cmb = transfert_cmds[frame.image_index];

		auto fence = device->createFence();
		device->transfertQueue().submit(
			core::makeConstObserverArray(transfert_cmb), {}, {},
			core::makeObserver(fence));

		auto fences = std::array{std::cref(*fence)};
		device->waitForFences(fences);

		device->graphicsQueue().submit(
			core::makeConstObserverArray(command_buffers[frame.image_index]),
			core::makeConstObserverArray(frame.image_available),
			core::makeConstObserverArray(frame.render_finished),
			frame.in_flight);

		surface->present(frame);
	}

	device->waitIdle();

	surface->destroy();

	return EXIT_SUCCESS;
}
