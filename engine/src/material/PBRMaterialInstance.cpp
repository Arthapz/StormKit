/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Ranges.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/material/PBRMaterial.hpp>
#include <storm/engine/material/PBRMaterialInstance.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
PBRMaterialInstance::PBRMaterialInstance(const Scene &scene, const PBRMaterial &material)
    : MaterialInstance { scene, material } {
    setAlbedoFactor({ 1.f, 1.f, 1.f, 1.f });
    setMetallicFactor(1.f);
    setRoughnessFactor(1.f);
    setAmbiantOcclusionFactor(1.f);
    setEmissiveFactor({ 0.f, 0.f, 0.f, 0.f });

    flush();
}

////////////////////////////////////////
////////////////////////////////////////
PBRMaterialInstance::~PBRMaterialInstance() = default;

////////////////////////////////////////
////////////////////////////////////////
PBRMaterialInstance::PBRMaterialInstance(PBRMaterialInstance &&) = default;

////////////////////////////////////////
////////////////////////////////////////
PBRMaterialInstance &PBRMaterialInstance::operator=(PBRMaterialInstance &&) = default;
