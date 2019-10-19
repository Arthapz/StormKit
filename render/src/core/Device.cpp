// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <unordered_set>

#include <storm/render/core/Instance.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPass.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Shader.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Device::Device(const PhysicalDevice &physical_device, const Instance &instance)
	: m_instance{&instance}, m_physical_device{&physical_device} {
	const auto &queue_families = m_physical_device->queueFamilies();

	auto graphics_id			= -1;
	auto graphics_count			= 0u;
	auto graphics_family_flags  = QueueFlag{};
	auto transfert_id			= -1;
	auto transfert_count		= 0u;
	auto transfert_family_flags = QueueFlag{};
	/*auto compute_id         = -1;
	auto compute_count          = 0u;
	auto compute_family_flags   = QueueFlag{};*/

	auto i = 0;
	for (const auto &family : queue_families) {
		if ((family.flags & QueueFlag::Graphics) == QueueFlag::Graphics &&
			graphics_id == -1) {
			graphics_id			  = i;
			graphics_count		  = family.count;
			graphics_family_flags = family.flags;
		}
		if ((family.flags & QueueFlag::Transfert) == QueueFlag::Transfert &&
			transfert_id == -1) {
			transfert_id		   = i;
			transfert_count		   = family.count;
			transfert_family_flags = family.flags;
		} else if ((family.flags & QueueFlag::Transfert) ==
					   QueueFlag::Transfert &&
				   (family.flags & QueueFlag::Graphics) !=
					   QueueFlag::Graphics) {
			transfert_id		   = i;
			transfert_count		   = family.count;
			transfert_family_flags = family.flags;
		}
		/*if((family.flags & QueueFlag::Compute) == QueueFlag::Compute) {
			compute_id           = i;
			compute_count        = family.count;
			compute_family_flags = family.flags;
		}*/
		++i;
	}

	auto graphics_priorities = std::vector<float>{};
	graphics_priorities.resize(graphics_count, 1.f);
	auto transfert_priorities = std::vector<float>{};
	transfert_priorities.resize(transfert_count, 1.f);
	/*auto compute_priorities = std::vector<float>{};
	compute_priorities.resize(compute_count, 1.f);*/

	const auto graphics_create_info = VkDeviceQueueCreateInfo{
		.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = gsl::narrow_cast<std::uint32_t>(graphics_id),
		.queueCount		  = graphics_count,
		.pQueuePriorities = std::data(graphics_priorities)};
	const auto transfert_create_info = VkDeviceQueueCreateInfo{
		.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = gsl::narrow_cast<std::uint32_t>(transfert_id),
		.queueCount		  = transfert_count,
		.pQueuePriorities = std::data(transfert_priorities)};
	/*const auto compute_create_info =
	  VkDeviceQueueCreateInfo{.sType            =
	  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .queueFamilyIndex =
	  gsl::narrow_cast<std::uint32_t>(compute_id), .queueCount       =
	  transfert_count, .pQueuePriorities = std::data(compute_priorities)};*/

	auto queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
	queue_create_infos.emplace_back(std::move(graphics_create_info));
	if (graphics_id != transfert_id)
		queue_create_infos.emplace_back(std::move(transfert_create_info));
	// queue_create_infos.emplace_back(std::move(compute_create_info));

	const auto enabled_features = VkPhysicalDeviceFeatures{};
	const auto create_info		= VkDeviceCreateInfo{
		 .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		 .queueCreateInfoCount =
			 gsl::narrow_cast<std::uint32_t>(std::size(queue_create_infos)),
		 .pQueueCreateInfos = std::data(queue_create_infos),
		 .enabledExtensionCount =
			 gsl::narrow_cast<std::uint32_t>(std::size(DEVICE_EXTENSIONS)),
		 .ppEnabledExtensionNames = std::data(DEVICE_EXTENSIONS),
		 .pEnabledFeatures		  = &enabled_features};

	m_vk_device = physical_device.createVkDevice(create_info);

	volkLoadDeviceTable(&m_device_table, m_vk_device);
	if (m_device_table.vkCreateSwapchainKHR == nullptr)
		m_device_table.vkCreateSwapchainKHR =
			reinterpret_cast<PFN_vkCreateSwapchainKHR>(
				vkGetDeviceProcAddr(m_vk_device, "vkCreateSwapchainKHR"));
	if (m_device_table.vkDestroySwapchainKHR == nullptr)
		m_device_table.vkDestroySwapchainKHR =
			reinterpret_cast<PFN_vkDestroySwapchainKHR>(
				vkGetDeviceProcAddr(m_vk_device, "vkDestroySwapchainKHR"));
	if (m_device_table.vkAcquireNextImageKHR == nullptr)
		m_device_table.vkAcquireNextImageKHR =
			reinterpret_cast<PFN_vkAcquireNextImageKHR>(
				vkGetDeviceProcAddr(m_vk_device, "vkAcquireNextImageKHR"));
	if (m_device_table.vkQueuePresentKHR == nullptr)
		m_device_table.vkQueuePresentKHR =
			reinterpret_cast<PFN_vkQueuePresentKHR>(
				vkGetDeviceProcAddr(m_vk_device, "vkQueuePresentKHR"));

	auto graphics_vk_queue = VkQueue{};
	m_device_table.vkGetDeviceQueue(
		m_vk_device, gsl::narrow_cast<std::uint32_t>(graphics_id), 0,
		&graphics_vk_queue);
	auto transfert_vk_queue = VkQueue{};
	m_device_table.vkGetDeviceQueue(m_vk_device, transfert_id, 0,
									&transfert_vk_queue);
	/*auto compute_vk_queue = VkQueue{};
	m_device_table.vkGetDeviceQueue(m_vk_device, compute_id, 0,
	&compute_vk_queue);*/

	m_graphics_queue  = std::make_unique<Queue>(*this, graphics_family_flags,
												graphics_id, graphics_vk_queue);
	m_transfert_queue = std::make_unique<Queue>(
		*this, transfert_family_flags, transfert_id, transfert_vk_queue);
	/*m_compute_queue =
	  std::make_unique<Queue>(*this, compute_family_flags, compute_id,
	  compute_vk_queue);*/

	m_vma_device_table = VmaVulkanFunctions{
		.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
		.vkGetPhysicalDeviceMemoryProperties =
			vkGetPhysicalDeviceMemoryProperties,
		.vkAllocateMemory		   = m_device_table.vkAllocateMemory,
		.vkFreeMemory			   = m_device_table.vkFreeMemory,
		.vkMapMemory			   = m_device_table.vkMapMemory,
		.vkUnmapMemory			   = m_device_table.vkUnmapMemory,
		.vkFlushMappedMemoryRanges = m_device_table.vkFlushMappedMemoryRanges,
		.vkInvalidateMappedMemoryRanges =
			m_device_table.vkInvalidateMappedMemoryRanges,
		.vkBindBufferMemory = m_device_table.vkBindBufferMemory,
		.vkBindImageMemory  = m_device_table.vkBindImageMemory,
		.vkGetBufferMemoryRequirements =
			m_device_table.vkGetBufferMemoryRequirements,
		.vkGetImageMemoryRequirements =
			m_device_table.vkGetImageMemoryRequirements,
		.vkCreateBuffer  = m_device_table.vkCreateBuffer,
		.vkDestroyBuffer = m_device_table.vkDestroyBuffer,
		.vkCreateImage   = m_device_table.vkCreateImage,
		.vkDestroyImage  = m_device_table.vkDestroyImage,
		.vkCmdCopyBuffer = m_device_table.vkCmdCopyBuffer};
	const auto alloc_create_info =
		VmaAllocatorCreateInfo{.physicalDevice   = physical_device,
							   .device			 = m_vk_device,
							   .pVulkanFunctions = &m_vma_device_table};

	auto result = vmaCreateAllocator(&alloc_create_info, &m_vma_allocator);
	STORM_ENSURES(result == VK_SUCCESS);
}

/////////////////////////////////////
/////////////////////////////////////
Device::~Device() {
	m_graphics_queue.reset();
	m_transfert_queue.reset();
	m_compute_queue.reset();

	if (m_vma_allocator != VK_NULL_HANDLE)
		vmaDestroyAllocator(m_vma_allocator);
	if (m_vk_device != VK_NULL_HANDLE)
		m_device_table.vkDestroyDevice(m_vk_device, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
Device::Device(Device &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Device &Device::operator=(Device &&) = default;

/////////////////////////////////////
/////////////////////////////////////
PipelineCacheOwnedPtr Device::createPipelineCache() const {
	return std::make_unique<PipelineCache>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::waitIdle() const noexcept {
	m_device_table.vkDeviceWaitIdle(m_vk_device);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::waitForFences(storm::core::span<const render::FenceCRef> fences,
						   bool wait_all, std::uint64_t timeout) const
	noexcept {
	auto vk_fences = std::vector<VkFence>{};
	vk_fences.reserve(std::size(fences));
	for (const auto &fence : fences)
		vk_fences.emplace_back(static_cast<const Fence &>(fence.get()));

	m_device_table.vkWaitForFences(
		m_vk_device, gsl::narrow_cast<std::uint32_t>(std::size(vk_fences)),
		std::data(vk_fences), wait_all, timeout);
}

/////////////////////////////////////
/////////////////////////////////////
render::ShaderOwnedPtr Device::createShader(core::filesystem::path filepath,
											render::Shader::Language language,
											render::ShaderType type) const {
	return std::make_unique<Shader>(std::move(filepath), language, type, *this);
}

/////////////////////////////////////
/////////////////////////////////////
render::ShaderOwnedPtr Device::createShader(core::span<const std::byte> data,
											render::Shader::Language language,
											render::ShaderType type) const {
	return std::make_unique<Shader>(data, language, type, *this);
}

/////////////////////////////////////
/////////////////////////////////////
render::GraphicsPipelineOwnedPtr Device::createGraphicsPipeline() const {
	return std::make_unique<GraphicsPipeline>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
render::RenderPassOwnedPtr Device::createRenderPass() const {
	return std::make_unique<RenderPass>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
render::FenceOwnedPtr Device::createFence() const {
	return std::make_unique<Fence>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
render::SemaphoreOwnedPtr Device::createSemaphore() const {
	return std::make_unique<Semaphore>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
render::HardwareBufferOwnedPtr
	Device::createHardwareBuffer(render::HardwareBufferUsage usage,
								 std::size_t size,
								 render::MemoryProperty property) const {
	const auto &physical_device =
		static_cast<const PhysicalDevice &>(*m_physical_device);

	return std::make_unique<HardwareBuffer>(
		*this, usage, size, property);
}

/////////////////////////////////////
/////////////////////////////////////
render::DescriptorSetLayoutOwnedPtr Device::createDescriptorSetLayout() const {
	return std::make_unique<DescriptorSetLayout>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
render::DescriptorPoolOwnedPtr
	Device::createDescriptorPool(const render::DescriptorSetLayout &layout,
								 std::vector<DescriptorPool::Size> sizes,
								 std::size_t max_sets) const {
	return std::make_unique<DescriptorPool>(*this, layout, std::move(sizes),
											max_sets);
}

/////////////////////////////////////
/////////////////////////////////////
render::TextureOwnedPtr Device::createTexture() const {
	return std::make_unique<Texture>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
render::SamplerOwnedPtr
	Device::createSampler(render::Sampler::Settings settings) const {
	return std::make_unique<Sampler>(std::move(settings), *this);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::waitIdle(VkQueue queue) const noexcept {
	m_device_table.vkQueueWaitIdle(queue);
}

/////////////////////////////////////
/////////////////////////////////////
VkSwapchainKHR
	Device::createVkSwapchain(const VkSwapchainCreateInfoKHR &create_info) const
	noexcept {
	auto vk_swapchain = VkSwapchainKHR{};

	auto result = m_device_table.vkCreateSwapchainKHR(m_vk_device, &create_info,
													  nullptr, &vk_swapchain);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_swapchain;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkSwapchain(VkSwapchainKHR swapchain) const noexcept {
	STORM_EXPECTS(swapchain != VK_NULL_HANDLE);

	m_device_table.vkDestroySwapchainKHR(m_vk_device, swapchain, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkShaderModule Device::createVkShaderModule(
	const VkShaderModuleCreateInfo &create_info) const noexcept {
	auto vk_shader_module = VkShaderModule{};

	auto result = m_device_table.vkCreateShaderModule(
		m_vk_device, &create_info, nullptr, &vk_shader_module);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_shader_module;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkShaderModule(VkShaderModule module) const noexcept {
	STORM_EXPECTS(module != VK_NULL_HANDLE);

	m_device_table.vkDestroyShaderModule(m_vk_device, module, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkPipeline Device::createVkGraphicsPipeline(
	const VkGraphicsPipelineCreateInfo &create_info) const noexcept {
	auto vk_pipeline = VkPipeline{};

	auto result = m_device_table.vkCreateGraphicsPipelines(
		m_vk_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &vk_pipeline);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_pipeline;
}

/////////////////////////////////////
/////////////////////////////////////
VkPipeline Device::createVkComputePipeline(
	const VkComputePipelineCreateInfo &create_info) const noexcept {
	auto vk_pipeline = VkPipeline{};

	auto result = m_device_table.vkCreateComputePipelines(
		m_vk_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &vk_pipeline);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_pipeline;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkPipeline(VkPipeline pipeline) const noexcept {
	STORM_EXPECTS(pipeline != VK_NULL_HANDLE);

	m_device_table.vkDestroyPipeline(m_vk_device, pipeline, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkPipelineLayout Device::createVkPipelineLayout(
	const VkPipelineLayoutCreateInfo &create_info) const noexcept {
	auto vk_pipeline_layout = VkPipelineLayout{};

	auto result = m_device_table.vkCreatePipelineLayout(
		m_vk_device, &create_info, nullptr, &vk_pipeline_layout);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_pipeline_layout;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkPipelineLayout(VkPipelineLayout pipeline_layout) const
	noexcept {
	STORM_EXPECTS(pipeline_layout != VK_NULL_HANDLE);

	m_device_table.vkDestroyPipelineLayout(m_vk_device, pipeline_layout,
										   nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkRenderPass
	Device::createVkRenderPass(const VkRenderPassCreateInfo &create_info) const
	noexcept {
	auto vk_render_pass = VkRenderPass{};

	auto result = m_device_table.vkCreateRenderPass(m_vk_device, &create_info,
													nullptr, &vk_render_pass);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_render_pass;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkRenderPass(VkRenderPass render_pass) const noexcept {
	STORM_EXPECTS(render_pass != VK_NULL_HANDLE);

	m_device_table.vkDestroyRenderPass(m_vk_device, render_pass, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkImage Device::createVkImage(const VkImageCreateInfo &create_info) const
	noexcept {
	auto vk_image = VkImage{};

	auto result = m_device_table.vkCreateImage(m_vk_device, &create_info,
											   nullptr, &vk_image);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_image;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkImage(VkImage image) const noexcept {
	STORM_EXPECTS(image != VK_NULL_HANDLE);

	m_device_table.vkDestroyImage(m_vk_device, image, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkImageView
	Device::createVkImageView(const VkImageViewCreateInfo &create_info) const
	noexcept {
	auto vk_image_view = VkImageView{};

	auto result = m_device_table.vkCreateImageView(m_vk_device, &create_info,
												   nullptr, &vk_image_view);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_image_view;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkImageView(VkImageView image_view) const noexcept {
	STORM_EXPECTS(image_view != VK_NULL_HANDLE);

	m_device_table.vkDestroyImageView(m_vk_device, image_view, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkSampler Device::createVkSampler(const VkSamplerCreateInfo &create_info) const
	noexcept {
	auto vk_sampler = VkSampler{};

	auto result = m_device_table.vkCreateSampler(m_vk_device, &create_info,
												 nullptr, &vk_sampler);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_sampler;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkSampler(VkSampler sampler) const noexcept {
	STORM_EXPECTS(sampler != VK_NULL_HANDLE);

	m_device_table.vkDestroySampler(m_vk_device, sampler, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkFramebuffer Device::createVkFramebuffer(
	const VkFramebufferCreateInfo &create_info) const noexcept {
	auto vk_framebuffer = VkFramebuffer{};

	auto result = m_device_table.vkCreateFramebuffer(m_vk_device, &create_info,
													 nullptr, &vk_framebuffer);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_framebuffer;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkFramebuffer(VkFramebuffer framebuffer) const noexcept {
	STORM_EXPECTS(framebuffer != VK_NULL_HANDLE);

	m_device_table.vkDestroyFramebuffer(m_vk_device, framebuffer, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkCommandPool Device::createVkCommandPool(
	const VkCommandPoolCreateInfo &create_info) const noexcept {
	auto vk_command_pool = VkCommandPool{};

	auto result = m_device_table.vkCreateCommandPool(m_vk_device, &create_info,
													 nullptr, &vk_command_pool);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_command_pool;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkCommandPool(VkCommandPool command_pool) const noexcept {
	STORM_EXPECTS(command_pool != VK_NULL_HANDLE);

	m_device_table.vkDestroyCommandPool(m_vk_device, command_pool, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkFence Device::createVkFence(const VkFenceCreateInfo &create_info) const
	noexcept {
	auto vk_fence = VkFence{};

	auto result = m_device_table.vkCreateFence(m_vk_device, &create_info,
											   nullptr, &vk_fence);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_fence;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkFence(VkFence fence) const noexcept {
	STORM_EXPECTS(fence != VK_NULL_HANDLE);

	m_device_table.vkDestroyFence(m_vk_device, fence, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkSemaphore
	Device::createVkSemaphore(const VkSemaphoreCreateInfo &create_info) const
	noexcept {
	auto vk_semaphore = VkSemaphore{};

	auto result = m_device_table.vkCreateSemaphore(m_vk_device, &create_info,
												   nullptr, &vk_semaphore);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_semaphore;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkSemaphore(VkSemaphore semaphore) const noexcept {
	STORM_EXPECTS(semaphore != VK_NULL_HANDLE);

	m_device_table.vkDestroySemaphore(m_vk_device, semaphore, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkBuffer Device::createVkBuffer(const VkBufferCreateInfo &create_info) const
	noexcept {
	auto vk_buffer = VkBuffer{};

	auto result = m_device_table.vkCreateBuffer(m_vk_device, &create_info,
												nullptr, &vk_buffer);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_buffer;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkBuffer(VkBuffer buffer) const noexcept {
	STORM_EXPECTS(buffer != VK_NULL_HANDLE);

	m_device_table.vkDestroyBuffer(m_vk_device, buffer, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkDescriptorSetLayout Device::createVkDescriptorSetLayout(
	const VkDescriptorSetLayoutCreateInfo &create_info) const noexcept {
	auto vk_descriptor_set_layout = VkDescriptorSetLayout{};

	auto result = m_device_table.vkCreateDescriptorSetLayout(
		m_vk_device, &create_info, nullptr, &vk_descriptor_set_layout);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_descriptor_set_layout;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkDescriptorSetLayout(VkDescriptorSetLayout layout) const
	noexcept {
	STORM_EXPECTS(layout != VK_NULL_HANDLE);

	m_device_table.vkDestroyDescriptorSetLayout(m_vk_device, layout, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
VkDescriptorPool Device::createVkDescriptorPool(
	const VkDescriptorPoolCreateInfo &create_info) const noexcept {
	auto vk_descriptor_pool = VkDescriptorPool{};

	auto result = m_device_table.vkCreateDescriptorPool(
		m_vk_device, &create_info, nullptr, &vk_descriptor_pool);
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_descriptor_pool;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::destroyVkDescriptorPool(VkDescriptorPool pool) const noexcept {
	STORM_EXPECTS(pool != VK_NULL_HANDLE);

	m_device_table.vkDestroyDescriptorPool(m_vk_device, pool, nullptr);
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<VkCommandBuffer> Device::allocateVkCommandBuffers(
	const VkCommandBufferAllocateInfo &allocate_info) const noexcept {
	auto vk_command_buffers = std::vector<VkCommandBuffer>{};
	vk_command_buffers.resize(allocate_info.commandBufferCount);

	auto result = m_device_table.vkAllocateCommandBuffers(
		m_vk_device, &allocate_info, std::data(vk_command_buffers));
	STORM_ENSURES(result == VK_SUCCESS);

	return vk_command_buffers;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::deallocateVkCommandBuffers(
	const VkCommandPool command_pool,
	storm::core::span<const VkCommandBuffer> command_buffers) const noexcept {
	for (const auto command_buffer : command_buffers)
		STORM_EXPECTS(command_buffer != VK_NULL_HANDLE);

	m_device_table.vkFreeCommandBuffers(
		m_vk_device, command_pool,
		gsl::narrow_cast<std::uint32_t>(std::size(command_buffers)),
		std::data(command_buffers));
}

/////////////////////////////////////
/////////////////////////////////////
VmaAllocation
	Device::allocateVmaAllocation(const VmaAllocationCreateInfo &allocate_info,
								  VkMemoryRequirements requirements) const
	noexcept {
	auto vma_allocation = VmaAllocation{};

	auto result = vmaAllocateMemory(m_vma_allocator, &requirements,
									&allocate_info, &vma_allocation, nullptr);
	STORM_ENSURES(result == VK_SUCCESS);

	return vma_allocation;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::deallocateVmaAllocation(VmaAllocation allocation) const noexcept {
	STORM_EXPECTS(allocation != VK_NULL_HANDLE);

	vmaFreeMemory(m_vma_allocator, allocation);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::bindVmaBufferMemory(VmaAllocation allocation,
								 VkBuffer buffer) const noexcept {
	STORM_EXPECTS(allocation != VK_NULL_HANDLE);
	STORM_EXPECTS(buffer != VK_NULL_HANDLE);

	vmaBindBufferMemory(m_vma_allocator, allocation, buffer);
}

/////////////////////////////////////
/////////////////////////////////////
std::byte *Device::mapVmaMemory(VmaAllocation allocation) const noexcept {
	std::byte *data = nullptr;

	auto result = vmaMapMemory(m_vma_allocator, allocation,
							   reinterpret_cast<void **>(&data));
	STORM_ENSURES(result == VK_SUCCESS);

	return data;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::unmapVmaMemory(VmaAllocation allocation) const noexcept {
	vmaUnmapMemory(m_vma_allocator, allocation);
}
