// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#ifdef STORM_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#include <cstdint>
#include <initializer_list>
#include <string>
#include <variant>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/resource/Fwd.hpp>
#include <storm/render/sync/Fwd.hpp>

#include <storm/graphics/framegraph/FrameGraphResources.hpp>
#include <storm/graphics/framegraph/FramePassBuilder.hpp>
#include <storm/graphics/framegraph/Fwd.hpp>

// based on https://github.com/acdemiralp/fg and
// https://media.contentapi.ea.com/content/dam/eacom/frostbite/files/gdc-framegraph.pptx
namespace storm::graphics {
	class STORM_PUBLIC FrameGraph : public core::NonCopyable {
	  public:
		explicit FrameGraph(const render::Device &device);
		~FrameGraph();

		FrameGraph(FrameGraph &&);
		FrameGraph &operator=(FrameGraph &&);

        const render::Semaphore &execute();

		inline render::Texture &backbuffer() {
			STORM_EXPECTS(m_backbuffer != nullptr);
			return *m_backbuffer;
		}
		inline const render::Semaphore &semaphore() const noexcept {
			return *m_semaphore;
		}

        inline void addWaitSemaphore(const render::Semaphore &semaphore) {
            m_wait_semaphores.emplace_back(core::makeConstObserver(&semaphore));
        }

	  private:
        struct Step {
			using Resources = std::vector<FrameGraphTextureResourceRef>;

			FramePassBaseRef frame_pass;
			Resources realized_resources;
			Resources derealized_resources;

			render::RenderPassOwnedPtr render_pass;
			render::FramebufferOwnedPtr frame_buffer;
		};

		using FrameGraphSteps = std::vector<Step>;

		void realize(Step &step);
		void derealize(Step &step);
		void createGPUResources(Step &step);

		render::DeviceConstObserverPtr m_device;
		render::QueueConstObserverPtr m_graphics_queue;

		std::string m_backbuffer_name;
		FrameGraphResources m_resources;
		FrameGraphSteps m_steps;

        std::vector<render::SemaphoreConstObserverPtr> m_wait_semaphores;
		render::FenceOwnedPtr m_fence;
		render::SemaphoreOwnedPtr m_semaphore;
		render::CommandBufferOwnedPtr m_command_buffer;
		render::TextureObserverPtr m_backbuffer;

		friend class FrameGraphBuilder;
		friend class FramePassBuilder;
	};
} // namespace storm::graphics
