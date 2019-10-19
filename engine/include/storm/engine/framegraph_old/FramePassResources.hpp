// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <string_view>
#include <unordered_map>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/pipeline/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassResources: public core::NonCopyable {
      public:
        FramePassResources(const engine::Engine &engine,
                           FrameGraph &frame_graph,
                           const FramePassBase &pass);
        ~FramePassResources();

        FramePassResources(FramePassResources &&);
        FramePassResources &operator=(FramePassResources &&);

        void realize();
        void derealize();

        inline render::TextureViewConstSpan textureViews() const noexcept;
        inline const render::RenderPass &renderpass() const noexcept;
        inline const render::Framebuffer &framebuffer() const noexcept;
        inline const render::DescriptorSetLayout &descriptorSetLayout() const noexcept;
        inline const render::DescriptorSet &descriptorSet() const noexcept;

      private:
        render::AttachmentLoadOperation detectLoadOperation();

        void createTextureViews();
        void createRenderPass();
        void createFramebuffer();
        void createDescriptorSet();

        engine::EngineConstObserverPtr m_engine;

        FrameGraphObserverPtr m_frame_graph;
        FramePassBaseConstObserverPtr m_frame_pass;

        FramePassTextureResourceConstObserverPtrArray m_realize_textures;
        FramePassBufferResourceConstObserverPtrArray m_realize_buffers;

        FramePassTextureResourceConstObserverPtrArray m_derealize_textures;
        FramePassBufferResourceConstObserverPtrArray m_derealize_buffers;

        render::SamplerOwnedPtr m_sampler;
        render::TextureViewArray m_texture_views;
        render::TextureViewArray m_texture_views2;

        core::Extentu m_extent;
        render::RenderPassOwnedPtr m_render_pass;
        render::FramebufferOwnedPtr m_framebuffer;

        storm::render::DescriptorSetLayoutOwnedPtr m_descriptor_layout;
        storm::render::DescriptorSetOwnedPtr m_descriptor_set;

        bool m_realized = false;

        friend class FrameGraph;
    };
} // namespace storm::engine

#include "FramePassResources.inl"
