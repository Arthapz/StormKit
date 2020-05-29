#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Profiler.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/sync/Fence.hpp>

#include <storm/core/DryComparisons.hpp>
#include <storm/core/Ranges.hpp>
#include <storm/core/Strings.hpp>

#include <deque>
#include <iterator>
#include <list>
#include <stack>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(Engine &engine) noexcept : m_engine { &engine } {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::~FrameGraph() {
    while (!std::empty(m_derealize_queue)) {
        auto &to_derealize = *m_derealize_queue.front();

        if (to_derealize.fence->wait() == render::WaitResult::Success) {
            for (auto handle : to_derealize.derealize_resource) {
                auto &resource = getResourceBase(handle);
                resource.derealize();
            }

            m_derealize_queue.pop();
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(FrameGraph &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph &FrameGraph::operator=(FrameGraph &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureID FrameGraph::presentTo(std::string name,
                                         render::Texture &texture,
                                         const render::Semaphore &available,
                                         const render::Semaphore &render_finished,
                                         render::Fence &finished) {
    const auto backbuffer_descriptor =
        FramePassTexture::Descriptor { .type   = render::TextureType::T2D,
                                       .format = texture.format(),
                                       .extent = texture.extent(),
                                       .usage  = render::TextureUsage::Color_Attachment };

    m_present_resource =
        setRetainedResource<FrameGraphTexture>(std::move(name),
                                               std::move(backbuffer_descriptor),
                                               { .storage = core::makeObserver(texture) });

    m_wait_semaphores   = core::makeConstObservers(available);
    m_signal_semaphores = core::makeConstObservers(render_finished);
    m_present_fence     = core::makeObserver(finished);

    return m_present_resource;
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::reset() {
    while (!std::empty(m_derealize_queue)) {
        auto &to_derealize = *m_derealize_queue.front();

        if (to_derealize.fence->wait() == render::WaitResult::Success) {
            for (auto handle : to_derealize.derealize_resource) {
                auto &resource = getResourceBase(handle);
                resource.derealize();
            }

            m_derealize_queue.pop();
        }
    }

    m_frame_passes.clear();
    m_resources.clear();

    m_wait_semaphores.clear();
    m_signal_semaphores.clear();

    m_present_fence = nullptr;

    m_timeline.clear();

    m_resource_id = 0u;
    m_pass_id     = 0u;
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::compile() noexcept {
    auto &profiler = m_engine->profiler();

    profiler.beginStage("Resource culling", 2);
    cullResources();
    profiler.endStage("Resource culling");

    profiler.beginStage("Timeline compute", 2);
    auto passes = std::vector<core::UInt32> {};

    for (const auto &pass : m_frame_passes) passes.emplace_back(pass->id());

    computeTimeline(passes);
    profiler.endStage("Timeline compute");

    profiler.beginStage("GPU objects creation", 2);
    prepareGPUObjects();
    profiler.endStage("GPU objects creation");
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::execute() {
    auto &profiler     = m_engine->profiler();
    const auto &device = m_engine->device();

    auto i = 0u;
    for (auto &step : m_timeline) {
        const auto step_stage = fmt::format("Step {} FrameBuffer creation", step.pass_name);
        profiler.beginStage(step_stage, 2);

        if (!std::empty(m_derealize_queue)) {
            auto &to_derealize = *m_derealize_queue.front();

            if (to_derealize.fence->status() == render::Fence::Status::Signaled) {
                for (auto handle : to_derealize.derealize_resource) {
                    auto &resource = getResourceBase(handle);
                    resource.derealize();
                }

                m_derealize_queue.pop();
            }
        }

        for (auto handle : step.realize_resource) {
            auto &resource = getResourceBase(handle);
            resource.realize(device);
        }

        auto texture_views = render::TextureViewConstObserverPtrArray {};

        for (const auto &handle : step.framebuffer_attachments) {
            const auto &resource     = getResourceBase(handle);
            const auto *pass_texture = static_cast<const FramePassTexture *>(&resource);
            const auto &texture      = pass_texture->resource().texture();

            auto aspect_mask = render::TextureAspectMask::Color;
            if (pass_texture->descriptor().usage ==
                render::TextureUsage::Depth_Stencil_Attachment) {
                aspect_mask = render::TextureAspectMask::Depth;
                if (render::isDepthStencilFormat(pass_texture->descriptor().format))
                    aspect_mask |= render::TextureAspectMask::Stencil;
            }

            auto &texture_view =
                m_texture_views.emplace_back(texture.createViewPtr(render::TextureViewType::T2D,
                                                                   { .aspect_mask = aspect_mask }));
            texture_views.emplace_back(core::makeConstObserver(texture_view));
        }

        step.frame_buffer =
            step.render_pass->createFramebufferPtr(step.framebuffer_extent, texture_views);
        device.setObjectName(*step.frame_buffer, fmt::format("{}:FramePass", step.pass_name));
        profiler.endStage(step_stage);

        const auto step_execution = fmt::format("Step {} execution", i);
        profiler.beginStage(step_execution, 2);
        step.cmb->begin(true);

        step.cmb->beginDebugRegion(fmt::format("{} pre-execution", step.pass_name),
                                   core::RGBColorDef::Blue<float>);
        step.pre_execute(*step.cmb);
        step.cmb->endDebugRegion();

        step.cmb->beginRenderPass(*step.render_pass,
                                  *step.frame_buffer,
                                  step.clear_values,
                                  step.use_sub_command_buffers);
        step.cmb->beginDebugRegion(fmt::format("{} execution", step.pass_name));

        for (auto subpass : step.pass_id) {
            const auto &pass = m_frame_passes[subpass];
            auto resources   = FramePassResources {
                *this,
                *pass,
                *step.render_pass,
                *step.frame_buffer,
            };

            const auto step_subpass = fmt::format("SubPass {} execution", step.pass_name);
            profiler.beginStage(step_subpass, 3);
            pass->execute(resources, *step.cmb);
            profiler.endStage(step_subpass);
        }

        step.cmb->endDebugRegion();
        step.cmb->endRenderPass();

        step.cmb->beginDebugRegion(fmt::format("{} post-execution", step.pass_name),
                                   core::RGBColorDef::Blue<float>);
        step.post_execute(*step.cmb);
        step.cmb->endDebugRegion();

        step.cmb->end();

        step.cmb->build();

        if (i == std::size(m_timeline) - 1)
            step.cmb->submit(m_wait_semaphores, m_signal_semaphores, m_present_fence);
        else
            step.cmb->submit();

        profiler.endStage(step_execution);
        ++i;
    }
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::cullResources() {
    for (auto &pass : m_frame_passes) pass->m_ref_count = std::size(pass->writes());
    for (auto &resource : m_resources) resource->m_ref_count = std::size(resource->readIn());

    auto unreferenced_resources = std::stack<FramePassResourceBaseObserverPtr> {};
    for (auto &resource : m_resources) {
        if (resource->m_ref_count == 0 && resource->isTransient())
            unreferenced_resources.emplace(core::makeObserver(resource));
    }

    while (!std::empty(unreferenced_resources)) {
        auto resource = unreferenced_resources.top();
        unreferenced_resources.pop();

        auto creator_id = resource->createdBy();
        auto &creator   = m_frame_passes[creator_id];
        if (creator->m_ref_count > 0) creator->m_ref_count--;

        if (creator->m_ref_count == 0 && !creator->isCullImune()) {
            for (auto handle : creator->m_reads) {
                auto &read_resource = getResourceBase(handle);
                if (read_resource.m_ref_count > 0) read_resource.m_ref_count--;

                if (read_resource.m_ref_count == 0 && read_resource.isTransient())
                    unreferenced_resources.emplace(&read_resource);
            }
        }

        for (auto &writer_id : resource->writeIn()) {
            auto &writer = m_frame_passes[writer_id];
            if (writer->m_ref_count > 0) writer->m_ref_count--;

            if (writer->m_ref_count == 0 && !writer->isCullImune()) {
                for (auto handle : writer->m_reads) {
                    auto &read_resource = getResourceBase(handle);

                    if (read_resource.m_ref_count > 0) read_resource.m_ref_count--;

                    if (read_resource.m_ref_count == 0 && read_resource.isTransient())
                        unreferenced_resources.emplace(&read_resource);
                }
            }
        }
    }
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::computeTimeline(core::span<const core::UInt32> passes) {
    for (auto pass_id : passes) {
        auto &pass = *m_frame_passes[pass_id];

        if (pass.m_ref_count == 0 && !pass.isCullImune()) continue;

        auto step = Step {};

        for (auto resource_handle : pass.creates()) {
            step.realize_resource.emplace_back(resource_handle);

            auto &resource = getResourceBase(resource_handle);

            const auto reads  = resource.readIn();
            const auto writes = resource.readIn();

            const auto is_readed_by_other =
                std::size(reads) > 1u && core::ranges::find_if(reads, [&pass](const auto id) {
                                             return id != pass.id();
                                         }) != core::ranges::end(reads);
            const auto is_writed_by_other =
                std::size(writes) > 1u && core::ranges::find_if(writes, [&pass](const auto id) {
                                              return id != pass.id();
                                          }) != core::ranges::end(writes);

            if (!is_readed_by_other && !is_writed_by_other)
                step.derealize_resource.emplace_back(resource_handle);
        }

        // auto reads_writes = pass.reads() | core::ranges::view::concat |
        // pass.writes();
        const auto pass_reads  = pass.reads();
        const auto pass_writes = pass.writes();
        auto reads_writes = std::vector<FramePassResourceHandle> { core::ranges::begin(pass_reads),
                                                                   core::ranges::end(pass_reads) };
        reads_writes.reserve(std::size(pass_writes));
        std::copy(core::ranges::begin(pass_writes),
                  core::ranges::end(pass_writes),
                  std::back_inserter(reads_writes));

        for (auto resource_handle : reads_writes) {
            auto &resource = getResourceBase(resource_handle);

            if (!resource.isTransient()) continue;

            const auto reads  = resource.readIn();
            const auto writes = resource.writeIn();

            auto last_index = std::optional<core::UInt32> {};
            if (!std::empty(reads)) {
                auto last_reader = core::ranges::find_if(passes, [&reads](auto handle) {
                    return handle == reads.back();
                });

                if (last_reader != core::ranges::end(passes)) last_index = *last_reader;
            }

            if (!std::empty(writes)) {
                auto last_reader = core::ranges::find_if(passes, [&writes](auto handle) {
                    return handle == writes.back();
                });

                if (last_reader != core::ranges::end(passes))
                    last_index = std::max(last_index.value_or(0u), *last_reader);
            }

            if (last_index.has_value() && last_index.value() == pass.id() &&
                resource_handle != m_present_resource)
                step.derealize_resource.emplace_back(resource_handle);
        }

        step.pass_id.emplace_back(pass_id);
        m_timeline.emplace_back(std::move(step));
    }
}

void FrameGraph::prepareGPUObjects() {
    const auto &device = m_engine->device();

    for (auto &step : m_timeline) {
        step.render_pass = device.createRenderPassPtr();

        auto attachments = std::vector<std::pair<FramePassTextureID, core::UInt32>> {};

        auto pre_executes =
            std::vector<_std::observer_ptr<const FramePassBase::PreExecuteCallback>> {};
        pre_executes.reserve(std::size(step.pass_id));

        auto post_executes =
            std::vector<_std::observer_ptr<const FramePassBase::PreExecuteCallback>> {};
        post_executes.reserve(std::size(step.pass_id));

        for (auto sub_step : step.pass_id) {
            auto &pass = m_frame_passes[sub_step];

            pre_executes.emplace_back(core::makeConstObserver(pass->preExecuteCallback()));
            post_executes.emplace_back(core::makeConstObserver(pass->postExecuteCallback()));

            step.pass_name = pass->name();

            auto sub_pass = render::RenderPass::Subpass {
                .bind_point = render::PipelineBindPoint::Graphics,
            };

            step.use_sub_command_buffers |= pass->isUsingSubCommandBuffer();

            const auto creates  = pass->creates();
            const auto reads    = pass->reads();
            const auto writes   = pass->writes();
            const auto samples  = pass->samples();
            const auto resolves = pass->resolves();

            auto read_only_attachments  = std::vector<FramePassTextureID> {};
            auto read_write_attachments = std::vector<FramePassTextureID> {};
            auto write_only_attachments = std::vector<FramePassTextureID> {};

            for (auto read : reads) {
                const auto &pass_resource = getResourceBase(read);
                const auto *pass_texture  = dynamic_cast<const FramePassTexture *>(&pass_resource);
                if (!pass_texture) continue;

                const auto &descriptor = pass_texture->descriptor();

                if (core::ranges::find_if(writes, [&read](auto w) {
                        return w.index() == read.index();
                    }) != core::ranges::end(writes))
                    read_write_attachments.emplace_back(read);
                else
                    read_only_attachments.emplace_back(read);

                step.framebuffer_extent.width =
                    std::max(step.framebuffer_extent.width, descriptor.extent.width);
                step.framebuffer_extent.height =
                    std::max(step.framebuffer_extent.height, descriptor.extent.height);
            }

            for (auto write : writes) {
                const auto &pass_resource = getResourceBase(write);
                const auto *pass_texture  = dynamic_cast<const FramePassTexture *>(&pass_resource);
                if (!pass_texture) continue;

                const auto &descriptor = pass_texture->descriptor();

                if (core::ranges::find_if(read_write_attachments, [&write](auto r) {
                        return write.index() == r.index();
                    }) == core::ranges::end(read_write_attachments))
                    write_only_attachments.emplace_back(write);

                step.framebuffer_extent.width =
                    std::max(step.framebuffer_extent.width, descriptor.extent.width);
                step.framebuffer_extent.height =
                    std::max(step.framebuffer_extent.height, descriptor.extent.height);
            }

            for (const auto &handle : read_only_attachments) {}
            for (const auto &handle : write_only_attachments) {
                const auto &resource   = getResource<FrameGraphTexture>(handle);
                const auto &descriptor = resource.descriptor();

                const auto is_depth_attachment =
                    core::checkFlag(descriptor.usage,
                                    render::TextureUsage::Depth_Stencil_Attachment);
                const auto subpass_layout =
                    (is_depth_attachment) ? render::TextureLayout::Depth_Stencil_Attachment_Optimal
                                          : render::TextureLayout::Color_Attachment_Optimal;

                auto it = core::ranges::find_if(attachments, [&handle](const auto &pair) {
                    return pair.first == handle;
                });

                if (it != core::ranges::end(attachments)) {
                    sub_pass.attachment_refs.emplace_back(render::RenderPass::Subpass::Ref {
                        it->second,
                        subpass_layout,
                    });

                    continue;
                }

                auto resolve = core::ranges::find_if(resolves, [&handle](auto h) {
                                   return h.index() == handle.index();
                               }) != core::ranges::end(resolves);

                auto attachment = createWriteOnlyDescription(resource, sub_step, resolve);

                if (attachment.load_op == render::AttachmentLoadOperation::Clear) {
                    if (is_depth_attachment) {
                        step.clear_values.emplace_back(render::ClearDepthStencil {});
                    } else {
                        step.clear_values.emplace_back(
                            render::ClearColor { .color = core::RGBColorDef::Silver<float> });
                    }
                }

                auto id = step.render_pass->addAttachmentDescription(std::move(attachment));

                sub_pass.attachment_refs.emplace_back(
                    render::RenderPass::Subpass::Ref { id, subpass_layout });

                attachments.emplace_back(handle, id);
            }
            for (const auto &handle : read_write_attachments) {
                const auto &resource   = getResource<FrameGraphTexture>(handle);
                const auto &descriptor = resource.descriptor();

                const auto is_depth_attachment =
                    core::checkFlag(descriptor.usage,
                                    render::TextureUsage::Depth_Stencil_Attachment);
                const auto subpass_layout =
                    (is_depth_attachment) ? render::TextureLayout::Depth_Stencil_Attachment_Optimal
                                          : render::TextureLayout::Color_Attachment_Optimal;

                auto it = core::ranges::find_if(attachments, [&handle](const auto &pair) {
                    return pair.first == handle;
                });

                if (it != core::ranges::end(attachments)) {
                    sub_pass.attachment_refs.emplace_back(
                        render::RenderPass::Subpass::Ref { it->second, subpass_layout });

                    continue;
                }

                auto attachment = createReadWriteDescription(resource, sub_step);
                auto id         = step.render_pass->addAttachmentDescription(std::move(attachment));

                sub_pass.attachment_refs.emplace_back(
                    render::RenderPass::Subpass::Ref { id, subpass_layout });

                attachments.emplace_back(handle, id);
            }

            for (const auto [id, _] : attachments) step.framebuffer_attachments.emplace_back(id);

            step.render_pass->addSubpass(std::move(sub_pass));
        }

        step.pre_execute = [pre_executes { std::move(pre_executes) }](render::CommandBuffer &cmb) {
            for (auto &execute : pre_executes) (*execute)(cmb);
        };
        step.post_execute =
            [post_executes { std::move(post_executes) }](render::CommandBuffer &cmb) {
                for (auto &execute : post_executes) (*execute)(cmb);
            };

        step.render_pass->build();
        device.setObjectName(*step.render_pass, fmt::format("StormKit:{}", step.pass_name));
        step.cmb = device.graphicsQueue().createCommandBufferPtr();
        device.setObjectName(*step.cmb, fmt::format("StormKit:{}:CommandBuffer", step.pass_name));
    }
}

////////////////////////////////////////
////////////////////////////////////////
render::RenderPass::AttachmentDescription
    FrameGraph::createDescription(const FramePassTexture &resource,
                                  render::AttachmentLoadOperation load_op,
                                  render::AttachmentStoreOperation store_op,
                                  render::TextureLayout source_layout,
                                  render::TextureLayout destination_layout,
                                  bool resolve) {
    const auto &descriptor = resource.descriptor();

    return render::RenderPass::AttachmentDescription { .format             = descriptor.format,
                                                       .samples            = descriptor.samples,
                                                       .load_op            = load_op,
                                                       .store_op           = store_op,
                                                       .source_layout      = source_layout,
                                                       .destination_layout = destination_layout,
                                                       .resolve            = resolve };
}

////////////////////////////////////////
////////////////////////////////////////
render::RenderPass::AttachmentDescription
    FrameGraph::createReadOnlyDescription(const FramePassTexture &resource) {
    const auto &descriptor = resource.descriptor();

    return {};
    // createDescription(resource);
}

////////////////////////////////////////
////////////////////////////////////////
render::RenderPass::AttachmentDescription
    FrameGraph::createReadWriteDescription(const FramePassTexture &resource, core::UInt32 pass_id) {
    const auto &descriptor = resource.descriptor();
    const auto is_depth_attachment =
        core::checkFlag(descriptor.usage, render::TextureUsage::Depth_Stencil_Attachment);

    auto load_op       = render::AttachmentLoadOperation::Load;
    auto store_op      = render::AttachmentStoreOperation::Store;
    auto source_layout = (is_depth_attachment)
                             ? render::TextureLayout::Depth_Stencil_Attachment_Optimal
                             : render::TextureLayout::Color_Attachment_Optimal;
    auto destination_layout = source_layout;

    if (resource.id() == m_present_resource.index()) {
        auto read_in = resource.readIn();
        if (std::empty(read_in) || read_in.back() == pass_id)
            destination_layout = render::TextureLayout::Present_Src;

        destination_layout = render::TextureLayout::Present_Src;
    }

    if (std::empty(resource.readIn())) store_op = render::AttachmentStoreOperation::Dont_Care;

    return createDescription(resource, load_op, store_op, source_layout, destination_layout, false);
}

////////////////////////////////////////
////////////////////////////////////////
render::RenderPass::AttachmentDescription
    FrameGraph::createWriteOnlyDescription(const FramePassTexture &resource,
                                           core::UInt32 pass_id,
                                           bool resolve) {
    const auto &descriptor = resource.descriptor();

    const auto is_depth_attachment =
        core::checkFlag(descriptor.usage, render::TextureUsage::Depth_Stencil_Attachment);

    const auto created_by = resource.createdBy() == pass_id;

    auto load_op            = render::AttachmentLoadOperation::Dont_Care;
    auto store_op           = render::AttachmentStoreOperation::Store;
    auto source_layout      = render::TextureLayout::Undefined;
    auto destination_layout = (is_depth_attachment)
                                  ? render::TextureLayout::Depth_Stencil_Attachment_Optimal
                                  : render::TextureLayout::Color_Attachment_Optimal;

    if (created_by) load_op = render::AttachmentLoadOperation::Clear;

    if (resource.id() == m_present_resource.index()) {
        load_op = render::AttachmentLoadOperation::Clear;

        auto read_in = resource.readIn();
        if (std::empty(read_in) || read_in.back() == pass_id)
            destination_layout = render::TextureLayout::Present_Src;

        if (resolve) load_op = render::AttachmentLoadOperation::Dont_Care;
    }

    if (std::empty(resource.readIn())) store_op = render::AttachmentStoreOperation::Dont_Care;

    return createDescription(resource,
                             load_op,
                             store_op,
                             source_layout,
                             destination_layout,
                             resolve);
}

////////////////////////////////////////
////////////////////////////////////////
std::string FrameGraph::exportGraphviz() {
    auto graphivz = std::string { "digraph framegraph {\n" };

    graphivz += "   rankdir = LR\n";
    graphivz += "   bgcolor = black\n";
    graphivz += "   node [shape=rectangle, fontname=\"helvetica\", fontsize=12]\n\n";

    for (const auto &pass : m_frame_passes) {
        graphivz += fmt::format("   \"{}\" [label=\"{}\\nID: {}\\nRefs: {}\", style=filled, "
                                "fillcolor=darkorange]\n",
                                pass->name(),
                                pass->name(),
                                pass->id(),
                                pass->m_ref_count);
    }

    graphivz += '\n';

    for (const auto &resource : m_resources) {
        graphivz += fmt::format(
            "   \"{}\" [label=\"{}\\nID: {}\\nRefs: {}\", style=filled, fillcolor={}]\n",
            resource->name(),
            resource->name(),
            resource->id(),
            resource->m_ref_count,
            (resource->isTransient()) ? "skyblue" : "steelblue");
    }

    graphivz += '\n';

    for (const auto &pass : m_frame_passes) {
        graphivz += fmt::format("   \"{}\" -> {{", pass->name());

        for (auto handle : pass->creates()) {
            const auto &resource = getResourceBase(handle);

            graphivz += fmt::format("       \"{}\"\n", resource.name());
        }

        graphivz += "    } [color=seagreen]\n";

        graphivz += fmt::format("   \"{}\" -> {{", pass->name());

        const auto pass_creates = pass->creates();
        for (auto handle : pass->writes()) {
            if (core::ranges::find(pass_creates, handle) != core::ranges::end(pass_creates))
                continue;

            const auto &resource = getResourceBase(handle);

            graphivz += fmt::format("       \"{}\"\n", resource.name());
        }

        graphivz += "    } [color=gold]\n";
    }

    graphivz += '\n';

    for (const auto &resource : m_resources) {
        graphivz += fmt::format("   \"{}\" -> {{", resource->name());

        for (auto handle : resource->readIn()) {
            const auto &pass = m_frame_passes[handle];

            graphivz += fmt::format("       \"{}\"\n", pass->name());
        }

        graphivz += "    } [color=firebrick]\n";
    }

    graphivz += "}";
    return graphivz;
}

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceHandle FrameGraph::create(FramePassResourceBaseOwnedPtr resource) {
    auto index = gsl::narrow_cast<core::UInt32>(std::size(m_resources));

    m_resources.emplace_back(std::move(resource));

    return FramePassResourceHandle { index };
}

////////////////////////////////////////
////////////////////////////////////////
FramePassResourceBase &FrameGraph::getResourceBase(FramePassResourceHandle handle) noexcept {
    STORM_EXPECTS(handle.isValid());
    return *m_resources[handle.index()];
}

////////////////////////////////////////
////////////////////////////////////////
const FramePassResourceBase &
    FrameGraph::getResourceBase(FramePassResourceHandle handle) const noexcept {
    STORM_EXPECTS(handle.isValid());
    return *m_resources[handle.index()];
}
