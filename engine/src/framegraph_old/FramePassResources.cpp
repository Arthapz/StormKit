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
FramePassResources::FramePassResources(const engine::Engine &engine,
                                       FrameGraph &frame_graph,
                                       const FramePassBase &frame_pass)
    : m_engine { &engine }, m_frame_graph { &frame_graph }, m_frame_pass { &frame_pass } {
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

////////////////////////////////////////
////////////////////////////////////////
void FramePassResources::realize() {
    if (m_realized) return;

    for (const auto &texture : m_realize_textures) { m_frame_graph->createTexture(*texture); }
    for (const auto &buffer : m_realize_buffers) { m_frame_graph->createBuffer(*buffer); }

    createDescriptorSet();
    createTextureViews();
    createRenderPass();
    createFramebuffer();

    m_realized = true;
}

////////////////////////////////////////
////////////////////////////////////////
void FramePassResources::derealize() {
}

////////////////////////////////////////
////////////////////////////////////////
void FramePassResources::createTextureViews() {
    const auto &device = m_engine->device();

    m_sampler = device.createSamplerPtr(
        { .mag_filter = render::Filter::Nearest, .min_filter = render::Filter::Nearest });

    const auto creates = m_frame_pass->createTextures();
    const auto reads   = m_frame_pass->readTextures();
    const auto writes  = m_frame_pass->writeTextures();

    m_texture_views.reserve(std::size(creates) + std::size(reads) + std::size(writes));
    m_texture_views2.reserve(std::size(creates) + std::size(reads) + std::size(writes));

    for (const auto &resource : writes) {
        const auto &texture = m_frame_graph->getTexture(*resource);

        auto &view = m_texture_views.emplace_back(texture.createView());
        device.setObjectName(view,
                             fmt::format("(FrameGraph) Writed texture view {}", resource->name()));
    }

    for (const auto &resource : creates) {
        const auto &texture = m_frame_graph->getTexture(*resource);
        auto view           = render::TextureViewConstObserverPtr {};

        if (!render::isDepthFormat(texture.format()))
            view = core::makeConstObserver(m_texture_views.emplace_back(texture.createView()));
        else if (render::isDepthOnlyFormat(texture.format()))
            view = core::makeConstObserver(m_texture_views.emplace_back(
                texture.createView(render::TextureViewType::T2D,
                                   { .aspect_mask = render::TextureAspectMask::Depth })));
        else
            view = core::makeConstObserver(m_texture_views.emplace_back(
                texture.createView(render::TextureViewType::T2D,
                                   { .aspect_mask = render::TextureAspectMask::Depth |
                                                    render::TextureAspectMask::Stencil })));

        device.setObjectName(*view,
                             fmt::format("(FrameGraph) Created texture view {}", resource->name()));
    }

    auto descriptors = std::vector<render::Descriptor> {};
    descriptors.reserve(std::size(reads));

    auto i = 0u;
    for (const auto &resource : reads) {
        const auto &texture = m_frame_graph->getTexture(*resource);

        const auto &view = m_texture_views2.emplace_back(texture.createView());

        descriptors.emplace_back(
            render::TextureDescriptor { .type    = render::DescriptorType::Combined_Texture_Sampler,
                                        .binding = i++,
                                        .layout  = render::TextureLayout::Shader_Read_Only_Optimal,
                                        .texture_view = core::makeConstObserver(view),
                                        .sampler      = core::makeConstObserver(m_sampler) });
        device.setObjectName(view,
                             fmt::format("(FrameGraph) Readed texture view {}", resource->name()));
    }

    if (std::size(reads) > 0) { m_descriptor_set->update(std::move(descriptors)); }
}

////////////////////////////////////////
////////////////////////////////////////
void FramePassResources::createRenderPass() {
    const auto &device = m_engine->device();
    m_render_pass      = device.createRenderPassPtr();

    const auto creates = m_frame_pass->createTextures();
    const auto reads   = m_frame_pass->readTextures();
    const auto writes  = m_frame_pass->writeTextures();

    m_extent.w = 0;
    m_extent.h = 0;

    auto subpass_attachment_refs = std::vector<render::RenderPass::Subpass::Ref> {};
    subpass_attachment_refs.reserve(std::size(creates) + std::size(reads) + std::size(writes));

    const auto detectOp = [this](FramePassBaseResource &resource) {
        const auto read_in  = resource.readIn();
        const auto write_in = resource.writeIn();

        const auto is_created = m_frame_pass == resource.createdBy();
        const auto is_readed =
            std::find(std::cbegin(read_in), std::cend(read_in), m_frame_pass) != std::cend(read_in);
        const auto is_writed =
            std::find(std::cbegin(write_in), std::cend(write_in), m_frame_pass) !=
            std::cend(write_in);

        auto load_op  = render::AttachmentLoadOperation::Dont_Care;
        auto store_op = render::AttachmentStoreOperation::Dont_Care;

        if (is_readed && !is_writed) {
            load_op  = render::AttachmentLoadOperation::Load;
            store_op = render::AttachmentStoreOperation::Dont_Care;
        } else if (is_readed && is_writed) {
            load_op  = render::AttachmentLoadOperation::Load;
            store_op = render::AttachmentStoreOperation::Store;
        } else if (!is_readed && is_writed) {
            load_op  = render::AttachmentLoadOperation::Dont_Care;
            store_op = render::AttachmentStoreOperation::Store;
        }

        if (resource.name() == "Backbuffer" && resource.writeIn()[0] == m_frame_pass)
            load_op = render::AttachmentLoadOperation::Clear;

        if (is_created) load_op = render::AttachmentLoadOperation::Clear;

        return std::make_pair(load_op, store_op);
    };

    for (const auto &resource : writes) {
        const auto &texture = m_frame_graph->getTexture(*resource);

        const auto [load_op, store_op]    = detectOp(*resource);
        const auto attachment_description = render::RenderPass::AttachmentDescription {
            .format             = texture.format(),
            .samples            = resource->description().sample_count,
            .load_op            = load_op,
            .store_op           = store_op,
            .source_layout      = resource->description().source_layout,
            .destination_layout = resource->description().destination_layout,
            .resolve            = false // resource->description().resolved
        };

        auto id = m_render_pass->addAttachmentDescription(std::move(attachment_description));

        const auto subpass_ref = render::RenderPass::Subpass::Ref { .attachment_id = id };
        subpass_attachment_refs.emplace_back(std::move(subpass_ref));

        m_extent.w = core::max(m_extent.w, texture.extent().w);
        m_extent.h = core::max(m_extent.h, texture.extent().h);
    }

    for (const auto &resource : creates) {
        const auto &texture = m_frame_graph->getTexture(*resource);

        const auto [load_op, store_op] = detectOp(*resource);

        const auto attachment_description = render::RenderPass::AttachmentDescription {
            .format             = texture.format(),
            .load_op            = load_op,
            .store_op           = store_op,
            .source_layout      = resource->description().source_layout,
            .destination_layout = resource->description().destination_layout
        };

        auto id = m_render_pass->addAttachmentDescription(std::move(attachment_description));

        const auto subpass_ref =
            render::RenderPass::Subpass::Ref { .attachment_id = id,
                                               .layout =
                                                   resource->description().destination_layout };
        subpass_attachment_refs.emplace_back(std::move(subpass_ref));

        m_extent.w = core::max(m_extent.w, texture.extent().w);
        m_extent.h = core::max(m_extent.h, texture.extent().h);
    }

    /*for (const auto &resource : reads) {
        const auto &texture = m_frame_graph->getTexture(*resource);

        const auto [load_op, store_op]	  = detectOp(*resource);
        const auto attachment_description = render::RenderPass::AttachmentDescription {
            .format				= texture.format(),
            .load_op			= load_op,
            .store_op			= store_op,
            .source_layout		= resource->description().destination_layout,
            .destination_layout = storm::render::TextureLayout::Shader_Read_Only_Optimal};

        auto id = m_render_pass->addAttachmentDescription(std::move(attachment_description));

        const auto subpass_ref = render::RenderPass::Subpass::Ref {.attachment_id = id};
        subpass_attachment_refs.emplace_back(std::move(subpass_ref));

        m_extent.w = core::max(m_extent.w, texture.extent().w);
        m_extent.h = core::max(m_extent.h, texture.extent().h);
    }*/

    m_render_pass->addSubpass({ .bind_point      = storm::render::PipelineBindPoint::Graphics,
                                .attachment_refs = std::move(subpass_attachment_refs) });

    m_render_pass->build();

    device.setObjectName(*m_render_pass,
                         fmt::format("(FrameGraph) RenderPass {}", m_frame_pass->name()));
}

////////////////////////////////////////
////////////////////////////////////////
void FramePassResources::createFramebuffer() {
    const auto &device = m_engine->device();

    m_framebuffer =
        m_render_pass->createFramebufferPtr(m_extent, core::toConstObservers(m_texture_views));
    device.setObjectName(*m_framebuffer,
                         fmt::format("(FrameGraph) Framebuffer {}", m_frame_pass->name()));
}

////////////////////////////////////////
////////////////////////////////////////
void FramePassResources::createDescriptorSet() {
    const auto &device = m_engine->device();
    const auto &pool   = m_engine->descriptorPool();

    const auto reads      = m_frame_pass->readTextures();
    const auto read_count = gsl::narrow_cast<core::UInt32>(std::size(reads));

    if (read_count == 0) return;

    m_descriptor_layout = device.createDescriptorSetLayoutPtr();

    for (auto i = 0u; i < read_count; ++i) {
        m_descriptor_layout->addBinding(
            { i,
              render::DescriptorType::Combined_Texture_Sampler,
              render::ShaderStage::Vertex | render::ShaderStage::Fragment,
              1 });
    }

    m_descriptor_layout->bake();
    m_descriptor_set = pool.allocateDescriptorSetPtr(*m_descriptor_layout);
    device.setObjectName(*m_descriptor_set,
                         fmt::format("(FrameGraph) Descriptor Set {}", m_frame_pass->name()));
}
