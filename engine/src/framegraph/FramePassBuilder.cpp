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
FramePassTextureID FramePassBuilder::sample(FramePassTextureID input) {
    input = read(input);

    m_frame_pass->m_samples.emplace_back(input);

    return input;
}

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureID FramePassBuilder::resolve(FramePassTextureID input) {
    input = write(input);

    m_frame_pass->m_resolves.emplace_back(input);

    return input;
}
