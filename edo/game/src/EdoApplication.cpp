#include "EdoApplication.hpp"
#include "Common.hpp"
#include "Tilemap.hpp"
#include "ecs/Archetypes.hpp"

#include <Configure.hpp>

#include <storm/core/Configure.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Strings.hpp>

#include <storm/log/LogHandler.hpp>

#include <storm/window/InputHandler.hpp>

#include <storm/render/core/Instance.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/resource/Shader.hpp>
#include <storm/render/resource/Texture.hpp>

using LogHandler = storm::log::LogHandler;
using storm::log::operator""_module;

EdoApplication::EdoApplication() : m_event_handler{m_window} {
	LogHandler::ilog("EDO"_module,
					 "Starting EDO {} - built with StormKit {} - {} mode",
					 EDO_VERSION, STORM_VERSION, STORM_BUILD_STRING);

	m_window.create("EDO", {{1280, 768}}, storm::window::WindowStyle::Close);
	LogHandler::ilog("EDO"_module, "Creating a window (size: {})",
					 m_window.size());

	m_window_name =
		fmt::format("EDO {} | {} mode | ", EDO_VERSION, STORM_BUILD_STRING) +
		"CPU: {} us | FPS: {} / sec";
	m_instance = std::make_unique<storm::render::Instance>();

	m_surface = m_instance->createSurface(m_window);

	auto &physical_device = m_instance->pickPhysicalDevice(*m_surface);
	LogHandler::ilog(
		"EDO"_module,
		"Using PhysicalDevice {{\n  vendor_name: {},\n  device_name: {},\n  "
		"type: {},\n  driver_version: {}.{}.{},\n  api_version: {}.{}.{}\n}}",
		physical_device.info().vendor_name, physical_device.info().device_name,
		physical_device.info().type,
		physical_device.info().driver_major_version,
		physical_device.info().driver_minor_version,
		physical_device.info().driver_patch_version,
		physical_device.info().api_major_version,
		physical_device.info().api_minor_version,
		physical_device.info().api_patch_version);

	m_device = physical_device.createLogicalDevice();
	m_surface->initialize(*m_device);

	const auto surface_image_count = m_surface->imageCount();
	LogHandler::ilog("EDO"_module, "Swapchain init with {} images",
					 surface_image_count);

	m_render_system = storm::core::makeObserver(
		&m_entity_manager.addSystem<RenderSystem>(*m_device, *m_surface));
	m_movement_system = storm::core::makeObserver(
		&m_entity_manager.addSystem<MovementSystem>());

	m_event_handler.addCallback(storm::window::EventType::Closed,
								[this](const auto &) { m_window.close(); });
	m_event_handler.addCallback(storm::window::EventType::KeyPressed,
								[this](const auto &event) {
									switch (event.keyEvent.key) {
									case storm::window::Key::ESCAPE:
										m_window.close();
										break;
									default:
										break;
									}
								});

	initPipeline();
	initScene();
}

EdoApplication::~EdoApplication() {
	m_surface->destroy();
}

void EdoApplication::run([[maybe_unused]] int argc,
						 [[maybe_unused]] char **argv) {
	using Clock = std::chrono::high_resolution_clock;

	auto start_frame_time  = Clock::now();
    auto end_frame_time	   = Clock::now();
	auto last_printed_time = Clock::now();
	while (m_window.isOpen()) {
		using MyMicro = std::chrono::duration<std::uint64_t, std::micro>;

		const auto delta = end_frame_time - start_frame_time;
		const auto delta_time =
			std::chrono::duration_cast<MyMicro>(delta).count();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(
				start_frame_time - last_printed_time)
				.count() >= 1000) {
			const auto delta_in_sec = static_cast<float>(delta_time) / 1000.f;

			m_window.setTitle(fmt::format(m_window_name, delta_in_sec,
										  1.f / (delta_in_sec / 1000.f)));

			last_printed_time = start_frame_time;
		}

		start_frame_time = Clock::now();

		m_event_handler.update();

		auto input_state = MovementSystem::InputState{};
		if (storm::window::InputHandler::isKeyPressed(storm::window::Key::Z))
			input_state.up = true;
		else if (storm::window::InputHandler::isKeyPressed(
					 storm::window::Key::S))
			input_state.down = true;
		else if (storm::window::InputHandler::isKeyPressed(
					 storm::window::Key::Q))
			input_state.left = true;
		else if (storm::window::InputHandler::isKeyPressed(
					 storm::window::Key::D))
			input_state.right = true;

		m_movement_system->updateInputState(std::move(input_state));
		m_entity_manager.step(delta_time);

		m_render_system->present();

		end_frame_time = Clock::now();
	}

	m_device->waitIdle();
}

void EdoApplication::initPipeline() {
	auto surface_extent = m_surface->extent();

	m_sprite_vertex_shader =
		m_device->createShader(RESOURCE_DIR / "shaders/sprite_vertex.glsl",
							   storm::render::Shader::Language::GLSL,
							   storm::render::ShaderType::Vertex);
	m_sprite_fragment_shader =
		m_device->createShader(RESOURCE_DIR / "shaders/sprite_fragment.glsl",
							   storm::render::Shader::Language::GLSL,
                               storm::render::ShaderType::Fragment);

	auto pipeline_state = storm::render::GraphicsPipelineState{};
	pipeline_state.depth_stencil_state.depth_test_enable   = true;
	pipeline_state.depth_stencil_state.depth_write_enable  = true;
	pipeline_state.vertex_input_state.binding_descriptions = {
		{0, sizeof(Vertex)}};
	pipeline_state.vertex_input_state.input_attribute_descriptions =
		Vertex::ATTRIBUTE_DESCRIPTIONS;
	pipeline_state.shader_state.shaders		= {*m_sprite_vertex_shader,
										   *m_sprite_fragment_shader};
	pipeline_state.viewport_state.viewports = {
		storm::render::Viewport{{0.f, 0.f},
								{static_cast<float>(surface_extent.w),
								 static_cast<float>(surface_extent.h)},
								{0.f, 1.f}}};
	pipeline_state.color_blend_state.attachments = {
		storm::render::GraphicsPipelineColorBlendAttachmentState{.blend_enable =
																	 true}};
	pipeline_state.viewport_state.scissors = {
		storm::render::Scissor{{0, 0}, surface_extent}};
	pipeline_state.rasterization_state.cull_mode =
        storm::render::CullMode::None;
    for (auto &layout : m_render_system->layout())
        pipeline_state.layout.descriptor_set_layouts.emplace_back(*layout);

	m_sprite_pipeline_state = pipeline_state;

	m_map_vertex_shader =
		m_device->createShader(RESOURCE_DIR / "shaders/map_vertex.glsl",
							   storm::render::Shader::Language::GLSL,
                               storm::render::ShaderType::Vertex);
	m_map_fragment_shader =
		m_device->createShader(RESOURCE_DIR / "shaders/map_fragment.glsl",
							   storm::render::Shader::Language::GLSL,
                               storm::render::ShaderType::Fragment);

	m_map_descriptor_set_layout = m_device->createDescriptorSetLayout();
	m_map_descriptor_set_layout->addBinding(
		{0u, storm::render::DescriptorType::UniformBuffer,
		 storm::render::ShaderType::Fragment, 1});
	m_map_descriptor_set_layout->addBinding(
		{1u, storm::render::DescriptorType::Combined_Image_Sampler,
		 storm::render::ShaderType::Fragment, 1});
	m_map_descriptor_set_layout->build();

	pipeline_state.shader_state.shaders = {*m_map_vertex_shader,
										   *m_map_fragment_shader};
	pipeline_state.layout.descriptor_set_layouts.emplace_back(
        *m_map_descriptor_set_layout);

	m_map_pipeline_state = std::move(pipeline_state);

	m_map_descriptor_pool = m_device->createDescriptorPool(
		*m_map_descriptor_set_layout,
		{{storm::render::DescriptorType::UniformBuffer, 1},
		 {storm::render::DescriptorType::Combined_Image_Sampler, 1}},
		1);

	m_map_descriptor_set =
		std::move(m_map_descriptor_pool->allocateDescriptorSets(1)[0]);
}

void EdoApplication::initScene() {
	m_sampler = m_device->createSampler(storm::render::Sampler::Settings{
		.mag_filter = storm::render::Filter::Nearest,
		.min_filter = storm::render::Filter::Nearest});

	m_character_image.loadFromFile(RESOURCE_DIR / "textures/character.png");
	m_character_image = storm::image::Image::flipY(m_character_image);

	m_character_texture = m_device->createTexture();
	m_character_texture->loadFromImage(m_character_image);

	m_tileset_image.loadFromFile(RESOURCE_DIR / "textures/Overworld.png");
	m_tileset_image = storm::image::Image::flipY(m_tileset_image);

	m_tileset_texture = m_device->createTexture();
	m_tileset_texture->loadFromImage(m_tileset_image);

	m_map_texture = m_device->createTexture();
	m_map_texture->loadFromMemory(
		{reinterpret_cast<const std::byte *>(std::data(TEST_TILEMAP)),
		 sizeof(TileID) * std::size(TEST_TILEMAP)},
		{TEST_TILEMAP_WIDTH, TEST_TILEMAP_HEIGHT},
		storm::render::PixelFormat::RG8_UNorm);

	struct TilesetInfo {
		storm::core::Vector2 tilemapsize;
		storm::core::Vector2 tilesize;
		storm::core::Vector2 mapsize;
	} tileset_info;
	tileset_info.tilemapsize = {40.f, 36.f};
    tileset_info.tilesize	 = {16.f, 16.f};
	tileset_info.mapsize	 = {TEST_TILEMAP_WIDTH, TEST_TILEMAP_HEIGHT};

	m_map_tileset_info_buffer =
		m_device->createUniformBuffer(sizeof(TilesetInfo));
	m_map_tileset_info_buffer->upload<TilesetInfo>({&tileset_info, 1});

	const auto buffer_descriptor =
		storm::render::BufferDescriptor{.binding = 0,
										.buffer	 = storm::core::makeConstObserver(m_map_tileset_info_buffer),
                                        .range	 = sizeof(TilesetInfo),
                                        .offset	 = 0};

	const auto texture_descriptor =
		storm::render::ImageDescriptor{.binding = 1,
                                       .layout	= m_map_texture->layout(),
									   .texture = storm::core::makeConstObserver(m_map_texture),
									   .sampler = storm::core::makeConstObserver(m_sampler)};

	m_map_descriptor_set->update(
		{std::move(buffer_descriptor), std::move(texture_descriptor)});

	makeCamera(m_entity_manager,
			   m_surface->extent().convertTo<storm::core::Extentf>());
	m_map = makeMap(*m_device, m_map_pipeline_state, m_entity_manager,
					{TEST_TILEMAP_WIDTH, TEST_TILEMAP_HEIGHT},
					*m_tileset_texture, *m_sampler, *m_map_descriptor_set);
	m_character =
		makeSprite(*m_device, m_sprite_pipeline_state, m_entity_manager,
				   {32.f, 64.f}, *m_character_texture, *m_sampler);
}
