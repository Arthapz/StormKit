// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>

#include <storm/render/sync/Fence.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Fence::Fence(const render::Device &device) : m_device{&device} {
	const auto &device_ = static_cast<const Device &>(*m_device);

	const auto create_info =
		VkFenceCreateInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
						  .flags = VK_FENCE_CREATE_SIGNALED_BIT};

	m_vk_fence = device_.createVkFence(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
Fence::~Fence() {
	const auto &device = static_cast<const Device &>(*m_device);

	if (m_vk_fence != VK_NULL_HANDLE)
		device.destroyVkFence(m_vk_fence);
}

/////////////////////////////////////
/////////////////////////////////////
Fence::Fence(Fence &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Fence &Fence::operator=(Fence &&) = default;

/////////////////////////////////////
/////////////////////////////////////
render::WaitResult Fence::wait(uint64_t wait_for) const {
	const auto &device		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device.vkDeviceTable();

	auto result =
		device_table.vkWaitForFences(device, 1, &m_vk_fence, VK_TRUE, wait_for);

	if (result == VK_SUCCESS)
		return render::WaitResult::Success;
	else if (result == VK_TIMEOUT)
		return render::WaitResult::Timeout;

	return render::WaitResult::Error;
}

/////////////////////////////////////
/////////////////////////////////////
void Fence::reset() {
	const auto &device		 = static_cast<const Device &>(*m_device);
	const auto &device_table = device.vkDeviceTable();

	device_table.vkResetFences(device, 1, &m_vk_fence);
}
