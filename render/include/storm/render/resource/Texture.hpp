// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/image/Image.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

namespace storm::render {
	class STORM_PUBLIC Texture : public core::NonCopyable {
	  public:
		explicit Texture(const Device &device);
		explicit Texture(const render::Device &device, core::Extent extent,
						 render::PixelFormat format, VkImage image);
		~Texture();

		Texture(Texture &&);
		Texture &operator=(Texture &&);

		void loadFromImage(image::Image &image);
		void loadFromMemory(storm::core::span<const std::byte> data,
									core::Extent extent,
									PixelFormat format);
		void
			createTextureData(core::Extent extent, PixelFormat format,
							  SampleCountFlag samples = SampleCountFlag::C1_BIT,
							  std::uint32_t mip_levels = 1,
							  std::uint32_t layers	 = 1,
							  ImageUsage usage		   = ImageUsage::Sampled |
												 ImageUsage::Transfert_Dst);

		inline core::Extent extent() const noexcept {
			return m_extent;
		}
		inline PixelFormat format() const noexcept {
			return m_format;
		}
		inline ImageLayout layout() const noexcept {
			return m_image_layout;
		}
		inline SampleCountFlag samples() const noexcept {
			return m_samples;
		}
		inline std::uint32_t mipLevels() const noexcept {
			return m_mip_levels;
		}
		inline std::uint32_t layers() const noexcept {
			return m_layers;
		}

		inline const Device &device() const noexcept {
            return *m_device;
		}

		inline VkImage vkImage() const noexcept {
			STORM_EXPECTS(m_vk_image != VK_NULL_HANDLE);
			return m_vk_image;
		}

		inline VkImageView vkImageView() const noexcept {
			STORM_EXPECTS(m_vk_image_view != VK_NULL_HANDLE);
			return m_vk_image_view;
		}

		inline operator VkImageView() const noexcept {
			STORM_EXPECTS(m_vk_image_view != VK_NULL_HANDLE);
			return m_vk_image_view;
		}

		inline const VkImageSubresourceRange &vkSubresourceRange() const
			noexcept {
			return m_subresource_range;
		}
	  protected:
		inline void setLayout(ImageLayout layout) const noexcept {
			m_image_layout = layout;
		}

		DeviceConstObserverPtr m_device;

		core::Extent m_extent			    = {0, 0};
		PixelFormat m_format			    = PixelFormat::Undefined;
		mutable ImageLayout m_image_layout = ImageLayout::Undefined;

		ImageUsage m_usage;

		SampleCountFlag m_samples  = SampleCountFlag::C1_BIT;
		std::uint32_t m_mip_levels = 1;
		std::uint32_t m_layers	 = 1;


		VmaAllocation m_vma_image_memory = VK_NULL_HANDLE;
		VkImage m_vk_image				 = VK_NULL_HANDLE;
		VkImageView m_vk_image_view		 = VK_NULL_HANDLE;

		VkImageSubresourceRange m_subresource_range;

		bool m_is_owning_image = true;

		friend class CommandBuffer;
	};
} // namespace storm::render
