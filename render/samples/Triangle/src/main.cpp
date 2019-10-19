#include <array>
#include <vector>

#include <storm/core/Strings.hpp>

#include <storm/log/LogHandler.hpp>

#include <storm/window/Window.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Shader.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

static constexpr auto WINDOW_WIDTH	= 1280u;
static constexpr auto WINDOW_HEIGHT = 768u;

#ifdef STORM_OS_MACOS
static const auto RESOURCE_DIR =
	std::filesystem::current_path() / "../Resources/";
#else
static const auto RESOURCE_DIR = std::filesystem::current_path();
#endif

struct Vertex {
	storm::core::Vector2 position;
	storm::core::Vector3 color;
};

static constexpr const auto VERTICES =
	std::array{Vertex{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			   Vertex{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			   Vertex{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
static constexpr auto VERTEX_BUFFER_SIZE = sizeof(Vertex) * std::size(VERTICES);

void initGraphicsPipeline(
	storm::core::Extent window_extent, storm::render::Device &device,
	storm::render::Surface &surface,
	storm::render::RenderPassOwnedPtr &render_pass,
	storm::render::GraphicsPipelineOwnedPtr &pipeline,
	std::vector<storm::render::FramebufferOwnedPtr> &framebuffers,
	std::vector<storm::render::CommandBufferOwnedPtr> &command_buffers,
	storm::render::Shader &vertex_shader,
	storm::render::Shader &fragment_shader,
	storm::render::HardwareBuffer &vertex_buffer) {
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
		{0, 0, render::Format::Float2, offsetof(Vertex, position)},
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
	pipeline->setRenderPass(*render_pass);
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

	log::LogHandler::setupDefaultLogger();

	auto context = render::Instance{};
	if (!context)
		return EXIT_FAILURE;

	auto window_style =
		window::WindowStyle::Close | window::WindowStyle::Resizable;
	auto video_settings = window::VideoSettings{WINDOW_WIDTH, WINDOW_HEIGHT};

	if (argc > 1 && std::strcmp(argv[1], "--fullscreen") == 0) {
		window_style   = window::WindowStyle::Fullscreen;
		video_settings = window::VideoSettings::getDesktopModes()[0];
	}

	auto window = window::Window{"StormKit Triangle", video_settings, window_style};

	auto surface = context.createSurface(window);
	auto device	 = context.pickPhysicalDevice(*surface).createLogicalDevice();
	surface->initialize(*device);
	const auto image_count = surface->imageCount();

	log::LogHandler::ilog("{}", RESOURCE_DIR.string());
	auto vertex_shader = device->createShader(
		RESOURCE_DIR / "shaders/vertex.glsl", render::Shader::Language::GLSL,
		render::ShaderType::Vertex);
	auto fragment_shader = device->createShader(
		RESOURCE_DIR / "shaders/fragment.glsl", render::Shader::Language::GLSL,
		render::ShaderType::Fragment);

	auto vertex_buffer = device->createVertexBuffer(VERTEX_BUFFER_SIZE);
	vertex_buffer->upload<Vertex>(VERTICES);

	auto render_pass	 = render::RenderPassOwnedPtr{};
	auto pipeline		 = render::GraphicsPipelineOwnedPtr{};
    auto framebuffers	 = std::vector<render::FramebufferOwnedPtr>{};
	auto command_buffers = std::vector<render::CommandBufferOwnedPtr>{};
	command_buffers = device->graphicsQueue().createCommandBuffers(image_count);

	initGraphicsPipeline(surface->extent(), *device, *surface, render_pass,
						 pipeline, framebuffers, command_buffers,
						 *vertex_shader, *fragment_shader, *vertex_buffer);

	auto need_recreate = false;
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

			initGraphicsPipeline(surface->extent(), *device, *surface,
								 render_pass, pipeline, framebuffers,
								 command_buffers, *vertex_shader,
								 *fragment_shader, *vertex_buffer);
		}

		auto frame = surface->acquireNextFrame();

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
