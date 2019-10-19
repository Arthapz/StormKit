#include <array>
#include <vector>

#include <storm/core/Strings.hpp>

#include <storm/log/ConsoleLogger.hpp>
#include <storm/log/LogHandler.hpp>

#include <storm/window/Window.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>
#include <storm/render/resource/Shader.hpp>

#include <storm/graphics/core/Mesh.hpp>
#include <storm/graphics/core/Renderer.hpp>
#include <storm/graphics/core/Vertex.hpp>
#include <storm/graphics/framegraph/FrameGraph.hpp>
#include <storm/graphics/framegraph/FrameGraphBuilder.hpp>
#include <storm/graphics/framegraph/FramePass.hpp>

#include <storm/entities/EntityManager.hpp>

static constexpr auto WINDOW_WIDTH	= 1280u;
static constexpr auto WINDOW_HEIGHT = 768u;
static constexpr auto WINDOW_TITLE	= gsl::czstring<>{"StormKit RotatingCube"};

#ifdef STORM_OS_MACOS
static const auto RESOURCE_DIR =
	std::filesystem::current_path() / "../Resources/";
#else
static const auto RESOURCE_DIR = std::filesystem::current_path();
#endif

using Vertex = storm::graphics::VertexP3C3;

static const auto VERTICES = storm::graphics::VertexP3C3Array{
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
static const auto VERTEX_BUFFER_SIZE = sizeof(Vertex) * std::size(VERTICES);

struct ColorPassData {
	storm::graphics::FrameGraphTextureResourceObserverPtr backbuffer  = nullptr;
	storm::graphics::FrameGraphTextureResourceObserverPtr depthbuffer = nullptr;
};

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

	log::LogHandler::setupLogger<log::ConsoleLogger>();

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

	auto window = window::Window{WINDOW_TITLE, video_settings, window_style};
	const auto &window_extent = window.size();

	auto surface = context.createSurface(window);
	auto device	 = context.pickPhysicalDevice(*surface).createLogicalDevice();
	surface->initialize(*device);

	auto renderer = graphics::Renderer{*device, *surface};

	auto vertex_shader = device->createShader(
        RESOURCE_DIR / "shaders/vertex.glsl", render::Shader::Language::GLSL,
		render::ShaderType::Vertex);
	auto fragment_shader = device->createShader(
        RESOURCE_DIR / "shaders/fragment.glsl", render::Shader::Language::GLSL,
		render::ShaderType::Fragment);

	auto pipeline_state = render::GraphicsPipelineState{};
	pipeline_state.depth_stencil_state.depth_test_enable   = true;
	pipeline_state.depth_stencil_state.depth_write_enable  = true;
	pipeline_state.vertex_input_state.binding_descriptions = {
		{0, sizeof(Vertex)}};
	pipeline_state.vertex_input_state.input_attribute_descriptions = {
		{0, 0, render::Format::Float3, offsetof(Vertex, position)},
		{0, 1, render::Format::Float3, offsetof(Vertex, color)}};
	pipeline_state.shader_state.shaders = {*vertex_shader, *fragment_shader};
	pipeline_state.viewport_state.viewports = {
		render::Viewport{{0.f, 0.f},
						 {static_cast<float>(window_extent.w),
						  static_cast<float>(window_extent.h)},
						 {0.f, 1.f}}};
	pipeline_state.color_blend_state.attachments = {{}};
	pipeline_state.viewport_state.scissors		 = {
		  render::Scissor{{0, 0}, window_extent.w, window_extent.h}};
	pipeline_state.rasterization_state.cull_mode = render::CullMode::Back;
	for (const auto &layout : renderer.defaultLayout())
		pipeline_state.layout.descriptor_set_layouts.emplace_back(*layout);

	renderer.setProjectionMatrix(
		core::perspective(storm::core::radians(45.f),
						  static_cast<float>(surface->extent().width) /
							  static_cast<float>(surface->extent().height),
						  0.1f, 10.f));
	renderer.setViewMatrix(core::lookAt(storm::core::Vector3{4.f, 4.f, 3.f},
										{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}));

	auto cube_mesh = graphics::Mesh{*device};
	cube_mesh.setVertexData(VERTICES, std::size(VERTICES));
	renderer.addMesh(pipeline_state, cube_mesh);

	auto cube_mesh2 = graphics::Mesh{*device};
	cube_mesh2.setVertexData(VERTICES, std::size(VERTICES));
	renderer.addMesh(pipeline_state, cube_mesh2);

	auto &framegraph_builder = renderer.frameBuilder();
	framegraph_builder.setBackbufferName("backbuffer");

	auto &color_pass = framegraph_builder.addPass<ColorPassData>(
		"ColorPass",
		[&](auto &builder, auto &data) {
			data.backbuffer = builder.create(
				"backbuffer", graphics::FrameGraphTextureDesc{
								  .extent = surface->extent(),
								  .format = surface->pixelFormat()});
			data.depthbuffer = builder.create(
				"depthbuffer",
				graphics::FrameGraphTextureDesc{
					.extent = surface->extent(),
					.format = render::PixelFormat::Depth32F_Stencil8,
					.layout =
						render::ImageLayout::Depth_Stencil_Attachment_Optimal});
		},
		[&](const auto &data, auto &render_pass, auto &framebuffer,
			auto &command_buffer) {
			command_buffer.beginRenderPass(
				render_pass, framebuffer,
				{render::ClearColor{}, render::ClearDepthStencil{}});

			renderer.drawMeshes(command_buffer, render_pass);

			command_buffer.endRenderPass();
		});
	color_pass.setCullImune(true);

	auto start_time = Clock::now();
	while (window.isOpen()) {
		auto event = storm::window::Event{};

		while (window.pollEvent(event)) {
			if (event.type == storm::window::EventType::Closed)
				window.close();
		}

		auto now_timepoint = Clock::now();
		const auto time =
			std::chrono::duration<float, std::chrono::seconds::period>{
				now_timepoint - start_time}
				.count();

		auto matrix =
			core::rotate(core::Matrix{1.f}, time * core::radians(90.f),
						 core::Vector3{0.f, 0.f, 1.f});
		cube_mesh.setModelMatrix(matrix);

		matrix = core::translate(matrix, {2.f, 0.f, 0.f});

		cube_mesh2.setModelMatrix(std::move(matrix));

		renderer.present();
	}

	device->waitIdle();
	surface->destroy();

	return EXIT_SUCCESS;
}
