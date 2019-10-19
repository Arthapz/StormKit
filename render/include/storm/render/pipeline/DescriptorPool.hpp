// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
	class STORM_PUBLIC DescriptorPool : public core::NonCopyable {
	  public:
		struct Size {
			DescriptorType type;
			std::size_t descriptor_count;
		};

		DescriptorPool(const Device &device,
								const DescriptorSetLayout &layout,
								std::vector<Size> sizes, std::size_t max_sets);
		 ~DescriptorPool() ;

		DescriptorPool(DescriptorPool &&);
		DescriptorPool &operator=(DescriptorPool &&);

		 std::vector<DescriptorSetOwnedPtr>
			allocateDescriptorSets(std::size_t count) ;

		inline const Device &device() const noexcept {
            return *m_device;
		}

	  private:
		DeviceConstObserverPtr m_device;
		DescriptorSetLayoutConstObserverPtr m_layout;

		VkDescriptorPool m_vk_descriptor_pool = VK_NULL_HANDLE;
	};
} // namespace storm::render
