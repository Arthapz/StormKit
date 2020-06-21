// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/RingBuffer.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Command.hpp>

#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/framegraph/FramePass.hpp>
#include <storm/engine/framegraph/FramePassBuilder.hpp>
#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/FramePassResourceHandle.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FrameGraph: public core::NonCopyable {
      public:
        explicit FrameGraph(Engine &engine) noexcept;
        ~FrameGraph();

        FrameGraph(FrameGraph &&);
        FrameGraph &operator=(FrameGraph &&);

        template<typename PassData, typename SetupCallback, typename ExecuteCallback>
        FramePass<PassData> &
            addPass(std::string name,
                    SetupCallback &&setup_callback,
                    ExecuteCallback &&execute_callback,
                    FramePassBase::PassType type = FramePassBase::PassType::Graphics);

        FramePassTextureID presentTo(std::string name,
                                     render::Texture &texture,
                                     const render::Semaphore &available,
                                     const render::Semaphore &render_finished,
                                     render::Fence &finished);
        template<typename Resource>
        FramePassResourceID<Resource> setRetainedResource(std::string name,
                                                          typename Resource::Descriptor descriptor,
                                                          Resource &&resource);

        void reset();
        void compile() noexcept;
        void execute();

        std::string exportGraphviz();

        template<typename Resource>
        [[nodiscard]] const Resource &get(FramePassResourceID<Resource> id) const noexcept;

        template<typename Resource>
        [[nodiscard]] const FramePassResource<Resource> &
            getResource(FramePassResourceID<Resource> id) const noexcept;

      private:
        friend class FramePassBuilder;
        friend class FramePassResources;

        struct Step {
            std::vector<core::UInt32> pass_id;

            std::string pass_name;

            std::vector<FramePassResourceHandle> realize_resource;
            std::vector<FramePassResourceHandle> derealize_resource;

            std::function<void(render::CommandBuffer &)> pre_execute;
            std::function<void(render::CommandBuffer &)> post_execute;

            render::FenceOwnedPtr fence;
            render::CommandBufferOwnedPtr cmb;

            core::Extentu framebuffer_extent;
            std::vector<FramePassResourceHandle> framebuffer_attachments;

            render::BeginRenderPassCommand::ClearValues clear_values;
            render::RenderPassOwnedPtr render_pass;
            render::FramebufferOwnedPtr frame_buffer;

            bool use_sub_command_buffers = false;
        };

        void cullResources();
        void computeTimeline(core::span<const core::UInt32> passes);
        void prepareGPUObjects();

        render::RenderPass::AttachmentDescription
            createDescription(const FramePassTexture &resource,
                              render::AttachmentLoadOperation load_op,
                              render::AttachmentStoreOperation store_op,
                              render::TextureLayout source_layout,
                              render::TextureLayout destination_layout,
                              bool resolve);
        render::RenderPass::AttachmentDescription
            createReadOnlyDescription(const FramePassTexture &resource);
        render::RenderPass::AttachmentDescription
            createReadWriteDescription(const FramePassTexture &resource, core::UInt32 pass_id);
        render::RenderPass::AttachmentDescription
            createWriteOnlyDescription(const FramePassTexture &resource,
                                       core::UInt32 pass_id,
                                       bool resolve);

        template<typename Resource>
        [[nodiscard]] FramePassResourceID<Resource>
            createTransientResource(std::string name, typename Resource::Descriptor descriptor);

        [[nodiscard]] FramePassResourceHandle create(FramePassResourceBaseOwnedPtr resource);

        template<typename Resource>
        [[nodiscard]] FramePassResource<Resource> &
            getResource(FramePassResourceID<Resource> id) noexcept;
        [[nodiscard]] FramePassResourceBase &
            getResourceBase(FramePassResourceHandle handle) noexcept;
        [[nodiscard]] const FramePassResourceBase &
            getResourceBase(FramePassResourceHandle handle) const noexcept;

        engine::EngineObserverPtr m_engine;

        FramePassBaseOwnedPtrArray m_frame_passes;
        FramePassResourceBaseOwnedPtrArray m_resources;

        FramePassTextureID m_present_resource;
        render::SemaphoreConstObserverPtrArray m_wait_semaphores;
        render::SemaphoreConstObserverPtrArray m_signal_semaphores;
        render::FenceObserverPtr m_present_fence;

        std::vector<Step> m_timeline;

        std::queue<_std::observer_ptr<Step>> m_derealize_queue;

        core::UInt32 m_resource_id = 0u;
        core::UInt32 m_pass_id     = 0u;

        render::TextureViewOwnedPtrArray m_texture_views;

        friend class FramePassBuilder;
    };

    using FrameGraphRingBuffer = core::RingBuffer<FrameGraph>;
} // namespace storm::engine

#include "FrameGraph.inl"
#include "FramePassBuilder.inl"
