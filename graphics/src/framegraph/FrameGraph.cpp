#include <storm/graphics/framegraph/FrameGraph.hpp>
#include <storm/graphics/framegraph/FramePass.hpp>

#include <stack>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>
#include <storm/render/resource/Texture.hpp>
#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::graphics;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(const render::Device &device)
	: m_device{core::makeConstObserver(&device)},
	  m_graphics_queue{core::makeConstObserver(&m_device->graphicsQueue())},
	  m_resources{device} {

	m_fence			 = m_device->createFence();
	m_semaphore		 = m_device->createSemaphore();
    m_command_buffer = m_graphics_queue->createCommandBuffer();
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::~FrameGraph() {
    if (m_fence) {
        m_device->waitForFence(*m_fence);
        for (auto &step : m_steps) {
            derealize(step);
        }
    }
};

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(FrameGraph &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph &FrameGraph::operator=(FrameGraph &&) = default;

////////////////////////////////////////
////////////////////////////////////////
const render::Semaphore &FrameGraph::execute() {
    m_command_buffer->reset();

    m_command_buffer->begin(true);

    for (auto &step : m_steps) {
        realize(step);

        step.frame_pass.get().execute(*step.render_pass, *step.frame_buffer,
                                      *m_command_buffer);
    }

    /*if (m_backbuffer != nullptr)
        m_command_buffer->transitionImageLayout(
            *m_backbuffer, render::ImageLayout::Shader_Read_Only_Optimal);*/
    m_command_buffer->end();

    m_command_buffer->build();

    m_graphics_queue->submit(
        core::makeConstObserverArray(m_command_buffer), m_wait_semaphores,
        core::makeConstObserverArray(m_semaphore), core::makeObserver(m_fence));

    return *m_semaphore;
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::realize(FrameGraph::Step &step) {
	for (auto &resource : step.realized_resources) resource.get().realize();

	createGPUResources(step);
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::derealize(FrameGraph::Step &step) {
	step.frame_buffer.reset(nullptr);

	for (auto &resource : step.derealized_resources) resource.get().derealize();
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::createGPUResources(FrameGraph::Step &step) {
	auto texture_ress = step.frame_pass.get().m_create_textures;
	texture_ress.insert(std::end(texture_ress),
						std::begin(step.frame_pass.get().m_write_textures),
						std::end(step.frame_pass.get().m_write_textures));

	if (!step.render_pass) {
		step.render_pass = m_device->createRenderPass();

		auto attachment_refs = std::vector<render::RenderPass::Subpass::Ref>{};

        for (auto &texture_res : texture_ress) {
			const auto &texture_desc = texture_res.get().description();

            const auto attachment = render::RenderPass::Attachment{
                .format		  = texture_desc.format,
                .samples	  = texture_desc.samples,
                .final_layout = texture_desc.layout};

            const auto id =
                step.render_pass->addAttachment(std::move(attachment));

            auto layout = texture_desc.layout;
            if (layout == render::ImageLayout::Present_Src)
                layout = render::ImageLayout::Color_Attachment_Optimal;

            attachment_refs.emplace_back(
                render::RenderPass::Subpass::Ref{id, layout});
		}

        step.render_pass->addSubpass(render::RenderPass::Subpass{
            render::PipelineBindPoint::Graphics, std::move(attachment_refs)});
        step.render_pass->build();
    }

    auto extent			  = core::Extent{};
    auto extent_swapchain = false;
    auto textures		  = std::vector<render::TextureConstObserverPtr>{};
    textures.reserve(std::size(texture_ress));

    for (const auto &texture_res : texture_ress) {
        if (texture_res.get().name() == m_backbuffer_name)
            m_backbuffer = _std::make_observer<render::Texture>(
                &texture_res.get().texture());

        const auto &texture = texture_res.get().texture();
        textures.emplace_back(core::makeConstObserver(&texture));

        if (texture_res.get().description().layout ==
            render::ImageLayout::Present_Src) {
            extent			 = texture.extent();
            extent_swapchain = true;
        }

        if (!extent_swapchain) {
            extent.w = std::max(extent.w, texture.extent().w);
            extent.h = std::max(extent.h, texture.extent().h);
        }
	}

    step.frame_buffer = step.render_pass->createFramebuffer(
        std::move(extent), std::move(textures));
}
