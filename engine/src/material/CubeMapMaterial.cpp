// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::device - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/sync/Fence.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/material/CubeMapMaterial.hpp>

#include <storm/engine/material/CubeMapMaterialInstance.hpp>

#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/PBRScene.hpp>

using namespace storm;
using namespace storm::engine;

static const auto CUBE_MAP_VERT_SHADER_DATA = std::vector<core::UInt32> {
#include "CubeMap.vert.spv.hpp"
};
static const auto CUBE_MAP_FRAG_SHADER_DATA = std::vector<core::UInt32> {
#include "CubeMap.frag.spv.hpp"
};

CubeMapMaterial::CubeMapMaterial(Scene &scene) : Material { scene } {
    buildShaders();

    auto &texture_pool = m_scene->texturePool();

    const auto &device          = m_scene->engine().device();
    const auto &shader_pool     = m_scene->shaderPool();
    const auto &vertex_shader   = shader_pool.get(CUBE_MAP_VERT_SHADER_NAME);
    const auto &fragment_shader = shader_pool.get(CUBE_MAP_FRAG_SHADER_NAME);

    addShader(vertex_shader);
    addShader(fragment_shader);

    addSampler(0,
               std::string { CubeMapMaterialInstance::CUBE_MAP_NAME },
               render::TextureViewType::Cube);

    finalize();

    if (!texture_pool.has(DEFAULT_CUBE_MAP_TEXTURE)) {
        constexpr auto silver  = core::RGBColorDef::Silver<float>.toVector4();
        constexpr auto silvers = std::array { silver, silver, silver, silver, silver, silver };

        static constexpr auto EXTENT = core::Extentu { .width = 1u, .height = 1u, .depth = 1u };

        auto &texture = texture_pool.create(DEFAULT_CUBE_MAP_TEXTURE,
                                            device,
                                            EXTENT,
                                            render::PixelFormat::RGBA8_UNorm,
                                            1u,
                                            1u,
                                            render::TextureType::T2D,
                                            render::TextureCreateFlag::Cube_Compatible);
        texture.loadFromMemory(core::toConstSpan<core::Byte>(silvers), 1u, 6u, 1u);

        device.setObjectName(texture, "StormKit:DefaultCubeMapMaterial:DefaultTexture");
    }
}
////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterial::~CubeMapMaterial() = default;

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterial::CubeMapMaterial(CubeMapMaterial &&) = default;

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterial &CubeMapMaterial::operator=(CubeMapMaterial &&) = default;

////////////////////////////////////////
////////////////////////////////////////
MaterialInstanceOwnedPtr CubeMapMaterial::createInstancePtr() const noexcept {
    return std::make_unique<CubeMapMaterialInstance>(*m_scene, *this);
}

////////////////////////////////////////
////////////////////////////////////////
void CubeMapMaterial::buildShaders() {
    auto &shader_pool = m_scene->shaderPool();
    if (!shader_pool.has(CUBE_MAP_VERT_SHADER_NAME)) {
        shader_pool.create(CUBE_MAP_VERT_SHADER_NAME,
                           CUBE_MAP_VERT_SHADER_DATA,
                           render::ShaderStage::Vertex,
                           m_scene->engine().device());
    }

    if (!shader_pool.has(CUBE_MAP_FRAG_SHADER_NAME)) {
        shader_pool.create(CUBE_MAP_FRAG_SHADER_NAME,
                           CUBE_MAP_FRAG_SHADER_DATA,
                           render::ShaderStage::Fragment,
                           m_scene->engine().device());
    }
}
