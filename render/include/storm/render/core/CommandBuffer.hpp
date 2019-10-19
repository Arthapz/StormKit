// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <queue>
#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Command.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/GraphicsPipeline.hpp>

namespace storm::render {
	class STORM_PUBLIC CommandBuffer : public core::NonCopyable {
	  public:
		enum class State { Initial, Recording, Executable };

		CommandBuffer(const render::Queue &queue,
					  render::CommandBufferLevel level,
					  VkCommandBuffer command_buffer);
		~CommandBuffer();

		CommandBuffer(CommandBuffer &&);
		CommandBuffer &operator=(CommandBuffer &&);

		void reset() noexcept;
		void build() noexcept;

        inline State state() const noexcept;
        inline CommandBufferLevel level() const noexcept;

        template <typename CommandT> inline void add(CommandT &&command);
		template <typename CommandT, typename... Args>
        inline void add(Args &&... args);

		inline void
			begin(bool one_time_submit					  = false,
                  std::optional<CommandBufferCRef> parent = std::nullopt);
        inline void end();

		inline void beginRenderPass(
			const RenderPass &render_pass, const Framebuffer &framebuffer,
			BeginRenderPassCommand::ClearValues clear_values = {
                ClearColor{.color = core::RGBColorDef::Silver}});
        inline void endRenderPass();

        inline void bindGraphicsPipeline(const GraphicsPipeline &pipeline);

		inline void draw(std::uint32_t vertex_count,
						 std::uint32_t instance_count = 1u,
                         std::uint32_t first_vertex	  = 0,
                         std::uint32_t first_instance = 0);
		inline void drawIndexed(std::uint32_t index_count,
								std::uint32_t instance_count = 1u,
                                std::uint32_t first_index	 = 0u,
                                std::int32_t vertex_offset	 = 0,
                                std::uint32_t first_instance = 0u);

		inline void bindVertexBuffers(std::vector<HardwareBufferCRef> buffers,
                                      std::vector<std::ptrdiff_t> offsets);
		inline void bindIndexBuffer(const HardwareBuffer &buffer,
									std::ptrdiff_t offset = 0,
                                    bool large_indices	  = false);
        inline void
            bindDescriptorSets(const GraphicsPipeline &pipeline,
                               std::vector<DescriptorSetCRef> descriptor_sets);

		inline void copyBuffer(const HardwareBuffer &source,
							   const HardwareBuffer &destination,
							   std::size_t size, std::ptrdiff_t src_offset = 0u,
                               std::ptrdiff_t dst_offset = 0u);
		inline void copyBufferToImage(const HardwareBuffer &source,
                                      const Texture &destination);
        inline void copyImage(const Texture &source, ImageLayout source_layout,
                              ImageAspectMask source_aspect_mask,
                              const Texture &destination,
                              ImageLayout destination_layout,
                              ImageAspectMask destination_aspect_mask);

		inline void transitionImageLayout(Texture &image,
                                          ImageLayout new_layout);

		inline void executeSubCommandBuffers(
            std::vector<CommandBufferCRef> command_buffers);

        inline VkCommandBuffer vkCommandBuffer() const noexcept;
        inline operator VkCommandBuffer() const noexcept;

		void updateImageLayouts();

	  private:
		State m_state = State::Initial;
		QueueConstObserverPtr m_queue;
		CommandBufferLevel m_level;

		std::queue<Command> m_commands;

		VkCommandBuffer m_vk_command_buffer = VK_NULL_HANDLE;

		std::unordered_map<const Texture *, ImageLayout>
			m_to_update_texture_layout;
	};
} // namespace storm::render

#include "CommandBuffer.inl"
