// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    class STORM_PUBLIC RenderPass: public core::NonCopyable {
      public:
        struct AttachmentDescription {
            PixelFormat format;
            SampleCountFlag samples = SampleCountFlag::C1_BIT;

            AttachmentLoadOperation load_op   = AttachmentLoadOperation::Clear;
            AttachmentStoreOperation store_op = AttachmentStoreOperation::Store;

            AttachmentLoadOperation stencil_load_op   = AttachmentLoadOperation::Dont_Care;
            AttachmentStoreOperation stencil_store_op = AttachmentStoreOperation::Dont_Care;

            TextureLayout source_layout      = TextureLayout::Undefined;
            TextureLayout destination_layout = TextureLayout::Present_Src;

            bool resolve = false;
        };

        struct Subpass {
            struct Ref {
                core::UInt32 attachment_id;

                TextureLayout layout = TextureLayout::Color_Attachment_Optimal;
            };

            PipelineBindPoint bind_point;
            std::vector<Ref> attachment_refs;
        };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Render_Pass;

        explicit RenderPass(const Device &device);
        ~RenderPass();

        RenderPass(RenderPass &&);
        RenderPass &operator=(RenderPass &&);

        core::UInt32 addAttachmentDescription(AttachmentDescription attachment);
        std::vector<core::UInt32>
            addAttachmentDescriptions(core::span<AttachmentDescription> attachment);
        inline void addSubpass(Subpass subpass);

        void build();
        Framebuffer createFramebuffer(core::Extentu extent,
                                      TextureViewConstObserverPtrArray attachments) const;
        FramebufferOwnedPtr
            createFramebufferPtr(core::Extentu extent,
                                 TextureViewConstObserverPtrArray attachments) const;

        inline const Device &device() const noexcept;

        inline core::span<const AttachmentDescription> attachmentDescriptions() const noexcept;
        inline core::span<const Subpass> subpasses() const noexcept;

        inline vk::RenderPass vkRenderPass() const noexcept;
        inline operator vk::RenderPass() const noexcept;
        inline vk::RenderPass vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstObserverPtr m_device;

        std::vector<AttachmentDescription> m_attachment_descriptions;
        std::vector<Subpass> m_subpasses;

        RAIIVkRenderPass m_vk_render_pass;
    };
} // namespace storm::render

#include "RenderPass.inl"
