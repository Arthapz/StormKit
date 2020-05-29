/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>

#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/PBRScene.hpp>

#include <storm/engine/drawable/CubeMap.hpp>

#include <storm/engine/material/Material.hpp>
#include <storm/engine/material/MaterialInstance.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
CubeMap::CubeMap(Scene &scene) : Drawable { scene.engine() }, m_scene { &scene } {
    auto &material = m_scene->materialPool().get(PBRScene::CUBEMAP_MATERIAL_NAME);

    m_material_instance = material->createInstancePtr();
}

////////////////////////////////////////
////////////////////////////////////////
CubeMap::~CubeMap() = default;

////////////////////////////////////////
////////////////////////////////////////
CubeMap::CubeMap(CubeMap &&) = default;

////////////////////////////////////////
////////////////////////////////////////
CubeMap &CubeMap::operator=(CubeMap &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void CubeMap::render(render::CommandBuffer &cmb,
                     const render::RenderPass &pass,
                     std::vector<BindableBaseConstObserverPtr> bindables,
                     render::GraphicsPipelineState state) {
    m_material_instance->flush();

    bindables.emplace_back(core::makeConstObserver(m_material_instance));

    state.shader_state                         = m_material_instance->parent().m_data.shader_state;
    state.rasterization_state                  = m_material_instance->m_rasterization_state;
    state.depth_stencil_state.depth_compare_op = render::CompareOperation::Less_Or_Equal;

    auto descriptors = std::vector<render::DescriptorSetCRef> {};
    auto offsets     = std::vector<core::UOffset> {};

    for (const auto &bindable : bindables) {
        descriptors.push_back(bindable->descriptorSet());

        const auto &offset = bindable->offset();
        if (offset.has_value()) offsets.push_back(offset.value());

        state.layout.descriptor_set_layouts.emplace_back(
            core::makeConstObserver(bindable->descriptorLayout()));
    }

    const auto &pipeline = m_engine->pipelineCache().getPipeline(state, pass);

    cmb.bindGraphicsPipeline(pipeline);
    cmb.bindDescriptorSets(pipeline, std::move(descriptors), std::move(offsets));

    cmb.draw(36);
}

////////////////////////////////////////
////////////////////////////////////////
void CubeMap::recomputeBoundingBox() const noexcept {
}
