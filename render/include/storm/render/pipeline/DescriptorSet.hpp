// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
	struct BufferDescriptor {
		std::uint32_t binding;
		HardwareBufferConstObserverPtr buffer;
		std::uint32_t range;
		std::uint32_t offset ;
	};

	struct ImageDescriptor {
		std::uint32_t binding;
		ImageLayout layout;
		TextureConstObserverPtr texture;
		SamplerConstObserverPtr sampler;
	};

	using Descriptor = std::variant<BufferDescriptor, ImageDescriptor>;

	class STORM_PUBLIC DescriptorSet : public core::NonCopyable {
	  public:
		DescriptorSet(const render::DescriptorPool &pool,
							   std::vector<DescriptorType> type,
							   VkDescriptorSet sets);
		 ~DescriptorSet() ;

		DescriptorSet(DescriptorSet &&);
		DescriptorSet &operator=(DescriptorSet &&);

		 void update(std::vector<Descriptor> descriptors) ;

		inline core::span<const DescriptorType> types() const noexcept {
			return m_types;
		}

		inline VkDescriptorSet vkDescriptorSet() const noexcept {
			STORM_EXPECTS(m_vk_descriptor_set != VK_NULL_HANDLE);
			return m_vk_descriptor_set;
		}

		inline operator VkDescriptorSet() const noexcept {
			STORM_EXPECTS(m_vk_descriptor_set != VK_NULL_HANDLE);
			return m_vk_descriptor_set;
		}

	  private:
        DescriptorPoolConstObserverPtr m_pool;

		std::vector<DescriptorType> m_types;

		VkDescriptorSet m_vk_descriptor_set;
	};
} // namespace storm::render
