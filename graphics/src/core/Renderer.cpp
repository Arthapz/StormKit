#include <storm/graphics/core/DefaultShaders.hpp>
#include <storm/graphics/core/MeshCache.hpp>
#include <storm/graphics/core/Renderer.hpp>
#include <storm/graphics/core/Vertex.hpp>
#include <storm/graphics/framegraph/FrameGraph.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>
#include <storm/render/pipeline/RenderPass.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>
#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::graphics;

namespace std {
	std::size_t hash<MaterialData>::
		operator()(const MaterialData &material) const noexcept {
		static auto material_hasher = std::hash<Material>{};

		auto hash = material_hasher(material.material);

		return hash;
	}
} // namespace std

////////////////////////////////////////
////////////////////////////////////////
Renderer::Renderer(const render::Device &device, render::Surface &surface)
	: m_device{&device}, m_surface{&surface},
	  m_graphics_queue{&m_device->graphicsQueue()},
	  m_transfert_queue{&m_device->transfertQueue()}, m_builder{*m_device},
	  m_framegraphs{m_surface->imageCount()} {
	const auto image_count = m_surface->imageCount();

	m_pipeline_cache = m_device->createPipelineCache();

	m_camera_descriptor_set_layout = m_device->createDescriptorSetLayout();
	m_camera_descriptor_set_layout->addBinding(
		render::DescriptorSetLayoutBinding{
			0u, render::DescriptorType::UniformBuffer,
			render::ShaderType::Vertex | render::ShaderType::Fragment, 1u});
	m_camera_descriptor_set_layout->build();

	m_per_material_descriptor_set_layout =
		m_device->createDescriptorSetLayout();
	m_per_material_descriptor_set_layout->addBinding(
		render::DescriptorSetLayoutBinding{
			0u, render::DescriptorType::Combined_Image_Sampler,
			render::ShaderType::Fragment, 1u});
	m_per_material_descriptor_set_layout->build();

	m_per_mesh_descriptor_set_layout = m_device->createDescriptorSetLayout();
	m_per_mesh_descriptor_set_layout->addBinding(
		render::DescriptorSetLayoutBinding{
			0u, render::DescriptorType::UniformBuffer,
			render::ShaderType::Vertex | render::ShaderType::Fragment, 1u});
	m_per_mesh_descriptor_set_layout->build();

	m_camera_descriptor_pool = m_device->createDescriptorPool(
		*m_camera_descriptor_set_layout,
		{{render::DescriptorType::UniformBuffer, MAX_CAMERA_SETS}},
		MAX_CAMERA_SETS);

	m_material_descriptor_pool = m_device->createDescriptorPool(
		*m_per_material_descriptor_set_layout,
		{{render::DescriptorType::Combined_Image_Sampler, MAX_MATERIAL_SETS}},
		MAX_MATERIAL_SETS);

	m_per_camera_set =
		std::move(m_camera_descriptor_pool->allocateDescriptorSets(1)[0]);

	m_matrices_transfert_cmds =
		m_graphics_queue->createCommandBuffers(image_count);

	m_per_camera_data_buffer = m_device->createUniformBuffer(
		sizeof(CameraMatrices), render::MemoryProperty::Device_Local, true);

	const auto matrix_descriptor = storm::render::BufferDescriptor{
		.binding = 0,
		.buffer  = core::makeConstObserver(m_per_camera_data_buffer),
		.range   = sizeof(CameraMatrices)};

	m_per_camera_set->update({matrix_descriptor});

	for (auto i = 0u; i < image_count; ++i) {
		auto matrices_staging_buffer =
			m_device->createStagingBuffer(sizeof(CameraMatrices));
		matrices_staging_buffer->upload<CameraMatrices>(
			{&m_camera_matrices, 1});

		auto &staging_buffer = m_per_camera_datas_staging.emplace_back(
			std::move(matrices_staging_buffer));
		m_matrices_transfert_finished.emplace_back(m_device->createSemaphore());

		auto &cmb = m_matrices_transfert_cmds[i];

		cmb->begin();
		cmb->copyBuffer(*staging_buffer, *m_per_camera_data_buffer,
						sizeof(CameraMatrices));
		cmb->end();
		cmb->build();
	}

	// LES SHADERS
	m_quad_vert_shader = m_device->createShader(QUAD_VERTEX_SHADER,
												render::Shader::Language::SPIRV,
												render::ShaderType::Vertex);
	m_quad_frag_shader = m_device->createShader(QUAD_FRAGMENT_SHADER,
												render::Shader::Language::SPIRV,
												render::ShaderType::Fragment);

	m_quad_descriptor_set_layout = m_device->createDescriptorSetLayout();
	m_quad_descriptor_set_layout->addBinding(
		{.binding		   = 0,
		 .type			   = render::DescriptorType::Combined_Image_Sampler,
		 .stages		   = render::ShaderType::Fragment,
		 .descriptor_count = 1});
	m_quad_descriptor_set_layout->build();

	m_quad_descriptor_pool = m_device->createDescriptorPool(
		*m_quad_descriptor_set_layout,
		{{render::DescriptorType::Combined_Image_Sampler, image_count}},
		image_count);

	m_quad_descriptor_sets =
		m_quad_descriptor_pool->allocateDescriptorSets(image_count);

	constexpr auto quad_vertices =
		std::array{VertexP2UV2{.position = {-1.f, -1.f}, .uv = {0.f, 0.f}},
				   VertexP2UV2{.position = {1.f, -1.f}, .uv = {1.f, 0.f}},
				   VertexP2UV2{.position = {-1.f, 1.f}, .uv = {0.f, 1.f}},
				   VertexP2UV2{.position = {-1.f, 1.f}, .uv = {0.f, 1.f}},
				   VertexP2UV2{.position = {1.f, -1.f}, .uv = {1.f, 0.f}},
				   VertexP2UV2{.position = {1.f, 1.f}, .uv = {1.f, 1.f}}};

	m_quad_vertices = m_device->createVertexBuffer(
		sizeof(VertexP2UV2) * std::size(quad_vertices),
		render::MemoryProperty::Device_Local, true);

	auto staging_buffer = m_device->createStagingBuffer(
		sizeof(VertexP2UV2) * std::size(quad_vertices));
	staging_buffer->upload<VertexP2UV2>(quad_vertices);

	auto copy_vertex_cmb = m_graphics_queue->createCommandBuffer();
	copy_vertex_cmb->begin(true);
	copy_vertex_cmb->copyBuffer(*staging_buffer, *m_quad_vertices,
								sizeof(VertexP2UV2) * std::size(quad_vertices));
	copy_vertex_cmb->end();
	copy_vertex_cmb->build();

	auto fence = m_device->createFence();

	m_graphics_queue->submit(core::makeConstObserverArray(copy_vertex_cmb), {},
							 {}, core::makeObserver(fence));

	m_quad_render_pass		 = m_device->createRenderPass();
	const auto attachment_id = m_quad_render_pass->addAttachment(
		render::RenderPass::Attachment{.format = surface.pixelFormat()});
	m_quad_render_pass->addSubpass(render::RenderPass::Subpass{
		.bind_point		 = render::PipelineBindPoint::Graphics,
		.attachment_refs = {{attachment_id}}});
	m_quad_render_pass->build();

	m_quad_pipeline = m_device->createGraphicsPipeline();
	m_quad_pipeline->vertexInputState().binding_descriptions = {
		{0, sizeof(VertexP2UV2)}};
	m_quad_pipeline->vertexInputState().input_attribute_descriptions =
		VertexP2UV2::ATTRIBUTE_DESCRIPTIONS;
	m_quad_pipeline->colorBlendState().attachments = {{}};
	m_quad_pipeline->shaderState().shaders		   = {*m_quad_vert_shader,
											  *m_quad_frag_shader};
	m_quad_pipeline->viewportState().viewports	 = {
		render::Viewport{{0.f, 0.f},
						 {static_cast<float>(m_surface->extent().w),
						  static_cast<float>(m_surface->extent().h)},
						 {0.f, 1.f}}};
	m_quad_pipeline->viewportState().scissors = {
		render::Scissor{{0, 0}, m_surface->extent()}};
	m_quad_pipeline->rasterizationState().cull_mode = render::CullMode::None;
	m_quad_pipeline->setRenderPass(*m_quad_render_pass);
	m_quad_pipeline->layout().descriptor_set_layouts.emplace_back(
		*m_quad_descriptor_set_layout);
	m_quad_pipeline->build();

	m_quad_framebuffers.reserve(image_count);
	for (const auto &texture : surface.textureOutput())
		m_quad_framebuffers.emplace_back(m_quad_render_pass->createFramebuffer(
			m_surface->extent(), {core::makeConstObserver(texture)}));

	m_quad_sampler = m_device->createSampler(storm::render::Sampler::Settings{
		.mag_filter = storm::render::Filter::Nearest,
		.min_filter = storm::render::Filter::Nearest});

	m_blit_cmds = m_graphics_queue->createCommandBuffers(image_count);

	m_device->waitForFence(*fence);
}

////////////////////////////////////////
////////////////////////////////////////
Renderer::~Renderer() = default;

/*////////////////////////////////////////
////////////////////////////////////////
Renderer::Renderer(Renderer &&moved)
	: m_device{moved.m_device}, m_surface{moved.m_surface},
	  m_graphics_queue{moved.m_graphics_queue},
	  m_transfert_queue{moved.m_transfert_queue},
	  m_pipeline_cache{std::move(
													  moved.m_pipeline_cache)},
	  m_camera_matrices{std::move(moved.m_camera_matrices)},
	  m_camera_matrices_need_update{moved.m_camera_matrices_need_update},
	  m_per_camera_data_buffer{std::move(moved.m_per_camera_data_buffer)},
	  m_per_camera_datas_staging{std::move(moved.m_per_camera_datas_staging)},
	  m_matrices_transfert_cmds{std::move(moved.m_matrices_transfert_cmds)},
	  m_matrices_transfert_finished{std::move(moved.m_matrices_transfert_finished)},
	  m_camera_descriptor_set_layout{std::move(moved.m_camera_descriptor_set_layout)}
	  m_builder{
		  std::move(moved.m_builder)},
	  m_framegraphs({std::move(moved.m_framegraphs)}), m_ {
}

////////////////////////////////////////
////////////////////////////////////////
Renderer &Renderer::operator=(Renderer &&moved) {
	if (&moved == this)
		return *this;
}*/

////////////////////////////////////////
////////////////////////////////////////
void Renderer::present() {
	// auto lock = std::unique_lock{m_mutex};

	m_frame = m_surface->acquireNextFrame();

	m_framegraphs.push(m_builder.compile());
	m_current_framegraph = core::makeObserver(&m_framegraphs.get());
	m_framegraphs.next();

	if (m_camera_matrices_need_update) {
		updateCamera(m_frame.image_index);

		m_current_framegraph->addWaitSemaphore(
			*m_matrices_transfert_finished[m_frame.image_index]);

		m_camera_matrices_need_update = false;
	}

	auto &frame_render_finished = m_current_framegraph->execute();

	auto &backbuffer	  = m_current_framegraph->backbuffer();
	auto &swapchain_image = *m_surface->textureOutput()[m_frame.image_index];

	auto &command_buffer = m_blit_cmds[m_frame.image_index];
	command_buffer->reset();
	command_buffer->begin(true);
	command_buffer->transitionImageLayout(
		swapchain_image, render::ImageLayout::Transfer_Dst_Optimal);
	command_buffer->transitionImageLayout(
		backbuffer, render::ImageLayout::Transfer_Src_Optimal);

	command_buffer->copyImage(backbuffer,
							  render::ImageLayout::Transfer_Src_Optimal,
							  render::ImageAspectMask::Color, swapchain_image,
							  render::ImageLayout::Transfer_Dst_Optimal,
							  render::ImageAspectMask::Color);
	command_buffer->transitionImageLayout(swapchain_image,
										  render::ImageLayout::Present_Src);
	command_buffer->end();
	command_buffer->build();

	/*auto &command_buffer = m_blit_cmds[m_frame.image_index];
	auto &framebuffer	 = m_quad_framebuffers[m_frame.image_index];
	auto &descriptor_set = m_quad_descriptor_sets[m_frame.image_index];
	descriptor_set->update({render::ImageDescriptor{
		0, backbuffer.layout(), backbuffer, *m_quad_sampler}});

	command_buffer->reset();
	command_buffer->begin(true);
	command_buffer->beginRenderPass(*m_quad_render_pass, *framebuffer);
	command_buffer->bindGraphicsPipeline(*m_quad_pipeline);
	command_buffer->bindDescriptorSets(*m_quad_pipeline, {*descriptor_set});
	command_buffer->bindVertexBuffers({*m_quad_vertices}, {0u});
	command_buffer->draw(6);
	command_buffer->endRenderPass();
	command_buffer->end();
	command_buffer->build();*/

	m_graphics_queue->submit(
		core::makeConstObserverArray(command_buffer),
		core::makeConstObserverArray(m_frame.image_available,
									 &frame_render_finished),
		core::makeConstObserverArray(m_frame.render_finished),
		m_frame.in_flight);

	m_surface->present(m_frame);
}

////////////////////////////////////////
////////////////////////////////////////
void Renderer::updateCamera(std::uint32_t frame_index) {
	// auto lock		= std::unique_lock{m_mutex};
	auto &staging   = *m_per_camera_datas_staging[frame_index];
	auto &semaphore = m_matrices_transfert_finished[frame_index];
	auto &cmb		= m_matrices_transfert_cmds[frame_index];

	staging.upload<CameraMatrices>({&m_camera_matrices, 1});

	m_graphics_queue->submit(core::makeConstObserverArray(cmb), {},
							 core::makeConstObserverArray(semaphore));
}

////////////////////////////////////////
////////////////////////////////////////
void Renderer::drawMeshes(render::CommandBuffer &cmb,
						  const render::RenderPass &render_pass) {
	// auto lock			   = std::unique_lock{m_mutex};
	const auto &camera_set = *m_per_camera_set;
	for (const auto &[pipeline_state, materials] : m_materials) {
		for (const auto &material : materials)
			drawMeshesByMaterial(camera_set, pipeline_state, material, cmb,
								 render_pass);
	}
}

////////////////////////////////////////
////////////////////////////////////////
void Renderer::drawMeshesByMaterial(
	const render::DescriptorSet &camera_set,
	const render::GraphicsPipelineState &pipeline_state,
	const MaterialData &material_data, render::CommandBuffer &cmb,
	const render::RenderPass &render_pass) {
	const auto &material_set = *material_data.set;
	const auto &mesh_cache   = m_meshes.at(material_data.material);

	const auto &pipeline =
		m_pipeline_cache->getPipeline(pipeline_state, render_pass);

	cmb.bindGraphicsPipeline(pipeline);

	auto semaphores = mesh_cache->drawMeshes(cmb, pipeline, camera_set,
											 material_set, m_frame.image_index);

	for (const auto &semaphore : semaphores)
		m_current_framegraph->addWaitSemaphore(*semaphore);
}

////////////////////////////////////////
////////////////////////////////////////
void Renderer::addMesh(const render::GraphicsPipelineState &state,
					   const Mesh &mesh) {
	// auto lock						   = std::unique_lock{m_mutex};
	static const auto default_material = Material{};

	addMesh(state, default_material, mesh);
}

void Renderer::addMesh(const render::GraphicsPipelineState &state,
					   const Material &material, const Mesh &mesh) {
	// auto lock		= std::unique_lock{m_mutex};
	auto &materials = m_materials[state];

	auto it = std::find_if(std::cbegin(materials), std::cend(materials),
						   [&material](const auto &material_data) {
							   auto hasher = std::hash<Material>{};
							   return hasher(material_data.material) ==
									  hasher(material);
						   });
	if (it == std::cend(materials)) {
		auto material_data = MaterialData{};
		material_data.set =
			std::move(m_material_descriptor_pool->allocateDescriptorSets(1)[0]);
		material_data.material = material;

		auto descriptors = std::vector<storm::render::Descriptor>{};
		if (material_data.material.diffuse_map != nullptr &&
			material_data.material.diffuse_map_sampler != nullptr) {
			const auto diffuse_descriptor = storm::render::ImageDescriptor{
				.binding = 0,
				.layout  = material_data.material.diffuse_map->layout(),
				.texture =
					core::makeConstObserver(material_data.material.diffuse_map),
				.sampler = core::makeConstObserver(
					material_data.material.diffuse_map_sampler)};

			descriptors.emplace_back(std::move(diffuse_descriptor));
		}

		if (!std::empty(descriptors))
			material_data.set->update(std::move(descriptors));

		it = m_materials[state].emplace(std::move(material_data)).first;
	}

	if (m_meshes[it->material] == nullptr)
		m_meshes[it->material] =
			std::make_unique<MeshCache>(m_surface->imageCount(), *m_device,
										*m_per_mesh_descriptor_set_layout);

	m_meshes[it->material]->addMesh(mesh);
}
