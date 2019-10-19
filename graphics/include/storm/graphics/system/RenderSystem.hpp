#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/graphics/framegraph/FrameGraph.hpp>
#include <storm/graphics/framegraph/FrameGraphBuilder.hpp>

#include <storm/entities/System.hpp>

namespace storm::graphics {
	class STORM_PUBLIC RenderSystem : public entities::System {
	  public:
		explicit RenderSystem(entities::EntityManager &manager,
							  const render::Device &device,
							  render::Surface &surface);
		~RenderSystem() override;

		RenderSystem(RenderSystem &&);
		RenderSystem &operator=(RenderSystem &&);

		void preUpdate() override;
		void update(std::uint64_t delta) override;

		FrameGraphBuilder &frameBuilder() noexcept {
			return m_frame_builder;
		}

	  private:
		render::DeviceConstObserverPtr m_device;
		render::SurfaceObserverPtr m_surface;

		core::RingBuffer<render::CommandBufferOwnedPtr> m_command_buffers;

		FrameGraphBuilder m_frame_builder;
		core::RingBuffer<FrameGraph> m_framegraphs;
		render::Surface::Frame m_frame;
	};
} // namespace storm::graphics
