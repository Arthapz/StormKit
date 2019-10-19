#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonMovable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/graphics/framegraph/FrameGraphResources.hpp>
#include <storm/graphics/framegraph/Fwd.hpp>

namespace storm::graphics {
	class STORM_PUBLIC FramePassBuilder : public core::NonCopyable,
										  public core::NonMovable {
	  public:
		FramePassBuilder(FrameGraph &graph, FramePassBase &pass);
		~FramePassBuilder();

		FrameGraphTextureResourceObserverPtr
			create(std::string name, FrameGraphTextureDesc description);
		FrameGraphTextureResourceObserverPtr
			write(FrameGraphTextureResource &resource);
		FrameGraphTextureResourceObserverPtr
			read(FrameGraphTextureResource &resource);

	  private:
		FrameGraph &m_graph;
		FramePassBase &m_pass;
	};
} // namespace storm::graphics
