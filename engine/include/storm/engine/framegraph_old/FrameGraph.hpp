// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/RingBuffer.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Command.hpp>

#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/framegraph/FramePassBuilder.hpp>
#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FrameGraph: public core::NonCopyable {
      public:
        explicit FrameGraph(Engine &engine) noexcept;
        ~FrameGraph();

        FrameGraph(FrameGraph &&);
        FrameGraph &operator=(FrameGraph &&);

        template<typename PassData>
        FramePass<PassData> &
            addPass(std::string name,
                    typename FramePass<PassData>::SetupCallback &&setup_callback,
                    typename FramePass<PassData>::ExecuteCallback &&execute_callback,
                    bool secondary_command_buffers = false);

        FramePassTextureResource &setRetainedTexture(std::string name,
                                                     const render::Texture &texture,
                                                     FramePassTextureDescription description);
        FramePassBufferResource &setRetainedTexture(std::string name,
                                                    const render::HardwareBuffer &buffer,
                                                    FramePassBufferDescription description);

        const render::Texture &getTexture(const FramePassTextureResource &resource) const noexcept;
        const render::HardwareBuffer &getBuffer(const FramePassTextureResource &resource) const
            noexcept;

        void reset();
        void bake();
        void execute(render::CommandBuffer &cmb);

      protected:
        FramePassTextureResource &addTransientTexture(std::string name,
                                                      FramePassTextureDescription description);
        FramePassBufferResource &addTransientBuffer(std::string name,
                                                    FramePassBufferDescription description);

        void createTexture(const FramePassTextureResource &resource);
        void createBuffer(const FramePassBufferResource &resource);

        friend class FramePassBuilder;
        friend class FramePassResources;

      private:
        struct Step {
            FramePassResourcesObserverPtr resources;

            std::unordered_map<FramePassTextureResourceConstObserverPtr, render::TextureLayout>
                transitions;

            FramePassBaseObserverPtr frame_pass;

            std::string debug_marker;
        };

        struct FramePassMerged {
            FramePassBaseObserverPtr pass;
            FramePassBaseObserverPtr parent = nullptr;
        };

        using FramePassMergedArray = std::vector<FramePassMerged>;
        using FramePassMergedSpan  = core::span<FramePassMerged>;

        void cullPasses(FramePassBaseObserverPtrArray &passes);
        FramePassMergedArray mergePasses(FramePassBaseObserverPtrSpan passes);
        void buildTimeline(FramePassMergedSpan passes);

        bool checkIfCompatible(const FramePassBase &first, const FramePassBase &second) const
            noexcept;

        engine::EngineObserverPtr m_engine;

        FramePassBaseOwnedPtrArray m_frame_passes;

        std::unordered_map<
            std::string,
            std::pair<FramePassTextureResourceOwnedPtr, render::TextureConstObserverPtr>>
            m_retained_textures;
        std::unordered_map<
            std::string,
            std::pair<FramePassBufferResourceOwnedPtr, render::HardwareBufferConstObserverPtr>>
            m_retained_buffers;

        FramePassTextureResourceOwnedPtrArray m_transient_textures;
        FramePassBufferResourceOwnedPtrArray m_transient_buffers;

        std::vector<Step> m_timeline;
        FramePassResourcesOwnedPtrArray m_step_resources;

        std::unordered_map<std::string_view, render::Texture> m_textures;
        std::unordered_map<std::string_view, render::HardwareBuffer> m_buffers;
    };

    using FrameGraphRingBuffer = core::RingBuffer<FrameGraph>;
} // namespace storm::engine

#include "FrameGraph.inl"
