/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/material/CubeMapMaterial.hpp>
#include <storm/engine/material/CubeMapMaterialInstance.hpp>

#include <storm/engine/scene/PBRScene.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterialInstance::CubeMapMaterialInstance(const Scene &scene,
                                                 const CubeMapMaterial &material)
    : MaterialInstance { scene, material } {
    const auto &texture_pool = scene.texturePool();
    const auto &texture      = texture_pool.get(CubeMapMaterial::DEFAULT_CUBE_MAP_TEXTURE);

    setCubeMap(texture);
    setFrontFace(render::FrontFace::Counter_Clockwise);

    flush();
}

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterialInstance::~CubeMapMaterialInstance() = default;

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterialInstance::CubeMapMaterialInstance(CubeMapMaterialInstance &&) = default;

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterialInstance &CubeMapMaterialInstance::operator=(CubeMapMaterialInstance &&) = default;
