#pragma once

#include <thread>

#include <storm/core/App.hpp>

#include <storm/window/EventHandler.hpp>
#include <storm/window/Window.hpp>

#include <storm/image/Image.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

#include <storm/entities/EntityManager.hpp>

#include "ecs/MovementSystem.hpp"
#include "ecs/RenderSystem.hpp"

class EdoApplication : public storm::core::App {
  public:
	static constexpr auto MAJOR_VERSION = 0;
	static constexpr auto MINOR_VERSION = 1;

	explicit EdoApplication();
	~EdoApplication() override;

	void run(int argc, char **argv) override;

  private:
	void initPipeline();
	void initScene();

	storm::window::Window m_window;
	std::string m_window_name;
	storm::window::EventHandler m_event_handler;

	storm::render::InstanceOwnedPtr m_instance;
	storm::render::SurfaceOwnedPtr m_surface;
	storm::render::DeviceOwnedPtr m_device;

	std::thread m_ecs_thread;
	storm::entities::EntityManager m_entity_manager;
	RenderSystemObserverPtr m_render_system;
	MovementSystemObserverPtr m_movement_system;

	storm::render::ShaderOwnedPtr m_sprite_vertex_shader;
	storm::render::ShaderOwnedPtr m_sprite_fragment_shader;
	storm::render::GraphicsPipelineState m_sprite_pipeline_state;

	storm::render::ShaderOwnedPtr m_map_vertex_shader;
	storm::render::ShaderOwnedPtr m_map_fragment_shader;
	storm::render::DescriptorSetLayoutOwnedPtr m_map_descriptor_set_layout;
	storm::render::DescriptorPoolOwnedPtr m_map_descriptor_pool;
	storm::render::DescriptorSetOwnedPtr m_map_descriptor_set;
	storm::render::GraphicsPipelineState m_map_pipeline_state;

	storm::image::Image m_character_image;
	storm::render::TextureOwnedPtr m_character_texture;

	storm::image::Image m_tileset_image;
	storm::render::TextureOwnedPtr m_tileset_texture;
	storm::render::TextureOwnedPtr m_map_texture;
	storm::render::HardwareBufferOwnedPtr m_map_tileset_info_buffer;

	storm::render::SamplerOwnedPtr m_sampler;

	storm::entities::Entity m_character;
	storm::entities::Entity m_map;
};
