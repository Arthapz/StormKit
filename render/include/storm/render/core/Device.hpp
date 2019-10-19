// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <gsl/string_span>

#include <storm/core/Filesystem.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Shader.hpp>

#include <storm/render/sync/Fwd.hpp>

namespace storm::render {
	class STORM_PUBLIC Device : public core::NonCopyable {
	  public:
		Device(const PhysicalDevice &physical_device, const Instance &instance);
        ~Device();

		Device(Device &&);
		Device &operator=(Device &&);

        void waitIdle() const noexcept;
        void waitForFences(storm::core::span<const FenceCRef> fences,
                           bool wait_all = true,
                           std::uint64_t timeout =
                               std::numeric_limits<std::uint64_t>::max()) const
            noexcept;
		inline void
			waitForFence(const Fence &fence,
						 std::uint64_t timeout =
                             std::numeric_limits<std::uint64_t>::max()) const
            noexcept;

        ShaderOwnedPtr createShader(core::filesystem::path filepath,
                                    Shader::Language language,
                                    ShaderType type) const;
        ShaderOwnedPtr createShader(core::span<const std::byte> data,
                                    Shader::Language language,
                                    ShaderType type) const;
        template <typename Container, std::enable_if_t<!std::is_same_v<
                                          Container, core::filesystem::path>>>
        inline ShaderOwnedPtr createShader(const Container &container,
                                           ShaderType type) const;

        GraphicsPipelineOwnedPtr createGraphicsPipeline() const;
        RenderPassOwnedPtr createRenderPass() const;
        FenceOwnedPtr createFence() const;
        SemaphoreOwnedPtr createSemaphore() const;
        HardwareBufferOwnedPtr createHardwareBuffer(
			HardwareBufferUsage usage, std::size_t size,
			MemoryProperty property = MemoryProperty::Host_Visible |
									  MemoryProperty::Host_Coherent) const;
        DescriptorSetLayoutOwnedPtr createDescriptorSetLayout() const;
        DescriptorPoolOwnedPtr
			createDescriptorPool(const DescriptorSetLayout &layout,
								 std::vector<DescriptorPool::Size> sizes,
								 std::size_t max_sets) const;
        TextureOwnedPtr createTexture() const;
        SamplerOwnedPtr createSampler(
			Sampler::Settings settings = Sampler::Settings{}) const;
		PipelineCacheOwnedPtr createPipelineCache() const;

		inline HardwareBufferOwnedPtr createVertexBuffer(
			std::size_t size,
			MemoryProperty property = MemoryProperty::Host_Visible |
									  MemoryProperty::Host_Coherent,
            bool use_staging = false) const;
		inline HardwareBufferOwnedPtr createIndexBuffer(
			std::size_t size,
			MemoryProperty property = MemoryProperty::Host_Visible |
									  MemoryProperty::Host_Coherent,
            bool use_staging = false) const;
		inline HardwareBufferOwnedPtr createUniformBuffer(
			std::size_t size,
			MemoryProperty property = MemoryProperty::Host_Visible |
									  MemoryProperty::Host_Coherent,
            bool use_staging = false) const;
		inline HardwareBufferOwnedPtr createStagingBuffer(
			std::size_t size,
			MemoryProperty property = MemoryProperty::Host_Visible |
                                      MemoryProperty::Host_Coherent) const;

        inline const Queue &graphicsQueue() const noexcept;
        inline const Queue &transfertQueue() const noexcept;
        inline const Queue &computeQueue() const noexcept;

        inline const PhysicalDevice &physicalDevice() const noexcept;

        inline VkDevice vkDevice() const noexcept;
        inline operator VkDevice() const noexcept;

        inline const VolkDeviceTable &vkDeviceTable() const noexcept;

        inline VmaAllocator vmaAllocator() const noexcept;

		void waitIdle(VkQueue queue) const noexcept;
		VkSwapchainKHR
			createVkSwapchain(const VkSwapchainCreateInfoKHR &create_info) const
			noexcept;
		void destroyVkSwapchain(VkSwapchainKHR swapchain) const noexcept;

		VkShaderModule createVkShaderModule(
			const VkShaderModuleCreateInfo &create_info) const noexcept;
		void destroyVkShaderModule(VkShaderModule module) const noexcept;

		VkPipeline createVkGraphicsPipeline(
			const VkGraphicsPipelineCreateInfo &create_info) const noexcept;
		VkPipeline createVkComputePipeline(
			const VkComputePipelineCreateInfo &create_info) const noexcept;
		void destroyVkPipeline(VkPipeline pipeline) const noexcept;

		VkPipelineLayout createVkPipelineLayout(
			const VkPipelineLayoutCreateInfo &create_info) const noexcept;
		void destroyVkPipelineLayout(VkPipelineLayout pipeline) const noexcept;

		VkRenderPass
			createVkRenderPass(const VkRenderPassCreateInfo &create_info) const
			noexcept;
		void destroyVkRenderPass(VkRenderPass pipeline) const noexcept;

		VkImage createVkImage(const VkImageCreateInfo &create_info) const
			noexcept;
		void destroyVkImage(VkImage image) const noexcept;

		VkImageView
			createVkImageView(const VkImageViewCreateInfo &create_info) const
			noexcept;
		void destroyVkImageView(VkImageView image_view) const noexcept;

		VkSampler createVkSampler(const VkSamplerCreateInfo &create_info) const
			noexcept;
		void destroyVkSampler(VkSampler sampler) const noexcept;

		VkFramebuffer createVkFramebuffer(
			const VkFramebufferCreateInfo &create_info) const noexcept;
		void destroyVkFramebuffer(VkFramebuffer framebuffer) const noexcept;

		VkCommandPool createVkCommandPool(
			const VkCommandPoolCreateInfo &create_info) const noexcept;
		void destroyVkCommandPool(VkCommandPool command_pool) const noexcept;

		VkFence createVkFence(const VkFenceCreateInfo &create_info) const
			noexcept;
		void destroyVkFence(VkFence fence) const noexcept;

		VkSemaphore
			createVkSemaphore(const VkSemaphoreCreateInfo &create_info) const
			noexcept;
		void destroyVkSemaphore(VkSemaphore semaphore) const noexcept;

		VkBuffer createVkBuffer(const VkBufferCreateInfo &create_info) const
			noexcept;
		void destroyVkBuffer(VkBuffer buffer) const noexcept;

		VkDescriptorSetLayout createVkDescriptorSetLayout(
			const VkDescriptorSetLayoutCreateInfo &create_info) const noexcept;
		void destroyVkDescriptorSetLayout(VkDescriptorSetLayout layout) const
			noexcept;

		VkDescriptorPool createVkDescriptorPool(
			const VkDescriptorPoolCreateInfo &create_info) const noexcept;
		void destroyVkDescriptorPool(VkDescriptorPool pool) const noexcept;

		std::vector<VkCommandBuffer> allocateVkCommandBuffers(
			const VkCommandBufferAllocateInfo &allocate_info) const noexcept;
		void deallocateVkCommandBuffers(
			const VkCommandPool command_pool,
			storm::core::span<const VkCommandBuffer> command_buffer) const
			noexcept;

		VmaAllocation
			allocateVmaAllocation(const VmaAllocationCreateInfo &allocate_info,
								  VkMemoryRequirements requirements) const
			noexcept;
		void deallocateVmaAllocation(VmaAllocation allocation) const noexcept;

		void bindVmaBufferMemory(VmaAllocation allocation,
								 VkBuffer buffer) const noexcept;

		std::byte *mapVmaMemory(VmaAllocation allocation) const noexcept;
		void unmapVmaMemory(VmaAllocation allocation) const noexcept;

	  protected:
		InstanceConstObserverPtr m_instance;

		PhysicalDeviceConstObserverPtr m_physical_device;

		QueueOwnedPtr m_graphics_queue;
		QueueOwnedPtr m_transfert_queue;
		QueueOwnedPtr m_compute_queue;

		VkDevice m_vk_device		 = VK_NULL_HANDLE;
		VmaAllocator m_vma_allocator = VK_NULL_HANDLE;
		VmaVulkanFunctions m_vma_device_table;

		struct VolkDeviceTable m_device_table;
    };
} // namespace storm::render

#include "Device.inl"
