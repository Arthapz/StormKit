// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "StormKitTextureNode.hpp"

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/core/Surface.hpp>

#include <storm/render/sync/Fence.hpp>

#include <storm/engine/Engine.hpp>

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>
#include <QtQuick/QSGTextureProvider>

#include <QVulkanFunctions>
#include <QVulkanInstance>

using namespace storm;

StormKitTextureNode::StormKitTextureNode(QQuickItem *item, engine::Engine &engine)
    : m_window { item->window() }, m_engine { &engine } {
    connect(m_window, &QQuickWindow::beforeRendering, this, &StormKitTextureNode::render);
}

void StormKitTextureNode::sync() {
    const auto need_new_texture = texture() == nullptr;

    if (need_new_texture) {
        auto current = m_window->graphicsStateInfo().currentFrameSlot;

        auto tex = m_engine->surface().textures()[current].vkHandle();

        auto *wrapper =
            QNativeInterface::QSGVulkanTexture::fromNative(tex,
                                                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                           m_window,
                                                           m_size);
        setTexture(wrapper);
    }
}

void StormKitTextureNode::render() {
    auto &frame = m_engine->beginFrame();
    m_engine->endFrame();

    auto &texture = m_engine->surface().textures()[frame.current_frame];

    auto cmb = m_engine->device().graphicsQueue().createCommandBuffer();
    cmb.begin(true);
    cmb.transitionTextureLayout(texture,
                                render::TextureLayout::Present_Src,
                                render::TextureLayout::Shader_Read_Only_Optimal);
    cmb.end();
    cmb.build();

    auto fence = m_engine->device().createFence();
    cmb.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

StormKitTextureNode::~StormKitTextureNode() = default;
