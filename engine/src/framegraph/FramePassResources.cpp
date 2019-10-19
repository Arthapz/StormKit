#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

#include <storm/engine/Engine.hpp>

#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FramePassResources::FramePassResources(FrameGraph &frame_graph,
                                       const FramePassBase &frame_pass,
                                       const render::RenderPass &render_pass,
                                       const render::Framebuffer &framebuffer)
    : m_frame_graph { &frame_graph }, m_frame_pass { &frame_pass }, m_render_pass { &render_pass },
      m_framebuffer { &framebuffer } {
}

////////////////////////////////////////
////////////////////////////////////////
FramePassResources::~FramePassResources() = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResources::FramePassResources(FramePassResources &&drawable) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassResources &FramePassResources::operator=(FramePassResources &&drawable) = default;
