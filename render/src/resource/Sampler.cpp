// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <fstream>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/resource/Sampler.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Sampler::Sampler(Settings settings, const render::Device &device)
	: m_settings{std::move(settings)}, m_device{&device} {
	const auto create_info = VkSamplerCreateInfo{
		.sType					 = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter				 = toVK(m_settings.mag_filter),
		.minFilter				 = toVK(m_settings.min_filter),
		.mipmapMode				 = toVK(m_settings.mipmap_mode),
		.addressModeU			 = toVK(m_settings.address_mode_u),
		.addressModeV			 = toVK(m_settings.address_mode_v),
		.addressModeW			 = toVK(m_settings.address_mode_w),
		.mipLodBias				 = m_settings.mip_lod_bias,
		.anisotropyEnable		 = m_settings.enable_anisotropy,
		.maxAnisotropy			 = m_settings.max_anisotropy,
		.compareEnable			 = m_settings.compare_enable,
		.compareOp				 = toVK(m_settings.compare_operation),
		.minLod					 = m_settings.min_lod,
		.maxLod					 = m_settings.max_lod,
		.borderColor			 = toVK(m_settings.border_color),
		.unnormalizedCoordinates = m_settings.unnormalized_coordinates};

	m_vk_sampler = static_cast<const Device &>(*m_device)
					   .createVkSampler(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
Sampler::~Sampler() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_sampler != VK_NULL_HANDLE)
		device.destroyVkSampler(m_vk_sampler);
}

/////////////////////////////////////
/////////////////////////////////////
Sampler::Sampler(Sampler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Sampler &Sampler::operator=(Sampler &&) = default;
