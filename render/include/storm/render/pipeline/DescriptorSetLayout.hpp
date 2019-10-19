// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
	struct DescriptorSetLayoutBinding {
		std::uint32_t binding;
		DescriptorType type;
		ShaderType stages;
		std::size_t descriptor_count;
	};

	class STORM_PUBLIC DescriptorSetLayout : public core::NonCopyable {
	  public:
		explicit DescriptorSetLayout(const Device &device);
		~DescriptorSetLayout();

		DescriptorSetLayout(DescriptorSetLayout &&);
		DescriptorSetLayout &operator=(DescriptorSetLayout &&);

		inline void addBinding(DescriptorSetLayoutBinding binding) {
			m_bindings.emplace_back(std::move(binding));

			updateHash();
		}

		void build();

		inline std::size_t hash() const noexcept {
			return m_hash;
		}
		inline storm::core::span<const DescriptorSetLayoutBinding>
			bindings() const noexcept {
			return m_bindings;
		}

		inline VkDescriptorSetLayout vkDescriptorSetLayout() const noexcept {
			STORM_EXPECTS(m_vk_descriptor_set_layout != VK_NULL_HANDLE);
			return m_vk_descriptor_set_layout;
		}

		inline operator VkDescriptorSetLayout() const noexcept {
			STORM_EXPECTS(m_vk_descriptor_set_layout != VK_NULL_HANDLE);
			return m_vk_descriptor_set_layout;
		}

		inline bool operator==(const DescriptorSetLayout &second) {
			return m_hash == second.hash();
		}

	  private:
		void updateHash() noexcept;

		DeviceConstObserverPtr m_device;

		std::vector<DescriptorSetLayoutBinding> m_bindings;

		std::size_t m_hash;

		VkDescriptorSetLayout m_vk_descriptor_set_layout = VK_NULL_HANDLE;
	};
} // namespace storm::render

HASH_FUNC(storm::render::DescriptorSetLayoutBinding)
HASH_FUNC(std::vector<storm::render::DescriptorSetLayoutBinding>)
