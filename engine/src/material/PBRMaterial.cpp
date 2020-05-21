// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::engine - ///////////
#include <storm/engine/material/PBRMaterial.hpp>

#include <storm/engine/material/PBRMaterialInstance.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Scene.hpp>

using namespace storm;
using namespace storm::engine;

static const auto COLOR_PASS_VERT_SHADER_DATA = std::vector<core::UInt32> {
#include "PBRColorPass.vert.spv.hpp"
};
static const auto COLOR_PASS_FRAG_SHADER_DATA = std::vector<core::UInt32> {
#include "PBRColorPass.frag.spv.hpp"
};

PBRMaterial::PBRMaterial(Scene &scene) : Material { scene } {
    buildShaders();

    const auto &shader_pool     = m_scene->shaderPool();
    const auto &vertex_shader   = shader_pool.get(PBR_COLOR_PASS_VERT_SHADER_NAME);
    const auto &fragment_shader = shader_pool.get(PBR_COLOR_PASS_FRAG_SHADER_NAME);

    addShader(vertex_shader);
    addShader(fragment_shader);

    addSampler(0, std::string { PBRMaterialInstance::ALBEDO_MAP_NAME });
    addSampler(1, std::string { PBRMaterialInstance::NORMAL_MAP_NAME });
    addSampler(2, std::string { PBRMaterialInstance::METALLIC_MAP_NAME });
    addSampler(3, std::string { PBRMaterialInstance::ROUGHNESS_MAP_NAME });
    addSampler(4, std::string { PBRMaterialInstance::AMBIANT_OCCLUSION_MAP_NAME });
    addSampler(5, std::string { PBRMaterialInstance::EMISSIVE_MAP_NAME });

    addUniform(std::string { PBRMaterialInstance::ALBEDO_FACTOR_NAME }, UniformType::Vec4f);
    addUniform(std::string { PBRMaterialInstance::METALLIC_FACTOR_NAME }, UniformType::Float);
    addUniform(std::string { PBRMaterialInstance::ROUGHNESS_FACTOR_NAME }, UniformType::Float);
    addUniform(std::string { PBRMaterialInstance::AMBIANT_OCCLUSION_FACTOR_NAME },
               UniformType::Float);
    addUniform("PAD0", UniformType::UInt32);
    addUniform(std::string { PBRMaterialInstance::EMISSIVE_FACTOR_NAME }, UniformType::Vec4f);
    // addUniform("double_sided", UniformType::Int8);

    finalize();
}
////////////////////////////////////////
////////////////////////////////////////
PBRMaterial::~PBRMaterial() = default;

////////////////////////////////////////
////////////////////////////////////////
PBRMaterial::PBRMaterial(PBRMaterial &&) = default;

////////////////////////////////////////
////////////////////////////////////////
PBRMaterial &PBRMaterial::operator=(PBRMaterial &&) = default;

////////////////////////////////////////
////////////////////////////////////////
MaterialInstanceOwnedPtr PBRMaterial::createInstancePtr() const noexcept {
    return std::make_unique<PBRMaterialInstance>(*m_scene, *this);
}

////////////////////////////////////////
////////////////////////////////////////
void PBRMaterial::buildShaders() {
    auto &shader_pool = m_scene->shaderPool();
    if (!shader_pool.has(PBR_COLOR_PASS_VERT_SHADER_NAME)) {
        shader_pool.create(PBR_COLOR_PASS_VERT_SHADER_NAME,
                           COLOR_PASS_VERT_SHADER_DATA,
                           render::ShaderStage::Vertex,
                           m_scene->engine().device());
    }

    if (!shader_pool.has(PBR_COLOR_PASS_FRAG_SHADER_NAME)) {
        shader_pool.create(PBR_COLOR_PASS_FRAG_SHADER_NAME,
                           COLOR_PASS_FRAG_SHADER_DATA,
                           render::ShaderStage::Fragment,
                           m_scene->engine().device());
    }
}
