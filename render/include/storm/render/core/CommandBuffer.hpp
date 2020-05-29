// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <queue>
#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Command.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Texture.hpp>

#include <storm/render/pipeline/GraphicsPipeline.hpp>

namespace storm::render {
    class STORM_PUBLIC CommandBuffer: public core::NonCopyable {
      public:
        enum class State { Initial, Recording, Executable };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Command_Buffer;

        CommandBuffer(const render::Queue &queue,
                      render::CommandBufferLevel level,
                      RAIIVkCommandBuffer &&command_buffer);
        ~CommandBuffer();

        CommandBuffer(CommandBuffer &&);
        CommandBuffer &operator=(CommandBuffer &&);

        void reset() noexcept;
        void build();
        inline void
            submit(storm::core::span<const SemaphoreConstObserverPtr> wait_semaphores   = {},
                   storm::core::span<const SemaphoreConstObserverPtr> signal_semaphores = {},
                   FenceObserverPtr fence = nullptr) const noexcept;

        inline State state() const noexcept;
        inline CommandBufferLevel level() const noexcept;

        template<typename CommandT>
        inline void add(CommandT &&command);
        template<typename CommandT, typename... Args>
        inline void add(Args &&... args);

        inline void beginDebugRegion(std::string_view name,
                                     core::RGBColorF color = core::RGBColorDef::White<float>);
        inline void insertDebugLabel(std::string_view name,
                                     core::RGBColorF color = core::RGBColorDef::White<float>);
        inline void endDebugRegion();

        inline void begin(bool one_time_submit                    = false,
                          std::optional<CommandBufferCRef> parent = std::nullopt);
        inline void end();

        inline void
            beginRenderPass(const RenderPass &render_pass,
                            const Framebuffer &framebuffer,
                            BeginRenderPassCommand::ClearValues clear_values = { ClearColor {
                                .color = core::RGBColorDef::Silver<float> } },
                            bool secondary_command_buffers                   = false);
        inline void nextSubPass();
        inline void endRenderPass();

        inline void bindGraphicsPipeline(const GraphicsPipeline &pipeline);

        inline void draw(core::UInt32 vertex_count,
                         core::UInt32 instance_count = 1u,
                         core::UInt32 first_vertex   = 0,
                         core::UInt32 first_instance = 0);
        inline void drawIndexed(core::UInt32 index_count,
                                core::UInt32 instance_count = 1u,
                                core::UInt32 first_index    = 0u,
                                core::Offset vertex_offset  = 0,
                                core::UInt32 first_instance = 0u);

        inline void bindVertexBuffers(std::vector<HardwareBufferCRef> buffers,
                                      std::vector<core::Offset> offsets);
        inline void bindIndexBuffer(const HardwareBuffer &buffer,
                                    core::Offset offset = 0,
                                    bool large_indices  = false);
        inline void bindDescriptorSets(const GraphicsPipeline &pipeline,
                                       std::vector<DescriptorSetCRef> descriptor_sets,
                                       std::vector<core::UOffset> dynamic_offsets = {});

        inline void copyBuffer(const HardwareBuffer &source,
                               const HardwareBuffer &destination,
                               core::ArraySize size,
                               core::UOffset src_offset = 0u,
                               core::UOffset dst_offset = 0u);
        inline void copyBufferToTexture(const HardwareBuffer &source,
                                        const Texture &destination,
                                        std::vector<BufferTextureCopy> buffer_image_copies = {});
        inline void copyTexture(const Texture &source,
                                const Texture &destination,
                                TextureLayout source_layout,
                                TextureLayout destination_layout,
                                TextureSubresourceLayers source_subresource_layers,
                                TextureSubresourceLayers destination_subresource_layers,
                                core::Extentu extent);

        inline void resolveTexture(const Texture &source,
                                   const Texture &destination,
                                   TextureLayout source_layout,
                                   TextureLayout destination_layout,
                                   TextureSubresourceLayers source_subresource_layers      = {},
                                   TextureSubresourceLayers destination_subresource_layers = {});

        inline void blitTexture(const Texture &source,
                                const Texture &destination,
                                TextureLayout source_layout,
                                TextureLayout destination_layout,
                                std::vector<BlitRegion> regions,
                                Filter filter);

        inline void transitionTextureLayout(const Texture &texture,
                                            TextureLayout source_layout,
                                            TextureLayout destination_layout,
                                            TextureSubresourceRange subresource_range = {});

        inline void executeSubCommandBuffers(std::vector<CommandBufferCRef> command_buffers);

        inline void setViewport(core::UInt32 first_viewport, std::vector<Viewport> viewports);

        inline void setScissor(core::UInt32 first_scissor, std::vector<Scissor> scissors);

        inline void pipelineBarrier(PipelineStageFlag src_mask,
                                    PipelineStageFlag dst_mask,
                                    DependencyFlag dependency,
                                    MemoryBarriers memory_barriers,
                                    BufferMemoryBarriers buffer_memory_barriers,
                                    ImageMemoryBarriers image_memory_barriers);

        inline void pushConstants(const GraphicsPipeline &pipeline,
                                  ShaderStage stage,
                                  std::vector<std::byte> data);

        inline vk::CommandBuffer vkCommandBuffer() const noexcept;
        inline operator vk::CommandBuffer() const noexcept;
        inline vk::CommandBuffer vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        State m_state = State::Initial;
        QueueConstObserverPtr m_queue;
        CommandBufferLevel m_level;

        std::queue<Command> m_commands;

        RAIIVkCommandBuffer m_vk_command_buffer;

        core::HashMap<TextureConstObserverPtr, TextureLayout> m_to_update_texture_layout;

        core::HashSet<std::string> m_debug_labels;
    };
} // namespace storm::render

#include "CommandBuffer.inl"
