#include <storm/engine/framegraph/FrameGraph.hpp>
#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassResources.hpp>

#include <storm/engine/Engine.hpp>

#include <storm/engine/core/Profiler.hpp>

#include <storm/render/core/CommandBuffer.hpp>

#include <storm/core/DryComparisons.hpp>

#include <deque>
#include <iterator>
#include <list>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(Engine &engine) noexcept : m_engine { &engine } {
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::~FrameGraph() = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FrameGraph(FrameGraph &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FrameGraph &FrameGraph::operator=(FrameGraph &&) = default;

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureResource &FrameGraph::setRetainedTexture(std::string name,
                                                         const render::Texture &texture,
                                                         FramePassTextureDescription description) {
    auto resource = std::make_unique<FramePassTextureResource>(name, std::move(description), false);

    auto [it, success] = m_retained_textures.emplace(
        std::move(name),
        std::make_pair<FramePassTextureResourceOwnedPtr, render::TextureConstObserverPtr>(
            std::move(resource),
            core::makeConstObserver(texture)));

    return *it->second.first;
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBufferResource &FrameGraph::setRetainedTexture(std::string name,
                                                        const render::HardwareBuffer &buffer,
                                                        FramePassBufferDescription description) {
    auto resource = std::make_unique<FramePassBufferResource>(name, std::move(description), false);

    auto [it, success] = m_retained_buffers.emplace(
        std::move(name),
        std::make_pair<FramePassBufferResourceOwnedPtr, render::HardwareBufferConstObserverPtr>(
            std::move(resource),
            core::makeConstObserver(buffer)));

    return *it->second.first;
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::bake() {
    auto passes = FramePassBaseObserverPtrArray {};
    passes.reserve(std::size(m_frame_passes));
    std::transform(std::begin(m_frame_passes),
                   std::end(m_frame_passes),
                   std::back_inserter(passes),
                   [](auto &ptr) { return core::makeObserver(ptr); });

    cullPasses(passes);
    auto merged = mergePasses(passes);

    for (auto &[pass, parent] : merged) {
        auto &resources = m_step_resources.emplace_back(
            std::make_unique<FramePassResources>(*m_engine, *this, *pass));

        for (const auto &texture : pass->createTextures()) {
            resources->m_realize_textures.emplace_back(core::makeConstObserver(texture));
        }

        auto transitions =
            std::unordered_map<FramePassTextureResourceConstObserverPtr, render::TextureLayout> {};
        for (const auto &texture : pass->readTextures()) {
            const auto write_in = texture->writeIn();
            const auto is_writed =
                std::find(std::cbegin(write_in), std::cend(write_in), pass) != std::cend(write_in);

            if (!is_writed) {
                if (texture->description().destination_layout ==
                    render::TextureLayout::Shader_Read_Only_Optimal)
                    continue;

                transitions.emplace(core::makeConstObserver(texture),
                                    render::TextureLayout::Shader_Read_Only_Optimal);
            } else {
                if (texture->description().destination_layout ==
                    render::TextureLayout::Color_Attachment_Optimal)
                    continue;

                transitions.emplace(core::makeConstObserver(texture),
                                    render::TextureLayout::Color_Attachment_Optimal);
            }
        }

        for (const auto &texture : pass->writeTextures()) {
            if (texture->description().destination_layout ==
                render::TextureLayout::Color_Attachment_Optimal)
                continue;

            transitions.emplace(core::makeConstObserver(texture),
                                render::TextureLayout::Color_Attachment_Optimal);
        }

        const auto step =
            Step { .resources    = core::makeObserver(resources),
                   .transitions  = std::move(transitions),
                   .frame_pass   = core::makeObserver(pass),
                   .debug_marker = fmt::format("Begin render pass - {}", pass->name()) };

        m_timeline.emplace_back(std::move(step));
    }
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::execute(storm::render::CommandBuffer &cmb) {
    auto &profiler = m_engine->profiler();

    for (auto &step : m_timeline) {
        profiler.beginStage(std::string { step.frame_pass->name() }, 2);
        step.resources->realize();

        const auto &writes  = step.frame_pass->writeTextures();
        const auto &creates = step.frame_pass->createTextures();

        auto clear_values = std::vector<render::ClearValue> {};
        clear_values.reserve(std::size(writes) + std::size(creates));

        for (auto &texture : writes) {
            if (render::isDepthFormat(texture->description().format))
                clear_values.emplace_back(render::ClearDepthStencil {});
            else
                clear_values.emplace_back(render::ClearColor {});
        }
        for (auto &texture : creates) {
            if (render::isDepthFormat(texture->description().format))
                clear_values.emplace_back(render::ClearDepthStencil {});
            else
                clear_values.emplace_back(render::ClearColor {});
        }

        for (const auto &[resource, layout] : step.transitions) {
            cmb.transitionTextureLayout(getTexture(*resource),
                                        resource->description().destination_layout,
                                        layout);
        }

        cmb.beginDebugRegion(step.debug_marker);

        cmb.beginRenderPass(step.resources->renderpass(),
                            step.resources->framebuffer(),
                            std::move(clear_values),
                            step.frame_pass->secondaryCommandBuffer());

        step.frame_pass->execute(*step.resources, cmb);

        cmb.endRenderPass();
        cmb.endDebugRegion();
        profiler.endStage(step.frame_pass->name());
    }
}

////////////////////////////////////////
////////////////////////////////////////
FramePassTextureResource &FrameGraph::addTransientTexture(std::string name,
                                                          FramePassTextureDescription description) {
    return *m_transient_textures.emplace_back(
        std::make_unique<FramePassTextureResource>(std::move(name), std::move(description)));
}

////////////////////////////////////////
////////////////////////////////////////
FramePassBufferResource &FrameGraph::addTransientBuffer(std::string name,
                                                        FramePassBufferDescription description) {
    return *m_transient_buffers.emplace_back(
        std::make_unique<FramePassBufferResource>(std::move(name), std::move(description)));
}

////////////////////////////////////////
////////////////////////////////////////
const render::Texture &FrameGraph::getTexture(const FramePassTextureResource &resource) const
    noexcept {
    if (!resource.isTransient()) {
        auto it =
            std::find_if(std::cbegin(m_retained_textures),
                         std::cend(m_retained_textures),
                         [&resource](const auto &pair) { return pair.first == resource.name(); });
        STORM_EXPECTS(it != std::cend(m_retained_textures));

        return *(it->second.second);
    }

    auto it = m_textures.find(resource.name());
    STORM_EXPECTS(it != std::cend(m_textures));

    return it->second;
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::createTexture(const FramePassTextureResource &resource) {
    const auto &device = m_engine->device();

    auto name               = resource.name();
    const auto &description = resource.description();

    const auto usage = render::TextureUsage::Input_Attachment | render::TextureUsage::Sampled |
                       ((render::isDepthFormat(description.format))
                            ? render::TextureUsage::Depth_Stencil_Attachment
                            : render::TextureUsage::Color_Attachment);

    auto &texture = m_textures.emplace(name, device.createTexture(description.type)).first->second;
    texture.createTextureData(description.extent,
                              description.format,
                              description.sample_count,
                              1,
                              1,
                              usage);

    device.setObjectName(texture, resource.name());
}

////////////////////////////////////////
////////////////////////////////////////
const render::HardwareBuffer &FrameGraph::getBuffer(const FramePassTextureResource &resource) const
    noexcept {
    if (!resource.isTransient()) {
        auto it =
            std::find_if(std::cbegin(m_retained_buffers),
                         std::cend(m_retained_buffers),
                         [&resource](const auto &pair) { return pair.first == resource.name(); });
        STORM_EXPECTS(it != std::cend(m_retained_buffers));

        return *(it->second.second);
    }

    auto it = m_buffers.find(resource.name());
    STORM_EXPECTS(it != std::cend(m_buffers));

    return it->second;
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::createBuffer(const FramePassBufferResource &resource) {
    const auto &device = m_engine->device();

    auto name              = resource.name();
    const auto description = resource.description();

    auto &buffer = m_buffers
                       .emplace(name,
                                device.createHardwareBuffer(render::HardwareBufferUsage::Storage,
                                                            description.size,
                                                            render::MemoryProperty::Device_Local))
                       .first->second;

    device.setObjectName(buffer, resource.name());
}

////////////////////////////////////////
////////////////////////////////////////
void FrameGraph::cullPasses(FramePassBaseObserverPtrArray &passes) {
    passes.erase(std::remove_if(std::begin(passes),
                                std::end(passes),
                                [](const auto &pass) {
                                    if (pass->isCullImune()) return false;

                                    const auto created_textures = pass->createTextures();
                                    const auto created_buffers  = pass->createBuffers();

                                    const auto writed_textures = pass->writeTextures();
                                    const auto writed_buffers  = pass->writeBuffers();

                                    if (std::empty(created_textures) &&
                                        std::empty(created_buffers) &&
                                        std::empty(writed_textures) && std::empty(writed_buffers))
                                        return true;

                                    const auto is_used = [&pass](const auto &resource) {
                                        const auto writed_in = resource->writeIn();
                                        const auto readed_in = resource->readIn();

                                        auto is_used = false;
                                        for (const auto &write_pass : writed_in) {
                                            if (write_pass != pass) is_used = true;
                                        }

                                        for (const auto &read_pass : readed_in) {
                                            if (read_pass != pass) is_used = true;
                                        }

                                        return is_used;
                                    };

                                    const auto to_be_cull =
                                        (std::any_of(std::cbegin(created_textures),
                                                     std::cend(created_textures),
                                                     is_used) ||
                                         std::any_of(std::cbegin(created_buffers),
                                                     std::cend(created_buffers),
                                                     is_used) ||
                                         std::any_of(std::cbegin(writed_textures),
                                                     std::cend(writed_textures),
                                                     is_used) ||
                                         std::any_of(std::cbegin(writed_buffers),
                                                     std::cend(writed_buffers),
                                                     is_used)) == false;

                                    return to_be_cull;
                                }),
                 std::end(passes));
}

////////////////////////////////////////
////////////////////////////////////////
FrameGraph::FramePassMergedArray FrameGraph::mergePasses(FramePassBaseObserverPtrSpan passes) {
    if (std::empty(passes)) return {};

    auto passes_merged = FramePassMergedArray {};
    passes_merged.reserve(std::size(passes));

    auto passes_list = std::list<FramePassBaseObserverPtr> {};
    std::copy(std::begin(passes),
              std::end(passes),
              std::insert_iterator { passes_list, std::end(passes_list) });

    while (!std::empty(passes_list)) {
        auto it   = std::begin(passes_list);
        auto pass = std::move(*it);
        passes_list.erase(it);

        auto pass_merged = FramePassMerged {};
        pass_merged.pass = pass;

        passes_merged.emplace_back(std::move(pass_merged));

        passes_list.erase(std::remove_if(std::begin(passes_list),
                                         std::end(passes_list),
                                         [&pass, &passes_merged, this](auto &possible_child) {
                                             const auto is_compatible =
                                                 checkIfCompatible(*pass, *possible_child);

                                             if (is_compatible) {
                                                 auto pass_merged   = FramePassMerged {};
                                                 pass_merged.pass   = std::move(possible_child);
                                                 pass_merged.parent = pass;

                                                 passes_merged.emplace_back(std::move(pass_merged));

                                                 return true;
                                             }
                                             return false;
                                         }),
                          std::end(passes_list));
    }

    return passes_merged;
}

bool FrameGraph::checkIfCompatible(const FramePassBase &first, const FramePassBase &second) const
    noexcept {
    return false;
}
