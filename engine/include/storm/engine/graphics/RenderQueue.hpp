// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/mesh/StaticMesh.hpp>

namespace storm::engine {
    class STORM_PUBLIC RenderQueue {
      public:
        static constexpr auto MESH_COUNT = 100; // TODO change this to dynamic sizing

        RenderQueue(const render::Device &device, std::string name);
        ~RenderQueue();

        RenderQueue(RenderQueue &&);
        RenderQueue &operator=(RenderQueue &&);

        void clear();
        void addMesh(const StaticMesh &mesh);

        void buildCommandBuffer(std::vector<render::DescriptorSetCRef> descriptor_sets,
                                std::vector<core::UOffset> offsets);

        inline render::CommandBuffer &commandBuffer() noexcept;

      private:
        render::DeviceConstObserverPtr m_device;

        std::vector<StaticMeshConstObserverPtr> m_meshes;
        std::vector<render::SemaphoreConstObserverPtr> m_semaphores;

        render::CommandBufferOwnedPtr m_cmb;

        std::string m_debug_marker;
    };
} // namespace storm::engine

#include "RenderQueue.inl"
