// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>

#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Semaphore::Semaphore(const render::Device &device) : m_device{&device} {
	const auto &device_ = static_cast<const Device &>(*m_device);

	const auto create_info =
		VkSemaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

	m_vk_semaphore = device_.createVkSemaphore(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
Semaphore::~Semaphore() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_semaphore != VK_NULL_HANDLE)
		device.destroyVkSemaphore(m_vk_semaphore);
}

/////////////////////////////////////
/////////////////////////////////////
Semaphore::Semaphore(Semaphore &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Semaphore &Semaphore::operator=(Semaphore &&) = default;
