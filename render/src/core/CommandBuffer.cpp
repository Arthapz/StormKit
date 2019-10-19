// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

static const auto old_layout_access_map =
    std::unordered_map<vk::ImageLayout, std::pair<vk::AccessFlags, vk::PipelineStageFlags>> {
        { vk::ImageLayout::eUndefined,
          { vk::AccessFlags {}, vk::PipelineStageFlagBits::eTopOfPipe } },
        { vk::ImageLayout::ePreinitialized,
          { vk::AccessFlags {}, vk::PipelineStageFlagBits::eTopOfPipe } },
        { vk::ImageLayout::eGeneral,
          { vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead,
            vk::PipelineStageFlagBits::eColorAttachmentOutput } },
        { vk::ImageLayout::eColorAttachmentOptimal,
          { vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead,
            vk::PipelineStageFlagBits::eColorAttachmentOutput } },
        { vk::ImageLayout::eDepthStencilAttachmentOptimal,
          { vk::AccessFlagBits::eDepthStencilAttachmentWrite |
                vk::AccessFlagBits::eDepthStencilAttachmentRead,
            vk::PipelineStageFlagBits::eLateFragmentTests } },
        { vk::ImageLayout::eDepthStencilReadOnlyOptimal,
          { vk::AccessFlagBits::eDepthStencilAttachmentRead,
            vk::PipelineStageFlagBits::eLateFragmentTests } },
        { vk::ImageLayout::eShaderReadOnlyOptimal,
          { vk::AccessFlagBits::eInputAttachmentRead,
            vk::PipelineStageFlagBits::eFragmentShader } },
        { vk::ImageLayout::eTransferSrcOptimal,
          { vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer } },
        { vk::ImageLayout::eTransferDstOptimal,
          { vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer } },
        { vk::ImageLayout::ePresentSrcKHR,
          { vk::AccessFlagBits::eMemoryRead, vk::PipelineStageFlagBits::eTransfer } }
    };

static const auto new_layout_access_map =
    std::unordered_map<vk::ImageLayout, std::pair<vk::AccessFlags, vk::PipelineStageFlags>> {
        { vk::ImageLayout::eUndefined, { vk::AccessFlags {}, {} } },
        { vk::ImageLayout::ePreinitialized, { vk::AccessFlags {}, {} } },
        { vk::ImageLayout::eGeneral,
          { vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead,
            vk::PipelineStageFlagBits::eVertexShader } },
        { vk::ImageLayout::eColorAttachmentOptimal,
          { vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead,
            vk::PipelineStageFlagBits::eColorAttachmentOutput } },
        { vk::ImageLayout::eDepthStencilAttachmentOptimal,
          { vk::AccessFlagBits::eDepthStencilAttachmentWrite |
                vk::AccessFlagBits::eDepthStencilAttachmentRead,
            vk::PipelineStageFlagBits::eEarlyFragmentTests } },
        { vk::ImageLayout::eDepthStencilReadOnlyOptimal,
          { vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eVertexInput } },
        { vk::ImageLayout::eShaderReadOnlyOptimal,
          { vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eFragmentShader } },
        { vk::ImageLayout::eTransferSrcOptimal,
          { vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer } },
        { vk::ImageLayout::eTransferDstOptimal,
          { vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer } },
        { vk::ImageLayout::ePresentSrcKHR,
          { vk::AccessFlagBits::eMemoryRead, vk::PipelineStageFlagBits::eTransfer } }
    };

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer::CommandBuffer(const render::Queue &queue,
                             render::CommandBufferLevel level,
                             RAIIVkCommandBuffer &&command_buffer)
    : m_queue { &queue }, m_level { level }, m_vk_command_buffer { std::move(command_buffer) } {
}

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer::~CommandBuffer() = default;

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer::CommandBuffer(CommandBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer &CommandBuffer::operator=(CommandBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void CommandBuffer::reset() noexcept {
    const auto &device = m_queue->device();

    m_vk_command_buffer->reset(vk::CommandBufferResetFlags {}, device.vkDispatcher());

    m_state = State::Initial;
}

/////////////////////////////////////
/////////////////////////////////////
void CommandBuffer::build() {
    STORM_EXPECTS(m_vk_command_buffer);
    STORM_EXPECTS(m_state == State::Recording);

    auto commands          = std::exchange(m_commands, std::queue<render::Command> {});
    const auto &device     = m_queue->device();
    const auto &dispatcher = device.vkDispatcher();

    while (!commands.empty()) {
        auto &command = commands.front();

        std::visit(
            core::overload {
                [this, &dispatcher](const BeginDebugRegionCommand &command) {
                    if (dispatcher.vkCmdBeginDebugUtilsLabelEXT &&
                        dispatcher.vkCmdEndDebugUtilsLabelEXT &&
                        dispatcher.vkCmdInsertDebugUtilsLabelEXT) {
                        const auto label_info = vk::DebugUtilsLabelEXT {}
                                                    .setPLabelName(std::data(command.name))
                                                    .setColor({ command.color.r,
                                                                command.color.g,
                                                                command.color.b,
                                                                command.color.a });

                        m_vk_command_buffer->beginDebugUtilsLabelEXT(label_info, dispatcher);
                    }
                },
                [this, &dispatcher](const InsertDebugLabelCommand &command) {
                    if (dispatcher.vkCmdBeginDebugUtilsLabelEXT &&
                        dispatcher.vkCmdEndDebugUtilsLabelEXT &&
                        dispatcher.vkCmdInsertDebugUtilsLabelEXT) {
                        const auto label_info = vk::DebugUtilsLabelEXT {}
                                                    .setPLabelName(std::data(command.name))
                                                    .setColor({ command.color.r,
                                                                command.color.g,
                                                                command.color.b,
                                                                command.color.a });

                        m_vk_command_buffer->insertDebugUtilsLabelEXT(label_info, dispatcher);
                    }
                },
                [this, &dispatcher]([[maybe_unused]] const EndDebugRegionCommand &command) {
                    if (dispatcher.vkCmdBeginDebugUtilsLabelEXT &&
                        dispatcher.vkCmdEndDebugUtilsLabelEXT &&
                        dispatcher.vkCmdInsertDebugUtilsLabelEXT) {
                        m_vk_command_buffer->endDebugUtilsLabelEXT(dispatcher);
                    }
                },
                [this, &dispatcher](const BeginCommand &command) {
                    const auto begin_info = vk::CommandBufferBeginInfo {}.setFlags(
                        (command.one_time_submit)
                            ? vk::CommandBufferUsageFlagBits::eOneTimeSubmit
                            : vk::CommandBufferUsageFlagBits::eSimultaneousUse);

                    m_vk_command_buffer->begin(begin_info, dispatcher);
                },
                [this, &dispatcher]([[maybe_unused]] const EndCommand &command) {
                    m_vk_command_buffer->end(dispatcher);
                },
                [this, &dispatcher](const BeginRenderPassCommand &command) {
                    const auto &render_pass = static_cast<const RenderPass &>(command.render_pass);
                    const auto &framebuffer = static_cast<const Framebuffer &>(command.framebuffer);

                    auto clear_values = std::vector<vk::ClearValue> {};
                    clear_values.reserve(std::size(command.clear_values));
                    for (const auto &clear_value_variant : command.clear_values) {
                        auto clear_value = vk::ClearValue {};

                        if (std::holds_alternative<ClearColor>(clear_value_variant)) {
                            const auto &clear_color = std::get<ClearColor>(clear_value_variant);

                            const auto clear_color_ =
                                vk::ClearColorValue {}.setFloat32({ clear_color.color.r,
                                                                    clear_color.color.g,
                                                                    clear_color.color.b,
                                                                    clear_color.color.a });

                            clear_value.setColor(std::move(clear_color_));
                        } else if (std::holds_alternative<ClearDepthStencil>(clear_value_variant)) {
                            const auto &clear_depth_stencil =
                                std::get<ClearDepthStencil>(clear_value_variant);

                            const auto clear_depth_stencil_ =
                                vk::ClearDepthStencilValue {}
                                    .setDepth(clear_depth_stencil.depth)
                                    .setStencil(clear_depth_stencil.stencil);

                            clear_value.setDepthStencil(std::move(clear_depth_stencil_));
                        }

                        clear_values.emplace_back(std::move(clear_value));
                    }

                    const auto render_pass_info =
                        vk::RenderPassBeginInfo {}
                            .setRenderArea(VkRect2D { .offset = { 0, 0 },
                                                      .extent = { framebuffer.extent().width,
                                                                  framebuffer.extent().height } })
                            .setRenderPass(render_pass)
                            .setFramebuffer(framebuffer)
                            .setClearValueCount(
                                gsl::narrow_cast<core::UInt32>(std::size(clear_values)))
                            .setPClearValues(std::data(clear_values));

                    auto subpass_content = vk::SubpassContents::eInline;
                    if (command.secondary_command_buffers)
                        subpass_content = vk::SubpassContents::eSecondaryCommandBuffers;

                    m_vk_command_buffer->beginRenderPass(render_pass_info,
                                                         subpass_content,
                                                         dispatcher);
                },
                [this, &dispatcher]([[maybe_unused]] const NextSubPassCommand &command) {
                    m_vk_command_buffer->nextSubpass(vk::SubpassContents::eInline, dispatcher);
                },
                [this, &dispatcher]([[maybe_unused]] const EndRenderPassCommand &command) {
                    m_vk_command_buffer->endRenderPass(dispatcher);
                },
                [this, &dispatcher](const BindGraphicsPipelineCommand &command) {
                    const auto &pipeline = command.pipeline;

                    m_vk_command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                      pipeline,
                                                      dispatcher);
                },
                [this, &dispatcher](const DrawCommand &command) {
                    STORM_EXPECTS(command.vertex_count > 0u);

                    m_vk_command_buffer->draw(command.vertex_count,
                                              command.instance_count,
                                              command.first_vertex,
                                              command.first_instance,
                                              dispatcher);
                },
                [this, &dispatcher](const DrawIndexedCommand &command) {
                    STORM_EXPECTS(command.index_count > 0u);

                    m_vk_command_buffer->drawIndexed(command.index_count,
                                                     command.instance_count,
                                                     command.first_index,
                                                     command.vertex_offset,
                                                     command.first_instance,
                                                     dispatcher);
                },
                [this, &dispatcher](const BindVertexBuffersCommand &command) {
                    STORM_EXPECTS(!std::empty(command.buffers));
                    STORM_EXPECTS(!std::empty(command.offsets));
                    STORM_EXPECTS(std::size(command.buffers) == std::size(command.offsets));

                    auto buffers = std::vector<vk::Buffer> {};
                    auto offsets = std::vector<vk::DeviceSize> {};

                    for (const auto &buffer : command.buffers) buffers.emplace_back(buffer.get());

                    for (const auto offset : command.offsets) offsets.emplace_back(offset);

                    m_vk_command_buffer->bindVertexBuffers(0, buffers, offsets, dispatcher);
                },
                [this, &dispatcher](const BindIndexBufferCommand &command) {
                    m_vk_command_buffer->bindIndexBuffer(command.buffer,
                                                         gsl::narrow_cast<vk::DeviceSize>(
                                                             command.offset),
                                                         (command.large_indices)
                                                             ? vk::IndexType::eUint32
                                                             : vk::IndexType::eUint16,
                                                         dispatcher);
                },
                [this, &dispatcher](const BindDescriptorSetsCommand &command) {
                    STORM_EXPECTS(!std::empty(command.descriptor_sets));

                    auto descriptor_sets = std::vector<vk::DescriptorSet> {};
                    for (const auto &descriptor_set : command.descriptor_sets)
                        descriptor_sets.emplace_back(descriptor_set.get());

                    const auto pipeline_layout = command.pipeline->vkPipelineLayout();

                    m_vk_command_buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                            pipeline_layout,
                                                            0,
                                                            descriptor_sets,
                                                            command.dynamic_offsets,
                                                            dispatcher);
                },
                [this, &dispatcher](const CopyBufferCommand &command) {
                    auto copy_buffer = vk::BufferCopy {}
                                           .setSize(command.size)
                                           .setSrcOffset(command.src_offset)
                                           .setDstOffset(command.dst_offset);

                    const auto copy_buffers = std::array { std::move(copy_buffer) };
                    m_vk_command_buffer->copyBuffer(command.source,
                                                    command.destination,
                                                    copy_buffers,
                                                    dispatcher);
                },

                [this, &dispatcher](const CopyBufferToTextureCommand &command) {
                    auto copy_regions = std::vector<vk::BufferImageCopy> {};
                    copy_regions.reserve(std::size(command.buffer_image_copies));

                    for (const auto &buffer_image_copy : command.buffer_image_copies) {
                        const auto image_subresource =
                            vk::ImageSubresourceLayers {}
                                .setAspectMask(
                                    toVK(buffer_image_copy.subresource_layers.aspect_mask))
                                .setMipLevel(buffer_image_copy.subresource_layers.mip_level)
                                .setBaseArrayLayer(
                                    buffer_image_copy.subresource_layers.base_array_layer)
                                .setLayerCount(buffer_image_copy.subresource_layers.layer_count);

                        const auto copy_buffer =
                            vk::BufferImageCopy {}
                                .setBufferOffset(buffer_image_copy.buffer_offset)
                                .setBufferRowLength(buffer_image_copy.buffer_row_length)
                                .setBufferImageHeight(buffer_image_copy.buffer_image_height)
                                .setImageSubresource(std::move(image_subresource))
                                .setImageOffset({ buffer_image_copy.offset.x,
                                                  buffer_image_copy.offset.y,
                                                  buffer_image_copy.offset.z })
                                .setImageExtent({ buffer_image_copy.extent.w,
                                                  buffer_image_copy.extent.h,
                                                  buffer_image_copy.extent.d });

                        copy_regions.emplace_back(std::move(copy_buffer));
                    }

                    m_vk_command_buffer->copyBufferToImage(command.source,
                                                           command.destination.vkImage(),
                                                           vk::ImageLayout::eTransferDstOptimal,
                                                           copy_regions,
                                                           dispatcher);
                },
                [this, &dispatcher](const CopyTextureCommand &command) {
                    const auto extent =
                        vk::Extent3D { command.source.extent().w, command.source.extent().h, 1 };

                    const auto src_subresource =
                        vk::ImageSubresourceLayers {}
                            .setAspectMask(toVK(command.source_subresource_layers.aspect_mask))
                            .setMipLevel(command.source_subresource_layers.mip_level)
                            .setBaseArrayLayer(command.source_subresource_layers.base_array_layer)
                            .setLayerCount(command.source_subresource_layers.layer_count);

                    const auto dst_subresource =
                        vk::ImageSubresourceLayers {}
                            .setAspectMask(toVK(command.destination_subresource_layers.aspect_mask))
                            .setMipLevel(command.destination_subresource_layers.mip_level)
                            .setBaseArrayLayer(
                                command.destination_subresource_layers.base_array_layer)
                            .setLayerCount(command.destination_subresource_layers.layer_count);

                    const auto region = vk::ImageCopy {}
                                            .setSrcSubresource(std::move(src_subresource))
                                            .setSrcOffset(vk::Offset3D { 0, 0, 0 })
                                            .setDstSubresource(std::move(dst_subresource))
                                            .setDstOffset(vk::Offset3D { 0, 0, 0 })
                                            .setExtent(std::move(extent));

                    const auto regions = std::array { std::move(region) };
                    m_vk_command_buffer->copyImage(command.source,
                                                   toVK(command.source_layout),
                                                   command.destination,
                                                   toVK(command.destination_layout),
                                                   regions,
                                                   dispatcher);
                },
                [this, &dispatcher](const ResolveTextureCommand &command) {
                    const auto extent =
                        vk::Extent3D { command.source.extent().w, command.source.extent().h, 1 };

                    const auto src_subresource =
                        vk::ImageSubresourceLayers {}
                            .setAspectMask(toVK(command.source_subresource_layers.aspect_mask))
                            .setMipLevel(command.source_subresource_layers.mip_level)
                            .setBaseArrayLayer(command.source_subresource_layers.base_array_layer)
                            .setLayerCount(command.source_subresource_layers.layer_count);

                    const auto dst_subresource =
                        vk::ImageSubresourceLayers {}
                            .setAspectMask(toVK(command.destination_subresource_layers.aspect_mask))
                            .setMipLevel(command.destination_subresource_layers.mip_level)
                            .setBaseArrayLayer(
                                command.destination_subresource_layers.base_array_layer)
                            .setLayerCount(command.destination_subresource_layers.layer_count);

                    const auto region = vk::ImageResolve {}
                                            .setSrcSubresource(std::move(src_subresource))
                                            .setSrcOffset(vk::Offset3D { 0, 0, 0 })
                                            .setDstSubresource(std::move(dst_subresource))
                                            .setDstOffset(vk::Offset3D { 0, 0, 0 })
                                            .setExtent(std::move(extent));

                    const auto regions = std::array { std::move(region) };
                    m_vk_command_buffer->resolveImage(command.source,
                                                      toVK(command.source_layout),
                                                      command.destination,
                                                      toVK(command.destination_layout),
                                                      regions,
                                                      dispatcher);
                },
                [this, &dispatcher](const TransitionTextureLayoutCommand &command) {
                    const auto src_layout = toVK(command.source_layout);
                    const auto dst_layout = toVK(command.destination_layout);

                    const auto &src_access = old_layout_access_map.find(src_layout);
                    const auto &dst_access = new_layout_access_map.find(dst_layout);

                    const auto src_stage = src_access->second.second;
                    const auto dst_stage = dst_access->second.second;

                    const auto subresource_range =
                        vk::ImageSubresourceRange {}
                            .setAspectMask(toVK(command.subresource_range.aspect_mask))
                            .setBaseMipLevel(command.subresource_range.base_mip_level)
                            .setLevelCount(command.subresource_range.level_count)
                            .setBaseArrayLayer(command.subresource_range.base_array_layer)
                            .setLayerCount(command.subresource_range.layer_count);

                    const auto barrier = vk::ImageMemoryBarrier {}
                                             .setSrcAccessMask(src_access->second.first)
                                             .setDstAccessMask(dst_access->second.first)
                                             .setOldLayout(src_layout)
                                             .setNewLayout(dst_layout)
                                             .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                             .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                                             .setImage(command.texture)
                                             .setSubresourceRange(std::move(subresource_range));

                    const auto barriers = std::array { std::move(barrier) };
                    const auto mem      = std::array<vk::MemoryBarrier, 0> {};
                    const auto buf      = std::array<vk::BufferMemoryBarrier, 0> {};
                    m_vk_command_buffer
                        ->pipelineBarrier(src_stage, dst_stage, {}, mem, buf, barriers, dispatcher);
                },
                [this, &dispatcher](const ExecuteSubCommandBuffersCommand &command) {
                    auto vk_command_buffers = std::vector<vk::CommandBuffer> {};
                    vk_command_buffers.reserve(std::size(command.command_buffers));

                    for (const auto &cmb : command.command_buffers) {
                        STORM_EXPECTS(cmb.get().level() == render::CommandBufferLevel::Secondary);
                        vk_command_buffers.emplace_back(cmb.get());
                    }

                    m_vk_command_buffer->executeCommands(vk_command_buffers, dispatcher);
                },
                [this, &dispatcher](const SetScissorCommand &command) {
                    auto scissors = std::vector<vk::Rect2D> {};
                    scissors.reserve(std::size(command.scissors));

                    for (const auto &scissor : command.scissors) {
                        scissors.emplace_back(
                            vk::Rect2D { vk::Offset2D { scissor.offset.x, scissor.offset.y },
                                         { scissor.extent.w, scissor.extent.h } });
                    }

                    m_vk_command_buffer->setScissor(command.first_scissor, scissors, dispatcher);
                },
                [this, &dispatcher](const PipelineBarrierCommand &command) {
                    auto memory_barriers = std::vector<vk::MemoryBarrier> {};
                    memory_barriers.reserve(std::size(command.memory_barriers));

                    for (const auto &barrier : command.memory_barriers) {
                        const auto vk_barrier = vk::MemoryBarrier {}
                                                    .setSrcAccessMask(toVK(barrier.src))
                                                    .setDstAccessMask(toVK(barrier.dst));

                        memory_barriers.emplace_back(std::move(vk_barrier));
                    }

                    auto buffer_memory_barriers = std::vector<vk::BufferMemoryBarrier> {};
                    buffer_memory_barriers.reserve(std::size(command.buffer_memory_barriers));

                    for (const auto &barrier : command.buffer_memory_barriers) {
                        const auto vk_barrier =
                            vk::BufferMemoryBarrier {}
                                .setSrcAccessMask(toVK(barrier.src))
                                .setDstAccessMask(toVK(barrier.dst))
                                .setSrcQueueFamilyIndex(barrier.src_queue_family_index)
                                .setDstQueueFamilyIndex(barrier.dst_queue_family_index)
                                .setBuffer(barrier.buffer)
                                .setOffset(barrier.offset)
                                .setSize(barrier.size);

                        buffer_memory_barriers.emplace_back(std::move(vk_barrier));
                    }

                    auto image_memory_barriers = std::vector<vk::ImageMemoryBarrier> {};
                    image_memory_barriers.reserve(std::size(command.image_memory_barriers));

                    for (const auto &barrier : command.image_memory_barriers) {
                        const auto vk_subresource_range =
                            vk::ImageSubresourceRange {}
                                .setAspectMask(toVK(barrier.range.aspect_mask))
                                .setBaseMipLevel(barrier.range.base_mip_level)
                                .setLevelCount(barrier.range.level_count)
                                .setBaseArrayLayer(barrier.range.base_array_layer)
                                .setLayerCount(barrier.range.layer_count);

                        const auto vk_barrier =
                            vk::ImageMemoryBarrier {}
                                .setSrcAccessMask(toVK(barrier.src))
                                .setDstAccessMask(toVK(barrier.dst))
                                .setSrcQueueFamilyIndex(barrier.src_queue_family_index)
                                .setDstQueueFamilyIndex(barrier.dst_queue_family_index)
                                .setOldLayout(toVK(barrier.old_layout))
                                .setNewLayout(toVK(barrier.new_layout))
                                .setImage(barrier.texture)
                                .setSubresourceRange(std::move(vk_subresource_range));

                        image_memory_barriers.emplace_back(std::move(vk_barrier));
                    }

                    m_vk_command_buffer->pipelineBarrier(toVK(command.src_mask),
                                                         toVK(command.dst_mask),
                                                         toVK(command.dependency),
                                                         memory_barriers,
                                                         buffer_memory_barriers,
                                                         image_memory_barriers,
                                                         dispatcher);
                },
                [this, &dispatcher](const PushConstantsCommand &command) {
                    STORM_EXPECTS(command.pipeline != nullptr);
                    STORM_EXPECTS(!std::empty(command.data));

                    m_vk_command_buffer->pushConstants(command.pipeline->vkPipelineLayout(),
                                                       toVK(command.stage),
                                                       0u,
                                                       std::size(command.data),
                                                       std::data(command.data),
                                                       dispatcher);
                } },
            command);

        commands.pop();
    }

    m_state = State::Executable;
}
