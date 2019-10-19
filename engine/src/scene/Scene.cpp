/////////// - StormKit::render - ///////////
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Engine.hpp>

#include <storm/engine/scene/Camera.hpp>
#include <storm/engine/scene/Scene.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
Scene::Scene(const Engine &engine) : m_engine { &engine } {
    m_default_camera =
        std::make_unique<Camera>(engine,
                                 Camera::Type::Perspective,
                                 m_engine->surface().extent().convertTo<core::Extentf>());
    m_camera = core::makeObserver(m_default_camera);

    const auto &layout = m_engine->pipelineBuilder().cameraLayout();
    const auto &pool   = m_engine->descriptorPool();

    const auto descriptors = std::array { render::Descriptor { m_camera->cameraDescriptor() } };

    m_camera_descriptor_set = pool.allocateDescriptorSetPtr(layout);
    m_camera_descriptor_set->update(descriptors);
}

////////////////////////////////////////
////////////////////////////////////////
Scene::~Scene() = default;

////////////////////////////////////////
////////////////////////////////////////
Scene::Scene(Scene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
Scene &Scene::operator=(Scene &&) = default;

////////////////////////////////////////
////////////////////////////////////////
void Scene::update(float delta) noexcept {
    m_camera->update(delta);

    m_entities.step(delta);
}
