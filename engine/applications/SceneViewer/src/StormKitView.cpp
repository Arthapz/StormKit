// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "StormKitView.hpp"
#include "StormKitTextureNode.hpp"

#include <QQuickWindow>
#include <QRunnable>

#include <storm/engine/Engine.hpp>

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QSGSimpleTextureNode>
#include <QVulkanInstance>

using namespace storm;

constexpr auto toBufferingCount(int in_flight) noexcept {
    switch (in_flight) {
        case 1: return render::Surface::Buffering::Simple;
        case 2: return render::Surface::Buffering::Double;
        default: return render::Surface::Buffering::Triple;
    }

    return render::Surface::Buffering::Triple;
}

StormKitView::StormKitView(QQuickItem *parent) : QQuickItem { parent }, m_model{this} {
    log::LogHandler::setupDefaultLogger();
    setFlag(ItemHasContents, true);
}

QSGNode *StormKitView::updatePaintNode(QSGNode *_node, [[maybe_unused]] UpdatePaintNodeData *data) {
    if(!m_initialized)
        initialize();

    auto node = static_cast<StormKitTextureNode *>(_node);

    if(!node && (width() <= 0 || height() <= 0)) return nullptr;

    if(!node) {
        m_texture_node = new StormKitTextureNode{this, *m_engine};
        node = m_texture_node;
    }

    m_texture_node->sync();

    node->setTextureCoordinatesTransform(QSGSimpleTextureNode::NoTransform);
    node->setFiltering(QSGTexture::Linear);
    node->setRect(0, 0, width(), height());

    window()->update();

    return node;
}

void StormKitView::geometryChange(const QRectF &new_geometry, const QRectF &old_geometry) {
    QQuickItem::geometryChange(new_geometry, old_geometry);

    if(new_geometry.size() != old_geometry.size())
        update();
}

void StormKitView::invalidateSceneGraph() {
    m_texture_node = nullptr;
}

void StormKitView::initialize() {
    auto window = QQuickItem::window();

    auto rif            = window->rendererInterface();
    auto qt_vk_instance = reinterpret_cast<QVulkanInstance *>(
        rif->getResource(window, QSGRendererInterface::VulkanInstanceResource));

    auto vk_instance        = qt_vk_instance->vkInstance();
    auto vk_physical_device = *reinterpret_cast<VkPhysicalDevice *>(
        rif->getResource(window, QSGRendererInterface::PhysicalDeviceResource));
    auto vk_device = *reinterpret_cast<VkDevice *>(
        rif->getResource(window, QSGRendererInterface::DeviceResource));

    const auto frames_in_flight = window->graphicsStateInfo().framesInFlight;
    m_engine = std::make_unique<engine::Engine>(vk_instance,
                                                vk_physical_device,
                                                vk_device,
                                                core::Extentu {
                                                    static_cast<core::UInt32>(size().width()),
                                                    static_cast<core::UInt32>(size().height()) },
                                                toBufferingCount(frames_in_flight));
    m_scene  = std::make_unique<StormKitScene>(*m_engine);
    m_model.setWorld(m_scene->world());

    connect(m_scene.get(), &StormKitScene::newEntity, &m_model, &EntityModel::addEntity);

    m_engine->setScene(*m_scene);

    m_initialized = true;
    emit initialized();
}

void StormKitView::releaseResources() {
    m_texture_node = nullptr;
}
