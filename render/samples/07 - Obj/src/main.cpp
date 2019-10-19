#include "tiny_obj_loader.h"

#include <storm/core/Strings.hpp>
#include <storm/image/Image.hpp>
#include <storm/render/buffer/HardwareBuffer.hpp>
#include <storm/render/command/CommandBuffer.hpp>
#include <storm/render/command/CommandPool.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/RenderPlugin.hpp>
#include <storm/render/core/Renderer.hpp>
#include <storm/render/descriptor/DescriptorPool.hpp>
#include <storm/render/renderpass/Pipeline.hpp>
#include <storm/render/renderpass/RenderPass.hpp>
#include <storm/render/shader/Program.hpp>
#include <storm/render/shader/ShaderModule.hpp>
#include <storm/render/surface/Surface.hpp>
#include <storm/render/surface/SwapChain.hpp>
#include <storm/render/sync/Fence.hpp>
#include <storm/render/texture/Image.hpp>
#include <storm/render/texture/ImageView.hpp>
#include <storm/render/texture/Sampler.hpp>

using namespace storm;

static constexpr auto WINDOW_WIDTH  = 800u;
static constexpr auto WINDOW_HEIGHT = 600u;
static constexpr auto WINDOW_TITLE  = gsl::czstring<>{"StormKit Obj"};

struct Vertex {
	render::Position3F position;
	render::Vector2F uv;
	bool operator==(const Vertex &other) const {
		return position == other.position && uv == other.uv;
	}
};

namespace std {
	template <> struct hash<Vertex> {
		std::size_t operator()(const Vertex &vertex) const {
			static auto float_hasher = std::hash<float>{};

			auto hash = float_hasher(vertex.position.x);
			core::hash_combine(hash, float_hasher(vertex.position.y));
			core::hash_combine(hash, float_hasher(vertex.position.z));
			core::hash_combine(hash, float_hasher(vertex.uv.x));
			core::hash_combine(hash, float_hasher(vertex.uv.y));

			return hash;
		}
	};
} // namespace std

using VertexArray = std::vector<Vertex>;
using IndexArray  = std::vector<std::uint32_t>;

struct Matrices {
	render::Matrix4F projection = render::Matrix4F{1.f};
	render::Matrix4F view		= render::Matrix4F{1.f};
	render::Matrix4F model		= render::Matrix4F{1.f};
};

struct Data {
	VertexArray vertices;
	IndexArray indices;

	render::RendererPtr renderer;
	render::DevicePtr device;

	// WSI and Swapchain
	render::SurfacePtr surface;
	render::SwapChainPtr swapchain;
	std::vector<render::ImageViewPtr> image_views;

	render::ShaderModulePtr vertex_shader;
	render::ShaderModulePtr fragment_shader;
	render::ProgramPtr program;

	render::CommandPoolPtr command_pool;
	render::HardwareBufferPtr vertex_buffer;
	render::HardwareBufferPtr index_buffer;

	Matrices matrices;
	std::vector<render::HardwareBufferPtr> matrices_buffer;

	image::ImagePtr image_data;
	render::ImagePtr image;
	render::ImageViewPtr image_view;
	render::SamplerPtr sampler;

	render::DescriptorPoolPtr descriptor_pool;
	render::DescriptorSetLayoutPtr descriptor_set_layout;
	std::vector<render::DescriptorSetPtr> descriptor_sets;

	render::ImagePtr depth_image;
	render::ImageViewPtr depth_view;

	render::PipelineState pipeline_state;
	render::PipelinePtr pipeline;

	render::RenderPassPtr render_pass;
	std::vector<render::FramebufferPtr> framebuffers;
};

void initRendererAndDevice(Data &data, const window::Window &window);
void initWSIAndSwapchain(Data &data, const window::Window &window);
void initShadersAndProgram(Data &data);
void initCommandPool(Data &data);
void initVertexBuffer(Data &data);
void initIndexBuffer(Data &data);
void initMatricesBuffer(Data &data);
void initImageImageViewAndSampler(Data &data);
void initDescriptorPoolAndSets(Data &data);
void initDepthImageAndImageView(Data &data);
void initPipeline(Data &data);
void initRenderPass(Data &data);
void initFramebuffers(Data &data);

int main() {
	using Clock = std::chrono::high_resolution_clock;

	// Initialise Window, Base Render object and WSI
	auto window = window::Window{
		WINDOW_TITLE, window::VideoSettings{WINDOW_WIDTH, WINDOW_HEIGHT},
		window::WindowStyle::Close | window::WindowStyle::Resizable};

	auto data = Data{};

	{
		auto attribs		   = tinyobj::attrib_t{};
		auto shapes			   = std::vector<tinyobj::shape_t>{};
		auto materials		   = std::vector<tinyobj::material_t>{};
		auto deduplication_map = std::unordered_map<Vertex, std::uint32_t>{};
		auto warn = std::string{}, err = std::string{};

		if (!tinyobj::LoadObj(&attribs, &shapes, &materials, &warn, &err,
							  "models/chalet.obj")) {
			exit(EXIT_FAILURE);
		}

		for (const auto &shape : shapes) {
			for (const auto &index : shape.mesh.indices) {
				auto vertex = Vertex{};

				vertex.position = {
					attribs.vertices[3 * index.vertex_index],
					attribs.vertices[3 * index.vertex_index + 1],
					attribs.vertices[3 * index.vertex_index + 2]};

				vertex.uv = {
					attribs.texcoords[2 * index.texcoord_index],
					1.f - attribs.texcoords[2 * index.texcoord_index + 1]};

				if (deduplication_map.count(vertex) == 0) {
					deduplication_map.emplace(vertex, std::size(data.vertices));
					data.vertices.emplace_back(std::move(vertex));
				}
				// indices.emplace_back(std::size(indices));
				data.indices.emplace_back(deduplication_map[vertex]);
			}
		}
	}

	initRendererAndDevice(data, window);
	initWSIAndSwapchain(data, window);
	initShadersAndProgram(data);
	initCommandPool(data);
	initVertexBuffer(data);
	initIndexBuffer(data);
	initMatricesBuffer(data);
	initImageImageViewAndSampler(data);
	initDescriptorPoolAndSets(data);
	initDepthImageAndImageView(data);
	initPipeline(data);
	initRenderPass(data);
	initFramebuffers(data);

	// Render loop
	auto start_time = Clock::now();
	while (window.isOpen()) {
		auto now_timepoint = Clock::now();
		auto event		   = window::Event{};

		while (window.pollEvent(event)) {
			if (event.type == window::EventType::Closed)
				window.close();
			else if (event.type == window::EventType::Resized) {
				data.device->waitIdle();

				data.swapchain->resizeSwapchain(
					{event.resizedEvent.width, event.resizedEvent.height});
			}
		}

		const auto time =
			std::chrono::duration<float, std::chrono::seconds::period>{
				now_timepoint - start_time}
				.count();

		data.matrices.model =
			glm::rotate(glm::mat4{1.f}, time * glm::radians(90.0f),
						glm::vec3{0.0f, 0.0f, 1.0f});
		auto frame = data.swapchain->nextFrame();

		auto &buffer	   = *data.matrices_buffer[frame.index];
		auto matrices_data = buffer.map();
		std::memcpy(matrices_data + offsetof(Matrices, model),
					reinterpret_cast<std::byte *>(&data.matrices.model),
					sizeof(render::Matrix4F));
		buffer.unmap();

		// setup the command buffer
		frame.command_buffer.reset();
		frame.command_buffer.begin(true);
		frame.command_buffer.beginRenderPass(*data.render_pass,
											 *data.framebuffers[frame.index]);
		frame.command_buffer.bindPipeline(*data.pipeline);
		frame.command_buffer.bindVertexBuffers({*data.vertex_buffer}, {0u});
		frame.command_buffer.bindIndexBuffer(*data.index_buffer, 0u, true);
		frame.command_buffer.bindDescriptorSets(
			{*data.descriptor_sets[frame.index]}, *data.pipeline);
		frame.command_buffer.drawIndexed(std::size(data.indices));
		frame.command_buffer.endRenderPass();
		frame.command_buffer.end();
		frame.command_buffer.compile();

		// Submit and present
		auto image_available = std::array{std::cref(frame.image_available)};
		auto render_finished = std::array{std::cref(frame.render_finished)};

		frame.fence.reset();
		data.device->presentQueue().submit(frame.command_buffer,
										   image_available, render_finished,
										   frame.fence);

		data.swapchain->present(std::move(frame));

		window.display();
	}

	data.device->waitIdle();

	return 0;
}

void initRendererAndDevice(Data &data, const window::Window &window) {
	auto settings			   = render::RendererSettings{};
	settings.app.name		   = "StormKit Triangle Example";
	settings.app.version.major = STORM_MAJOR_VERSION;
	settings.app.version.minor = STORM_MINOR_VERSION;
	settings.app.version.patch = STORM_PATCH_VERSION;
	settings.enable_validation = true;

	data.renderer = render::Renderer::createRenderer(
		render::Renderer::availablePlugins()[0u], std::move(settings));

	const auto &physical_device = data.renderer->pickBestPhysicalDevice();
	data.device					= data.renderer->createDevice(
		physical_device, window, render::QueueFamilyFlag::Graphics);
}

void initWSIAndSwapchain(Data &data, const window::Window &window) {
	data.surface = data.device->createSurface(window);

	data.swapchain = data.surface->createSwapchain(
		render::Extent2U{WINDOW_WIDTH, WINDOW_HEIGHT});
	data.swapchain->onResize = [&data]() {
		data.framebuffers.clear();
		data.image_views.clear();
		data.pipeline.reset();
		data.render_pass.reset();

		for (const auto &image : data.swapchain->images())
			data.image_views.emplace_back(data.device->createImageView(image));
		initPipeline(data);
		initRenderPass(data);
		initFramebuffers(data);
	};
	for (const auto &image : data.swapchain->images())
		data.image_views.emplace_back(data.device->createImageView(image));
}

void initShadersAndProgram(Data &data) {
	data.vertex_shader = data.device->createShaderModule(
		render::ShaderStage::Vertex,
		core::filesystem::path{"shaders/vertex.spv"}, false);

	data.fragment_shader = data.device->createShaderModule(
		render::ShaderStage::Fragment,
		core::filesystem::path{"shaders/fragment.spv"}, false);

	data.program = data.device->createProgram(std::array{
		std::cref(*data.vertex_shader), std::cref(*data.fragment_shader)});
}

void initCommandPool(Data &data) {
	data.command_pool =
		data.device->createCommandPool(data.device->transfertQueue());
}

void initVertexBuffer(Data &data) {
	const auto size = std::size(data.vertices) * sizeof(Vertex);

	auto stagging_buffer = data.device->createHardwareBuffer(
		size, render::BufferUsageFlag::TransfertSrc,
		render::MemoryPropertyFlag::Host_Visible |
			render::MemoryPropertyFlag::Host_Coherent);

	auto vertex_buffer_data = stagging_buffer->map();
	std::memcpy(vertex_buffer_data,
				reinterpret_cast<const std::byte *>(std::data(data.vertices)),
				size);
	stagging_buffer->unmap();

	data.vertex_buffer = data.device->createVertexBuffer(
		size, render::MemoryPropertyFlag::Device_Local,
		render::BufferUsageFlag::TransfertDst);

	auto fence = data.device->createFence();
	fence->reset();

	auto command_buffer = data.command_pool->allocatePrimaryCommandBuffer();
	command_buffer->begin(true);
	command_buffer->copyBuffer(*stagging_buffer, *data.vertex_buffer, size);
	command_buffer->end();
	command_buffer->compile();

	data.device->transfertQueue().submit(*command_buffer, {}, {}, *fence);
	fence->wait();
}

void initIndexBuffer(Data &data) {
	const auto size = std::size(data.indices) * sizeof(std::uint32_t);

	auto stagging_buffer = data.device->createHardwareBuffer(
		size, render::BufferUsageFlag::TransfertSrc,
		render::MemoryPropertyFlag::Host_Visible |
			render::MemoryPropertyFlag::Host_Coherent);

	auto index_buffer_data = stagging_buffer->map();
	std::memcpy(index_buffer_data,
				reinterpret_cast<const std::byte *>(std::data(data.indices)),
				size);
	stagging_buffer->unmap();

	data.index_buffer = data.device->createIndexBuffer(
		size, render::MemoryPropertyFlag::Device_Local,
		render::BufferUsageFlag::TransfertDst);

	auto fence = data.device->createFence();
	fence->reset();

	auto command_buffer = data.command_pool->allocatePrimaryCommandBuffer();
	command_buffer->begin(true);
	command_buffer->copyBuffer(*stagging_buffer, *data.index_buffer, size);
	command_buffer->end();
	command_buffer->compile();

	data.device->transfertQueue().submit(*command_buffer, {}, {}, *fence);
	fence->wait();
}

void initMatricesBuffer(Data &data) {
	data.matrices.projection = glm::perspective(
		glm::radians(45.f), float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f,
		10.f);
	data.matrices.view =
		glm::lookAt(glm::vec3{2.f, 2.f, 2.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});

	for (auto i = 0u; i < std::size(data.image_views); ++i) {
		auto &buffer =
			data.matrices_buffer.emplace_back(data.device->createUniformBuffer(
				sizeof(Matrices),
				render::MemoryPropertyFlag::Host_Visible |
					render::MemoryPropertyFlag::Host_Coherent));
		auto uniform_buffer_data = buffer->map();
		std::memcpy(uniform_buffer_data,
					reinterpret_cast<const std::byte *>(&data.matrices),
					sizeof(Matrices));
		buffer->unmap();
	}
}

void initImageImageViewAndSampler(Data &data) {
	data.image_data = std::make_unique<image::Image>("models/chalet.jpg");

	auto size = std::size(data.image_data->data());

	auto stagging_buffer = data.device->createHardwareBuffer(
		size, render::BufferUsageFlag::TransfertSrc,
		render::MemoryPropertyFlag::Host_Visible |
			render::MemoryPropertyFlag::Host_Coherent);

	auto index_buffer_data = stagging_buffer->map();
	std::memcpy(index_buffer_data, std::data(data.image_data->data()), size);
	stagging_buffer->unmap();

	data.image = data.device->createImage(
		render::PixelFormat::RGBA8_UNorm,
		render::Extent2U{
			gsl::narrow_cast<std::uint32_t>(data.image_data->size().width),
			gsl::narrow_cast<std::uint32_t>(data.image_data->size().height)},
		render::ImageLayout::Undefined, {render::ImageAspectFlag::Color},
		render::ImageUsageFlag::Transfert_Dst | render::ImageUsageFlag::Sampled,
		render::MemoryPropertyFlag::Device_Local);

	auto fence = data.device->createFence();
	fence->reset();

	auto command_buffer = data.command_pool->allocatePrimaryCommandBuffer();
	command_buffer->begin(true);
	command_buffer->transitionImageLayout(
		*data.image, render::ImageLayout::Transfer_Dst_Optimal);
	command_buffer->copyBufferToImage(*stagging_buffer, *data.image);
	command_buffer->transitionImageLayout(
		*data.image, render::ImageLayout::Shader_Read_Only_Optimal);
	command_buffer->end();
	command_buffer->compile();

	data.device->transfertQueue().submit(*command_buffer, {}, {}, *fence);
	fence->wait();

	data.image_view = data.device->createImageView(*data.image);

	data.sampler = data.device->createSampler();
}

void initDepthImageAndImageView(Data &data) {
	data.depth_image = data.device->createImage(
		render::PixelFormat::Depth32F, data.swapchain->extent(),
		render::ImageLayout::Undefined, {render::ImageAspectFlag::Depth},
		render::ImageUsageFlag::Depth_Stencil_Attachment,
		render::MemoryPropertyFlag::Device_Local);

	auto fence = data.device->createFence();
	fence->reset();

	auto command_buffer = data.command_pool->allocatePrimaryCommandBuffer();
	command_buffer->begin(true);
	command_buffer->transitionImageLayout(
		*data.depth_image,
		render::ImageLayout::Depth_Stencil_Attachment_Optimal);
	command_buffer->end();

	command_buffer->compile();

	data.device->transfertQueue().submit(*command_buffer, {}, {}, *fence);
	fence->wait();

	data.depth_view = data.device->createImageView(*data.depth_image);
}

void initPipeline(Data &data) {
	const auto &extent = data.swapchain->extent();

	data.pipeline_state							   = render::PipelineState{};
	data.pipeline_state.viewport_state.viewport	= {0u, 0u, extent.width,
													  extent.height};
	data.pipeline_state.viewport_state.scissor	 = {{0u, 0u}, extent};
	data.pipeline_state.shader_stage_state.program = data.program.get();
	data.pipeline_state.rasterization_state.cull_mode =
		render::CullModeFlag::None;
	data.pipeline_state.depth_stencil_state.depth_test_enable  = true;
	data.pipeline_state.depth_stencil_state.depth_write_enable = true;

	data.pipeline_state.vertex_input_state.binding_descriptions = {
		{0, sizeof(Vertex), render::VertexInputRate::Vertex}};

	data.pipeline_state.vertex_input_state.attribute_descriptions = {
		{0, 0, render::Format::Float3, offsetof(Vertex, position)},
		{0, 1, render::Format::Float2, offsetof(Vertex, uv)}};

	data.pipeline_state.pipeline_layout.descriptor_set_layouts = {
		*data.descriptor_set_layout};

	data.pipeline = data.device->createPipeline(data.pipeline_state);
}

void initRenderPass(Data &data) {
	auto color_attachment		  = render::AttachmentDescription{};
	color_attachment.format		  = data.swapchain->surfaceFormat().format;
	color_attachment.load_op	  = render::AttachmentLoadOperation::Clear;
	color_attachment.store_op	 = render::AttachmentStoreOperation::Store;
	color_attachment.final_layout = render::ImageLayout::Present_Src;
	auto depth_attachment		  = render::AttachmentDescription{};
	depth_attachment.format		  = data.depth_image->format();
	depth_attachment.load_op	  = render::AttachmentLoadOperation::Clear;
	depth_attachment.store_op	 = render::AttachmentStoreOperation::Dont_Care;
	depth_attachment.final_layout =
		render::ImageLayout::Depth_Stencil_Attachment_Optimal;

	auto subpass = render::SubPass{};
	subpass.color_attachment_references.emplace_back(
		render::AttachmentReference{0});
	subpass.depth_stencil_attachment_reference = render::AttachmentReference{
		1, render::ImageLayout::Depth_Stencil_Attachment_Optimal};

	data.render_pass = data.device->createRenderPass(
		{color_attachment, depth_attachment}, {subpass});

	data.pipeline->initialize(*data.render_pass);
}

void initFramebuffers(Data &data) {
	const auto &extent = data.swapchain->extent();

	data.framebuffers = std::vector<render::FramebufferPtr>{};
	for (const auto &image_view : data.image_views) {
		data.framebuffers.emplace_back(data.device->createFramebuffer(
			*data.render_pass, extent, {*image_view, *data.depth_view}));
	}
}

void initDescriptorPoolAndSets(Data &data) {
	const auto descriptor_count = std::size(data.image_views);

	data.descriptor_set_layout = data.device->createDescriptorSetLayout();
	data.descriptor_set_layout->addBinding(
		{0u, render::ShaderStage::Vertex,
		 render::DescriptorType::Uniform_Buffer, 1});
	data.descriptor_set_layout->addBinding(
		{1u, render::ShaderStage::Fragment,
		 render::DescriptorType::Combined_Image_Sampler, 1});

	data.descriptor_pool = data.device->createDescriptorPool(
		{{render::DescriptorType::Uniform_Buffer, descriptor_count},
		 {render::DescriptorType::Combined_Image_Sampler, descriptor_count}});

	auto layouts = std::vector<render::DescriptorSetLayoutRef>{};
	layouts.resize(descriptor_count, *data.descriptor_set_layout);

	data.descriptor_sets =
		data.descriptor_pool->allocateDescriptorSets(descriptor_count, layouts);

	auto i = 0u;
	for (auto &descriptor_set : data.descriptor_sets) {
		descriptor_set->addDescriptor(
			{0, 0, render::DescriptorType::Uniform_Buffer,
			 render::BufferDescriptorInfo{*data.matrices_buffer[i++],
										  sizeof(Matrices), 0u}});
		descriptor_set->addDescriptor(
			{1, 0, render::DescriptorType::Combined_Image_Sampler,
			 render::ImageDescriptorInfo{data.image->layout(), *data.image_view,
										 *data.sampler}});

		descriptor_set->write();
	}
}
