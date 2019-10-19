// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    inline CommandBuffer::State CommandBuffer::state() const noexcept {
        return m_state;
    }
    inline CommandBufferLevel CommandBuffer::level() const noexcept {
        return m_level;
    }

    template <typename CommandT>
    inline void CommandBuffer::add(CommandT &&command) {
        STORM_EXPECTS(m_state == State::Initial || m_state == State::Recording);

        if (m_state == State::Initial)
            m_state = State::Recording;

        m_commands.emplace(std::forward<CommandT>(command));
    }

    template <typename CommandT, typename... Args>
    inline void CommandBuffer::add(Args &&... args) {
        STORM_EXPECTS(m_state == State::Initial || m_state == State::Recording);

        if (m_state == State::Initial)
            m_state = State::Recording;

        m_commands.emplace(CommandT{std::forward<Args>(args)...});
    }

    inline void CommandBuffer::begin(bool one_time_submit,
                                     std::optional<CommandBufferCRef> parent) {
        add<BeginCommand>(one_time_submit, parent);
    }

    inline void CommandBuffer::end() {
        add<EndCommand>();
    }

    inline void CommandBuffer::beginRenderPass(
        const RenderPass &render_pass, const Framebuffer &framebuffer,
        BeginRenderPassCommand::ClearValues clear_values) {
        add<BeginRenderPassCommand>(render_pass, framebuffer,
                                    std::move(clear_values));
    }

    inline void CommandBuffer::endRenderPass() {
        add<EndRenderPassCommand>();
    }

    inline void
        CommandBuffer::bindGraphicsPipeline(const GraphicsPipeline &pipeline) {
        add<BindGraphicsPipelineCommand>(pipeline);
    }

    inline void CommandBuffer::draw(std::uint32_t vertex_count,
                                    std::uint32_t instance_count,
                                    std::uint32_t first_vertex,
                                    std::uint32_t first_instance) {
        add<DrawCommand>(vertex_count, instance_count, first_vertex,
                         first_instance);
    }

    inline void CommandBuffer::drawIndexed(std::uint32_t index_count,
                                           std::uint32_t instance_count,
                                           std::uint32_t first_index,
                                           std::int32_t vertex_offset,
                                           std::uint32_t first_instance) {
        add<DrawIndexedCommand>(index_count, instance_count, first_index,
                                vertex_offset, first_instance);
    }

    inline void CommandBuffer::bindVertexBuffers(
        std::vector<HardwareBufferCRef> buffers,
        std::vector<std::ptrdiff_t> offsets) {
        add<BindVertexBuffersCommand>(std::move(buffers), std::move(offsets));
    }

    inline void CommandBuffer::bindIndexBuffer(const HardwareBuffer &buffer,
                                               std::ptrdiff_t offset,
                                               bool large_indices) {
        add<BindIndexBufferCommand>(buffer, offset, large_indices);
    }

    inline void CommandBuffer::copyBuffer(const HardwareBuffer &source,
                                          const HardwareBuffer &destination,
                                          std::size_t size,
                                          std::ptrdiff_t src_offset,
                                          std::ptrdiff_t dst_offset) {
        add<CopyBufferCommand>(source, destination, size, src_offset,
                               dst_offset);
    }

    inline void CommandBuffer::bindDescriptorSets(
        const GraphicsPipeline &pipeline,
        std::vector<DescriptorSetCRef> descriptor_sets) {
        add<BindDescriptorSetsCommand>(&pipeline, std::move(descriptor_sets));
    }

    inline void CommandBuffer::copyBufferToImage(const HardwareBuffer &source,
                                                 const Texture &destination) {
        add<CopyBufferToImageCommand>(source, destination);
    }

    inline void CommandBuffer::transitionImageLayout(Texture &image,
                                                     ImageLayout new_layout) {
        add<TransitionImageLayoutCommand>(image, new_layout);
    }

    inline void CommandBuffer::executeSubCommandBuffers(
        std::vector<CommandBufferCRef> command_buffers) {
        add<ExecuteSubCommandBuffersCommand>(std::move(command_buffers));
    }

    inline void CommandBuffer::copyImage(
        const Texture &source, ImageLayout source_layout,
        ImageAspectMask source_aspect_mask, const Texture &destination,
        ImageLayout destination_layout,
        ImageAspectMask destination_aspect_mask) {
        add<CopyImageCommand>(source, source_layout, source_aspect_mask,
                              destination, destination_layout,
                              destination_aspect_mask);
    }

    inline VkCommandBuffer CommandBuffer::vkCommandBuffer() const noexcept {
        STORM_EXPECTS(m_vk_command_buffer != VK_NULL_HANDLE);
        return m_vk_command_buffer;
    }

    inline CommandBuffer::operator VkCommandBuffer() const noexcept {
        STORM_EXPECTS(m_vk_command_buffer != VK_NULL_HANDLE);
        return m_vk_command_buffer;
    }

} // namespace storm::render
