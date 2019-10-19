// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
	class STORM_PUBLIC RenderPass : public core::NonCopyable {
	  public:
		struct Attachment {
			PixelFormat format;
			SampleCountFlag samples = SampleCountFlag::C1_BIT;

			AttachmentLoadOperation load_op   = AttachmentLoadOperation::Clear;
			AttachmentStoreOperation store_op = AttachmentStoreOperation::Store;

			AttachmentLoadOperation stencil_load_op =
				AttachmentLoadOperation::Dont_Care;
			AttachmentStoreOperation stencil_store_op =
				AttachmentStoreOperation::Dont_Care;

			ImageLayout initial_layout = ImageLayout::Undefined;
			ImageLayout final_layout   = ImageLayout::Present_Src;
		};

		struct Subpass {
			struct Ref {
				std::uint32_t attachment_id;

				ImageLayout layout = ImageLayout::Color_Attachment_Optimal;
			};

			PipelineBindPoint bind_point;
			std::vector<Ref> attachment_refs;
		};

		explicit RenderPass(const Device &device);
		 ~RenderPass() ;

		RenderPass(RenderPass &&);
		RenderPass &operator=(RenderPass &&);

		std::uint32_t addAttachment(Attachment attachment);
		inline void addSubpass(Subpass subpass) {
			m_subpasses.emplace_back(std::move(subpass));
		}

		 void build() ;
		 FramebufferOwnedPtr createFramebuffer(
            core::Extent extent,
			std::vector<TextureConstObserverPtr> textures) const ;

		inline const Device &device() const noexcept {
            return *m_device;
		}

		inline storm::core::span<const Attachment> attachments() const
			noexcept {
			return m_attachments;
		}

		inline storm::core::span<const Subpass> subpasses() const noexcept {
			return m_subpasses;
		}

		inline VkRenderPass vkRenderPass() const noexcept {
			STORM_EXPECTS(m_vk_render_pass != VK_NULL_HANDLE);
			return m_vk_render_pass;
		}

		inline operator VkRenderPass() const noexcept {
			STORM_EXPECTS(m_vk_render_pass != VK_NULL_HANDLE);
			return m_vk_render_pass;
		}

	  private:
		DeviceConstObserverPtr m_device;

		std::vector<Attachment> m_attachments;
		std::vector<Subpass> m_subpasses;

		VkRenderPass m_vk_render_pass = VK_NULL_HANDLE;
	};
} // namespace storm::render
