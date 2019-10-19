#include <storm/graphics/framegraph/FrameGraph.hpp>
#include <storm/graphics/framegraph/FramePass.hpp>
#include <storm/graphics/framegraph/FramePassBuilder.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
FramePassBuilder::FramePassBuilder(FrameGraph &graph, FramePassBase &pass)
	: m_graph{graph}, m_pass{pass} {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBuilder::~FramePassBuilder() = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResourceObserverPtr
	FramePassBuilder::create(std::string name,
							 FrameGraphTextureDesc description) {
	auto &texture = m_graph.m_resources.addTransientTexture(
		std::move(name), std::move(description));

	m_pass.m_create_textures.emplace_back(texture);
	texture.m_creator = _std::make_observer(&m_pass);

	return _std::make_observer(&texture);
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResourceObserverPtr
	FramePassBuilder::write(FrameGraphTextureResource &resource) {
	m_pass.m_write_textures.emplace_back(resource);
	resource.m_writers.emplace_back(m_pass);

	return _std::make_observer(&resource);
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraphTextureResourceObserverPtr
	FramePassBuilder::read(FrameGraphTextureResource &resource) {
	m_pass.m_read_textures.emplace_back(resource);
	resource.m_readers.emplace_back(m_pass);

	return _std::make_observer(&resource);
}
