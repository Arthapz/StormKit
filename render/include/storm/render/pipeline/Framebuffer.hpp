// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
	class STORM_PUBLIC Framebuffer : public core::NonCopyable {
	  public:
		Framebuffer(const RenderPass &render_pass, core::Extent size,
							 std::vector<TextureConstObserverPtr> textures);
		 ~Framebuffer() ;

		Framebuffer(Framebuffer &&);
		Framebuffer &operator=(Framebuffer &&);

		inline const core::Extent &extent() const noexcept {
			return m_extent;
		}

		inline const Device &device() const noexcept {
            return m_render_pass->device();
		}

        inline storm::core::span<const TextureConstObserverPtr> textures() const
            noexcept {
			return m_textures;
		}

		inline VkFramebuffer vkFramebuffer() const noexcept {
			STORM_EXPECTS(m_vk_framebuffer != VK_NULL_HANDLE);
			return m_vk_framebuffer;
		}

		inline operator VkFramebuffer() const noexcept {
			STORM_EXPECTS(m_vk_framebuffer != VK_NULL_HANDLE);
			return m_vk_framebuffer;
		}

	  private:
        RenderPassConstObserverPtr m_render_pass;

		core::Extent m_extent;
		std::vector<TextureConstObserverPtr> m_textures;

		VkFramebuffer m_vk_framebuffer = VK_NULL_HANDLE;
	};
} // namespace storm::render
