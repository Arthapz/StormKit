/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/material/CubeMapMaterial.hpp>
#include <storm/engine/material/CubeMapMaterialInstance.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
CubeMapMaterialInstance::CubeMapMaterialInstance(const Scene &scene,
                                                 const CubeMapMaterial &material)
    : MaterialInstance { scene, material } {
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
