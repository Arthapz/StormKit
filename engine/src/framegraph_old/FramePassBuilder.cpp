#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassBuilder.hpp>

using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FramePassBuilder::FramePassBuilder(FrameGraph &frame_graph, FramePassBase &frame_pass) noexcept
    : m_frame_graph { &frame_graph }, m_frame_pass { &frame_pass } {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBuilder::~FramePassBuilder() = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassBuilder::FramePassBuilder(FramePassBuilder &&drawable) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassBuilder &FramePassBuilder::operator=(FramePassBuilder &&drawable) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureResourceObserverPtr
    FramePassBuilder::create(std::string name, FramePassTextureDescription description) {
    auto &texture = m_frame_graph->addTransientTexture(std::move(name), std::move(description));

    m_frame_pass->m_create_textures.emplace_back(&texture);
    texture.m_create_by = m_frame_pass;

    return core::makeObserver(texture);
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBufferResourceObserverPtr
    FramePassBuilder::create(std::string name, FramePassBufferDescription description) {
    auto &buffer = m_frame_graph->addTransientBuffer(std::move(name), std::move(description));

    m_frame_pass->m_create_buffers.emplace_back(&buffer);
    buffer.m_create_by = m_frame_pass;

    return core::makeObserver(buffer);
}

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureResourceObserverPtr FramePassBuilder::read(FramePassTextureResource &texture) {
    m_frame_pass->m_read_textures.emplace_back(&texture);
    texture.m_read_in.emplace_back(m_frame_pass);

    return core::makeObserver(texture);
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBufferResourceObserverPtr FramePassBuilder::read(FramePassBufferResource &buffer) {
    m_frame_pass->m_read_buffers.emplace_back(&buffer);
    buffer.m_read_in.emplace_back(m_frame_pass);

    return core::makeObserver(buffer);
}

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureResourceObserverPtr FramePassBuilder::write(FramePassTextureResource &texture) {
    m_frame_pass->m_write_textures.emplace_back(&texture);
    texture.m_write_in.emplace_back(m_frame_pass);

    return core::makeObserver(texture);
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBufferResourceObserverPtr FramePassBuilder::write(FramePassBufferResource &buffer) {
    m_frame_pass->m_write_buffers.emplace_back(&buffer);
    buffer.m_write_in.emplace_back(m_frame_pass);

    return core::makeObserver(buffer);
}
