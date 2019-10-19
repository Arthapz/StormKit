#include <storm/graphics/framegraph/FramePass.hpp>
#include <storm/graphics/system/RenderSystem.hpp>
#include <storm/graphics/system/StaticGeometryComponent.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

using namespace storm;
using namespace storm::graphics;

struct ColorPassData {
	FrameGraphTextureResourceObserverPtr backbuffer = nullptr;
};

////////////////////////////////////////
////////////////////////////////////////
RenderSystem::RenderSystem(entities::EntityManager &manager,
						   const render::Device &device,
						   render::Surface &surface)
	: entities::System{manager, 1u, {StaticGeometryComponent::TYPE}},
	  m_device{&device}, m_surface{&surface},
	  m_command_buffers{m_surface->imageCount()}, m_frame_builder{*m_device},
	  m_framegraphs{m_surface->imageCount()} {

	m_frame_builder.addPass<ColorPassData>(
		"ColorPass",
		[this](auto &builder, auto &data) {
			data.backbuffer = builder.create(
				"backbuffer",
				FrameGraphTextureDesc{.extent = m_surface->extent(),
									  .format = m_surface->pixelFormat()});
		},
		[this](const auto &data, auto &render_pass, auto &framebuffer,
			   auto &command_buffer) {
			// command_buffer->beginRenderPass(render_pass, framebuffer);
			// TODO PIPELINE ET DRAWABLE CACHE

			for (auto entity : m_entities) {
			}
		});
}

////////////////////////////////////////
////////////////////////////////////////
RenderSystem::~RenderSystem() = default;

////////////////////////////////////////
////////////////////////////////////////
RenderSystem::RenderSystem(RenderSystem &&) = default;

////////////////////////////////////////
////////////////////////////////////////
RenderSystem &RenderSystem::operator=(RenderSystem &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void RenderSystem::preUpdate() {
	m_frame = m_surface->acquireNextFrame();

	m_framegraphs.emplace(m_frame_builder.compile());

	//	m_command_buffers.push(m_graphics_queue->createCommandBuffer());
}

////////////////////////////////////////
////////////////////////////////////////
void RenderSystem::update(std::uint64_t delta) {
}
