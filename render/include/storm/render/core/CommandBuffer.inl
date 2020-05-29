// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::submit(storm::core::span<const SemaphoreConstObserverPtr> wait_semaphores,
                               storm::core::span<const SemaphoreConstObserverPtr> signal_semaphores,
                               FenceObserverPtr fence) const noexcept {
        auto to_submit = core::makeConstObserversArray(this);
        m_queue->submit(to_submit, wait_semaphores, signal_semaphores, fence);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBuffer::State CommandBuffer::state() const noexcept { return m_state; }
    /////////////////////////////////////
    /////////////////////////////////////
    CommandBufferLevel CommandBuffer::level() const noexcept { return m_level; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename CommandT>
    void CommandBuffer::add(CommandT &&command) {
        STORM_EXPECTS(m_state == State::Initial || m_state == State::Recording);

        if (m_state == State::Initial) m_state = State::Recording;

        m_commands.emplace(std::forward<CommandT>(command));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename CommandT, typename... Args>
    void CommandBuffer::add(Args &&... args) {
        STORM_EXPECTS(m_state == State::Initial || m_state == State::Recording);

        if (m_state == State::Initial) m_state = State::Recording;

        m_commands.emplace(CommandT { std::forward<Args>(args)... });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::beginDebugRegion(std::string_view name, core::RGBColorF color) {
        add<BeginDebugRegionCommand>(std::string { name }, std::move(color));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::insertDebugLabel(std::string_view name, core::RGBColorF color) {
        add<InsertDebugLabelCommand>(std::string { name }, std::move(color));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::endDebugRegion() { add<EndDebugRegionCommand>(); }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::begin(bool one_time_submit, std::optional<CommandBufferCRef> parent) {
        add<BeginCommand>(one_time_submit, parent);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::end() { add<EndCommand>(); }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::beginRenderPass(const RenderPass &render_pass,
                                        const Framebuffer &framebuffer,
                                        BeginRenderPassCommand::ClearValues clear_values,
                                        bool secondary_command_buffers) {
        add<BeginRenderPassCommand>(render_pass,
                                    framebuffer,
                                    std::move(clear_values),
                                    secondary_command_buffers);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::nextSubPass() { add<NextSubPassCommand>(); }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::endRenderPass() { add<EndRenderPassCommand>(); }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::bindGraphicsPipeline(const GraphicsPipeline &pipeline) {
        add<BindGraphicsPipelineCommand>(pipeline);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::draw(core::UInt32 vertex_count,
                             core::UInt32 instance_count,
                             core::UInt32 first_vertex,
                             core::UInt32 first_instance) {
        add<DrawCommand>(vertex_count, instance_count, first_vertex, first_instance);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::drawIndexed(core::UInt32 index_count,
                                    core::UInt32 instance_count,
                                    core::UInt32 first_index,
                                    core::Offset vertex_offset,
                                    core::UInt32 first_instance) {
        add<DrawIndexedCommand>(index_count,
                                instance_count,
                                first_index,
                                vertex_offset,
                                first_instance);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::bindVertexBuffers(std::vector<HardwareBufferCRef> buffers,
                                          std::vector<core::Offset> offsets) {
        add<BindVertexBuffersCommand>(std::move(buffers), std::move(offsets));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::bindIndexBuffer(const HardwareBuffer &buffer,
                                        core::Offset offset,
                                        bool large_indices) {
        add<BindIndexBufferCommand>(buffer, offset, large_indices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::bindDescriptorSets(const GraphicsPipeline &pipeline,
                                           std::vector<DescriptorSetCRef> descriptor_sets,
                                           std::vector<core::UOffset> dynamic_offsets) {
        add<BindDescriptorSetsCommand>(&pipeline,
                                       std::move(descriptor_sets),
                                       std::move(dynamic_offsets));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::copyBuffer(const HardwareBuffer &source,
                                   const HardwareBuffer &destination,
                                   core::ArraySize size,
                                   core::UOffset src_offset,
                                   core::UOffset dst_offset) {
        add<CopyBufferCommand>(source, destination, size, src_offset, dst_offset);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::copyBufferToTexture(const HardwareBuffer &source,
                                            const Texture &destination,
                                            std::vector<BufferTextureCopy> buffer_texture_copies) {
        if (std::empty(buffer_texture_copies)) {
            buffer_texture_copies.emplace_back(BufferTextureCopy {
                0,
                0,
                0,
                {},
                { 0, 0, 0 },
                { destination.extent().w, destination.extent().h, destination.extent().d } });
        }

        add<CopyBufferToTextureCommand>(source, destination, std::move(buffer_texture_copies));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::copyTexture(const Texture &source,
                                    const Texture &destination,
                                    TextureLayout source_layout,
                                    TextureLayout destination_layout,
                                    TextureSubresourceLayers source_subresource_layers,
                                    TextureSubresourceLayers destination_subresource_layers,
                                    core::Extentu extent) {
        add<CopyTextureCommand>(source,
                                source_layout,
                                std::move(source_subresource_layers),
                                destination,
                                destination_layout,
                                std::move(destination_subresource_layers),
                                extent);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::resolveTexture(const Texture &source,
                                       const Texture &destination,
                                       TextureLayout source_layout,
                                       TextureLayout destination_layout,
                                       TextureSubresourceLayers source_subresource_layers,
                                       TextureSubresourceLayers destination_subresource_layers) {
        add<ResolveTextureCommand>(source,
                                   source_layout,
                                   std::move(source_subresource_layers),
                                   destination,
                                   destination_layout,
                                   std::move(destination_subresource_layers));
    }

    inline void CommandBuffer::blitTexture(const Texture &source,
                                           const Texture &destination,
                                           TextureLayout source_layout,
                                           TextureLayout destination_layout,
                                           std::vector<BlitRegion> regions,
                                           Filter filter) {
        add<BlitTextureCommand>(source,
                                source_layout,
                                destination,
                                destination_layout,
                                std::move(regions),
                                filter);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::transitionTextureLayout(const Texture &image,
                                                TextureLayout source_layout,
                                                TextureLayout destination_layout,
                                                TextureSubresourceRange subresource_range) {
        add<TransitionTextureLayoutCommand>(image,
                                            source_layout,
                                            destination_layout,
                                            std::move(subresource_range));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::executeSubCommandBuffers(std::vector<CommandBufferCRef> command_buffers) {
        add<ExecuteSubCommandBuffersCommand>(std::move(command_buffers));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::setViewport(core::UInt32 first_viewport, std::vector<Viewport> viewports) {
        add<SetViewportCommand>(first_viewport, std::move(viewports));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::setScissor(core::UInt32 first_scissor, std::vector<Scissor> scissors) {
        add<SetScissorCommand>(first_scissor, std::move(scissors));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::pipelineBarrier(PipelineStageFlag src_mask,
                                        PipelineStageFlag dst_mask,
                                        DependencyFlag dependency,
                                        MemoryBarriers memory_barriers,
                                        BufferMemoryBarriers buffer_memory_barriers,
                                        ImageMemoryBarriers image_memory_barriers) {
        add<PipelineBarrierCommand>(src_mask,
                                    dst_mask,
                                    dependency,
                                    std::move(memory_barriers),
                                    std::move(buffer_memory_barriers),
                                    std::move(image_memory_barriers));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    void CommandBuffer::pushConstants(const GraphicsPipeline &pipeline,
                                      ShaderStage stage,
                                      std::vector<std::byte> data) {
        add<PushConstantsCommand>(&pipeline, stage, std::move(data));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::CommandBuffer CommandBuffer::vkCommandBuffer() const noexcept {
        STORM_EXPECTS(m_vk_command_buffer);
        return *m_vk_command_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBuffer::operator vk::CommandBuffer() const noexcept { return vkCommandBuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::CommandBuffer CommandBuffer::vkHandle() const noexcept { return vkCommandBuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 CommandBuffer::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkCommandBuffer_T *());
    }
} // namespace storm::render
