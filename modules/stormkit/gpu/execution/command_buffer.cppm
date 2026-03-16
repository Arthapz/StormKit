// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.execution:command_buffer;

import std;

import stormkit.core;
import stormkit.gpu.core;
import stormkit.gpu.resource;

import :descriptors;
import :render_pass;
import :pipeline;
import :swapchain;

namespace stdr = std::ranges;

namespace cmonadic = stormkit::core::monadic;

export namespace stormkit::gpu {
    class SwapChain;
    class Queue;
    class CommandBuffer;
    class CommandPool;

    namespace view {
        class Queue;
        class CommandBuffer;
        class CommandPool;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<Queue> {
            using Of          = Queue;
            using ElementType = VkQueue;
            using DeleterType = decltype(cmonadic::noop());
            using ViewType    = view::Queue;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::QUEUE;
        };

        template<>
        struct ObjectInfo<CommandBuffer> {
            using Of          = CommandBuffer;
            using ElementType = VkCommandBuffer;
            using DeleterType = decltype(cmonadic::noop());
            using ViewType    = view::CommandBuffer;
            using OwnedBy     = Device;

            static constexpr auto DISABLE_CREATE_ALLOCATE = true;

            static constexpr auto DEBUG_TYPE = DebugObjectType::COMMAND_BUFFER;
        };

        template<>
        struct ObjectInfo<CommandPool> {
            using Of          = CommandPool;
            using ElementType = VkCommandPool;
            using DeleterType = PFN_vkDestroyCommandPool VolkDeviceTable::*;
            using ViewType    = view::CommandPool;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::COMMAND_POOL;
        };
    } // namespace meta

    class STORMKIT_GPU_API Queue: public OwnedByDevice<Queue> {
      public:
        struct SubmitInfo {
            std::span<const view::Semaphore>     wait_semaphores   = {};
            std::span<const PipelineStageFlag>   wait_dst_stages   = {};
            std::span<const view::CommandBuffer> command_buffers   = {};
            std::span<const view::Semaphore>     signal_semaphores = {};
        };

        ~Queue() noexcept;

        Queue(const Queue&)          = delete;
        auto operator=(const Queue&) = delete;

        Queue(Queue&&) noexcept;
        auto operator=(Queue&&) noexcept -> Queue&;

        auto wait_idle() const noexcept -> Expected<void>;

        auto submit(std::span<const SubmitInfo> submit_infos, std::optional<view::Fence> fence = std::nullopt) const noexcept
          -> Expected<void>;

        auto submit(const SubmitInfo& submit_info, std::optional<view::Fence> fence = std::nullopt) const noexcept
          -> Expected<void>;

        [[nodiscard]]
        auto present(std::span<const view::SwapChain> swapchains,
                     std::span<const view::Semaphore> wait_semaphores,
                     std::span<const u32>             image_indices) const noexcept -> Expected<Result>;

        [[nodiscard]]
        auto entry() const noexcept -> const Device::QueueEntry&;

        // clang-format off
  // private:
        // clang-format on
        Queue(PrivateTag, view::Device&& device) noexcept;
        auto do_init(const Device::QueueEntry&) -> void;

      private:
        Device::QueueEntry m_entry;
    };

    namespace view {
        class Queue: DeviceObject<gpu::Queue> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Buffer>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            Queue(const gpu::Queue& of) noexcept;
            template<cmeta::ContainedOrPointerOf<gpu::Queue> T>
            Queue(const T& of) noexcept;
            ~Queue() noexcept;

            Queue(const Queue&) noexcept;
            auto operator=(const Queue&) noexcept -> Queue&;

            Queue(Queue&&) noexcept;
            auto operator=(Queue&&) noexcept -> Queue&;

            auto wait_idle() const noexcept -> Expected<void>;

            auto submit(std::span<const gpu::Queue::SubmitInfo> submit_infos,
                        std::optional<Fence>                    fence = std::nullopt) const noexcept -> Expected<void>;

            auto submit(const gpu::Queue::SubmitInfo& submit_info, std::optional<Fence> fence = std::nullopt) const noexcept
              -> Expected<void>;

            [[nodiscard]]
            auto present(std::span<const SwapChain> swapchains,
                         std::span<const Semaphore> wait_semaphores,
                         std::span<const u32>       image_indices) const noexcept -> Expected<Result>;

            [[nodiscard]]
            auto entry() const noexcept -> const gpu::Device::QueueEntry&;
        };
    } // namespace view

    struct RenderPassInheritanceInfo {
        std::optional<view::RenderPass>  render_pass = std::nullopt;
        u32                              subpass     = 0;
        std::optional<view::FrameBuffer> framebuffer = std::nullopt;
    };

    struct RenderingInheritanceInfo {
        u32                        view_mask             = 0;
        std::vector<PixelFormat>   color_attachments     = {};
        std::optional<PixelFormat> depth_attachment      = std::nullopt;
        std::optional<PixelFormat> stencil_attachment    = std::nullopt;
        SampleCountFlag            rasterization_samples = SampleCountFlag::C1;
    };

    using InheritanceInfo = std::variant<std::monostate, RenderPassInheritanceInfo, RenderingInheritanceInfo>;

    struct RenderingInfo {
        struct Attachment {
            struct Resolve {
                gpu::view::ImageView image_view;
                ResolveModeFlag      mode;
                gpu::ImageLayout     layout = ImageLayout::ATTACHMENT_OPTIMAL;
            };

            Ref<const gpu::ImageView> image_view;
            gpu::ImageLayout          layout = ImageLayout::ATTACHMENT_OPTIMAL;

            std::optional<Resolve> resolve = std::nullopt;

            AttachmentLoadOperation  load_op  = AttachmentLoadOperation::CLEAR;
            AttachmentStoreOperation store_op = AttachmentStoreOperation::STORE;

            std::optional<ClearValue> clear_value = std::nullopt;
        };

        math::irect render_area;
        u32         layer_count = 1u;
        u32         view_mask   = 0u;

        std::vector<Attachment>   color_attachments  = {};
        std::optional<Attachment> depth_attachment   = std::nullopt;
        std::optional<Attachment> stencil_attachment = std::nullopt;
    };

    class STORMKIT_GPU_API CommandBuffer: public OwnedByDevice<CommandBuffer> {
      public:
        enum class State {
            INITIAL,
            RECORDING,
            EXECUTABLE,
        };

        using RecordClosure = FunctionRef<Expected<void>(CommandBuffer&)>;
        using Deleter       = std::function<void(view::Device, view::CommandPool, VkCommandBuffer)>;
        ~CommandBuffer() noexcept;

        CommandBuffer(const CommandBuffer&)                    = delete;
        auto operator=(const CommandBuffer&) -> CommandBuffer& = delete;

        CommandBuffer(CommandBuffer&&) noexcept;
        auto operator=(CommandBuffer&&) noexcept -> CommandBuffer&;

        [[nodiscard]]
        auto state() const noexcept -> State;
        [[nodiscard]]
        auto level() const noexcept -> CommandBufferLevel;

        auto record(RecordClosure   record_closure,
                    bool            one_time_submit  = false,
                    InheritanceInfo inheritance_info = std::monostate {}) noexcept -> Expected<void>;

        auto reset() noexcept -> Expected<void>;
        auto begin(bool one_time_submit = false, InheritanceInfo inheritance_info = std::monostate {}) noexcept -> Expected<void>;
        auto end() noexcept -> Expected<void>;

        auto begin_debug_region(std::string_view name, const fcolor_rgb& color = colors::WHITE<f32>) const noexcept
          -> const CommandBuffer&;
        auto insert_debug_label(std::string_view name, const fcolor_rgb& color = colors::WHITE<f32>) const noexcept
          -> const CommandBuffer&;
        auto end_debug_region() const noexcept -> const CommandBuffer&;

        auto begin_rendering(const RenderingInfo& info, bool secondary_commandbuffers = false) const noexcept
          -> const CommandBuffer&;
        auto begin_render_pass(view::RenderPass            render_pass,
                               view::FrameBuffer           framebuffer,
                               std::span<const ClearValue> clear_values = std::array { ClearValue {
                                 ClearColor { .color = colors::SILVER<f32> } } },
                               bool secondary_commandbuffers            = false) const noexcept -> const CommandBuffer&;
        auto next_sub_pass() const noexcept -> const CommandBuffer&;
        auto end_render_pass() const noexcept -> const CommandBuffer&;
        auto end_rendering() const noexcept -> const CommandBuffer&;

        auto bind_pipeline(const Pipeline& pipeline) const noexcept -> const CommandBuffer&;
        auto set_viewport(u32 first_viewport, std::span<const Viewport> viewports) const noexcept -> const CommandBuffer&;
        auto set_scissor(u32 first_scissor, std::span<const Scissor> scissors) const noexcept -> const CommandBuffer&;
        auto set_line_width(f32 width) const noexcept -> const CommandBuffer&;
        auto set_depth_bias(f32 constant_factor, f32 clamp, f32 slope_factor) const noexcept -> const CommandBuffer&;
        auto set_blend_constants(std::span<const f32> constants) const noexcept -> const CommandBuffer&;
        auto set_depth_bounds(f32 min, f32 max) noexcept -> CommandBuffer&;
        auto set_stencil_compare_mask(StencilFaceFlag face, u32 mask) const noexcept -> const CommandBuffer&;
        auto set_stencil_write_mask(StencilFaceFlag face, u32 mask) const noexcept -> const CommandBuffer&;
        auto set_stencil_reference(StencilFaceFlag face, u32 reference) const noexcept -> const CommandBuffer&;

        auto dispatch(u32 group_count_x, u32 group_count_y, u32 group_count_z) const noexcept -> const CommandBuffer&;

        auto draw(u32 vertex_count, u32 instance_count = 1u, u32 first_vertex = 0, u32 first_instance = 0) const noexcept
          -> const CommandBuffer&;
        auto draw_indexed(u32 index_count,
                          u32 instance_count = 1u,
                          u32 first_index    = 0u,
                          i32 vertex_offset  = 0,
                          u32 first_instance = 0u) noexcept -> CommandBuffer&;
        auto draw_indirect(view::Buffer buffer, usize offset, u32 draw_count, u32 stride) const noexcept -> const CommandBuffer&;
        auto draw_indexed_indirect(view::Buffer buffer, usize offset, u32 draw_count, u32 stride) const noexcept
          -> const CommandBuffer&;

        auto bind_vertex_buffers(std::span<const view::Buffer> buffers, std::span<const u64> offsets) const noexcept
          -> const CommandBuffer&;
        auto bind_index_buffer(view::Buffer buffer, u64 offset = 0, bool large_indices = false) const noexcept
          -> const CommandBuffer&;
        auto bind_descriptor_sets(view::Pipeline                       pipeline,
                                  view::PipelineLayout                 layout,
                                  std::span<const view::DescriptorSet> descriptor_sets,
                                  std::span<const u32> dynamic_offsets = {}) const noexcept -> const CommandBuffer&;

        auto copy_buffer(view::Buffer src, view::Buffer dst, usize size, u64 src_offset = 0u, u64 dst_offset = 0u) const noexcept
          -> const CommandBuffer&;
        auto copy_buffer_to_image(view::Buffer src, view::Image dst, std::span<const BufferImageCopy> buffer_image_copies = {})
          const noexcept -> const CommandBuffer&;
        auto copy_image_to_buffer(view::Image src, view::Buffer dst, std::span<const BufferImageCopy> buffer_image_copies = {})
          const noexcept -> const CommandBuffer&;
        auto copy_image(view::Image                   src,
                        view::Image                   dst,
                        ImageLayout                   src_layout,
                        ImageLayout                   dst_layout,
                        const ImageSubresourceLayers& src_subresource_layers,
                        const ImageSubresourceLayers& dst_subresource_layers,
                        const math::uextent3&         extent) const noexcept -> const CommandBuffer&;

        auto resolve_image(view::Image                   src,
                           view::Image                   dst,
                           ImageLayout                   src_layout,
                           ImageLayout                   dst_layout,
                           const ImageSubresourceLayers& src_subresource_layers = {},
                           const ImageSubresourceLayers& dst_subresource_layers = {}) const noexcept -> const CommandBuffer&;

        auto blit_image(view::Image                 src,
                        view::Image                 dst,
                        ImageLayout                 src_layout,
                        ImageLayout                 dst_layout,
                        std::span<const BlitRegion> regions,
                        Filter                      filter) const noexcept -> const CommandBuffer&;

        auto transition_image_layout(view::Image                  image,
                                     ImageLayout                  src_layout,
                                     ImageLayout                  dst_layout,
                                     const ImageSubresourceRange& subresource_range = {}) const noexcept -> const CommandBuffer&;

        auto execute_sub_command_buffers(std::span<const view::CommandBuffer> commandbuffers) const noexcept
          -> const CommandBuffer&;

        auto pipeline_barrier(PipelineStageFlag                    src_mask,
                              PipelineStageFlag                    dst_mask,
                              DependencyFlag                       dependency,
                              std::span<const MemoryBarrier>       memory_barriers,
                              std::span<const BufferMemoryBarrier> buffer_memory_barriers,
                              std::span<const ImageMemoryBarrier>  image_memory_barriers) const noexcept -> const CommandBuffer&;

        auto push_constants(view::PipelineLayout  pipeline_layout,
                            ShaderStageFlag       stage,
                            std::span<const Byte> data,
                            u32                   offset = 0u) const noexcept -> const CommandBuffer&;

        auto submit(view::Queue                        queue,
                    std::span<view::Semaphore>         wait_semaphores   = {},
                    std::span<const PipelineStageFlag> wait_dst_stages   = {},
                    std::span<view::Semaphore>         signal_semaphores = {},
                    std::optional<view::Fence>         fence             = std::nullopt) const noexcept -> Expected<void>;

        CommandBuffer(PrivateTag, view::Device&&);
        auto do_init(PrivateTag, view::CommandPool&&, CommandBufferLevel, VkCommandBuffer&&, Deleter&&) noexcept
          -> Expected<void>;

      private:
        static auto create(view::Device, view::CommandPool, CommandBufferLevel, VkCommandBuffer&&, Deleter&&) noexcept
          -> CommandBuffer;
        static auto allocate(view::Device, view::CommandPool, CommandBufferLevel, VkCommandBuffer&&, Deleter&&) noexcept
          -> Heap<CommandBuffer>;

        CommandBufferLevel m_level = CommandBufferLevel::PRIMARY;

        Deleter m_deleter;

        State m_state = State::INITIAL;

        friend class CommandPool;
    };

    namespace view {
        class STORMKIT_GPU_API CommandBuffer: public OwnedByDevice<gpu::CommandBuffer> {
          public:
            CommandBuffer(const gpu::CommandBuffer& of) noexcept;
            template<cmeta::ContainedOrPointerOf<gpu::CommandBuffer> T>
            CommandBuffer(const T& of) noexcept;
            ~CommandBuffer() noexcept;

            CommandBuffer(const CommandBuffer&) noexcept;
            auto operator=(const CommandBuffer&) noexcept -> CommandBuffer&;

            CommandBuffer(CommandBuffer&&) noexcept;
            auto operator=(CommandBuffer&&) noexcept -> CommandBuffer&;

            [[nodiscard]]
            auto state() const noexcept -> State;
            [[nodiscard]]
            auto level() const noexcept -> CommandBufferLevel;

            auto record(RecordClosure   record_closure,
                        bool            one_time_submit  = false,
                        InheritanceInfo inheritance_info = std::monostate {}) noexcept -> Expected<void>;

            auto reset() noexcept -> Expected<void>;
            auto begin(bool one_time_submit = false, InheritanceInfo inheritance_info = std::monostate {}) noexcept
              -> Expected<void>;
            auto end() noexcept -> Expected<void>;

            auto begin_debug_region(std::string_view name, const fcolor_rgb& color = colors::WHITE<f32>) const noexcept
              -> const CommandBuffer&;
            auto insert_debug_label(std::string_view name, const fcolor_rgb& color = colors::WHITE<f32>) const noexcept
              -> const CommandBuffer&;
            auto end_debug_region() const noexcept -> const CommandBuffer&;

            auto begin_rendering(const RenderingInfo& info, bool secondary_commandbuffers = false) const noexcept
              -> const CommandBuffer&;
            auto begin_render_pass(view::RenderPass            render_pass,
                                   view::FrameBuffer           framebuffer,
                                   std::span<const ClearValue> clear_values = std::array { ClearValue {
                                     ClearColor { .color = colors::SILVER<f32> } } },
                                   bool secondary_commandbuffers            = false) const noexcept -> const CommandBuffer&;
            auto next_sub_pass() const noexcept -> const CommandBuffer&;
            auto end_render_pass() const noexcept -> const CommandBuffer&;
            auto end_rendering() const noexcept -> const CommandBuffer&;

            auto bind_pipeline(const Pipeline& pipeline) const noexcept -> const CommandBuffer&;
            auto set_viewport(u32 first_viewport, std::span<const Viewport> viewports) const noexcept -> const CommandBuffer&;
            auto set_scissor(u32 first_scissor, std::span<const Scissor> scissors) const noexcept -> const CommandBuffer&;
            auto set_line_width(f32 width) const noexcept -> const CommandBuffer&;
            auto set_depth_bias(f32 constant_factor, f32 clamp, f32 slope_factor) const noexcept -> const CommandBuffer&;
            auto set_blend_constants(std::span<const f32> constants) const noexcept -> const CommandBuffer&;
            auto set_depth_bounds(f32 min, f32 max) noexcept -> CommandBuffer&;
            auto set_stencil_compare_mask(StencilFaceFlag face, u32 mask) const noexcept -> const CommandBuffer&;
            auto set_stencil_write_mask(StencilFaceFlag face, u32 mask) const noexcept -> const CommandBuffer&;
            auto set_stencil_reference(StencilFaceFlag face, u32 reference) const noexcept -> const CommandBuffer&;

            auto dispatch(u32 group_count_x, u32 group_count_y, u32 group_count_z) const noexcept -> const CommandBuffer&;

            auto draw(u32 vertex_count, u32 instance_count = 1u, u32 first_vertex = 0, u32 first_instance = 0) const noexcept
              -> const CommandBuffer&;
            auto draw_indexed(u32 index_count,
                              u32 instance_count = 1u,
                              u32 first_index    = 0u,
                              i32 vertex_offset  = 0,
                              u32 first_instance = 0u) const noexcept -> const CommandBuffer&;
            auto draw_indirect(view::Buffer buffer, usize offset, u32 draw_count, u32 stride) const noexcept
              -> const CommandBuffer&;
            auto draw_indexed_indirect(view::Buffer buffer, usize offset, u32 draw_count, u32 stride) const noexcept
              -> const CommandBuffer&;

            auto bind_vertex_buffers(std::span<const view::Buffer> buffers, std::span<const u64> offsets) const noexcept
              -> const CommandBuffer&;
            auto bind_index_buffer(view::Buffer buffer, u64 offset = 0, bool large_indices = false) const noexcept
              -> const CommandBuffer&;
            auto bind_descriptor_sets(view::Pipeline                       pipeline,
                                      view::PipelineLayout                 layout,
                                      std::span<const view::DescriptorSet> descriptor_sets,
                                      std::span<const u32> dynamic_offsets = {}) const noexcept -> const CommandBuffer&;

            auto copy_buffer(view::Buffer src, view::Buffer dst, usize size, u64 src_offset = 0u, u64 dst_offset = 0u) noexcept
              -> const CommandBuffer&;
            auto copy_buffer_to_image(view::Buffer                     src,
                                      view::Image                      dst,
                                      std::span<const BufferImageCopy> buffer_image_copies = {}) const noexcept
              -> const CommandBuffer&;
            auto copy_image_to_buffer(view::Image                      src,
                                      view::Buffer                     dst,
                                      std::span<const BufferImageCopy> buffer_image_copies = {}) const noexcept
              -> const CommandBuffer&;
            auto copy_image(view::Image                   src,
                            view::Image                   dst,
                            ImageLayout                   src_layout,
                            ImageLayout                   dst_layout,
                            const ImageSubresourceLayers& src_subresource_layers,
                            const ImageSubresourceLayers& dst_subresource_layers,
                            const math::uextent3&         extent) const noexcept -> const CommandBuffer&;

            auto resolve_image(view::Image                   src,
                               view::Image                   dst,
                               ImageLayout                   src_layout,
                               ImageLayout                   dst_layout,
                               const ImageSubresourceLayers& src_subresource_layers = {},
                               const ImageSubresourceLayers& dst_subresource_layers = {}) const noexcept -> const CommandBuffer&;

            auto blit_image(view::Image                 src,
                            view::Image                 dst,
                            ImageLayout                 src_layout,
                            ImageLayout                 dst_layout,
                            std::span<const BlitRegion> regions,
                            Filter                      filter) const noexcept -> const CommandBuffer&;

            auto transition_image_layout(view::Image                  image,
                                         ImageLayout                  src_layout,
                                         ImageLayout                  dst_layout,
                                         const ImageSubresourceRange& subresource_range = {}) const noexcept
              -> const CommandBuffer&;

            auto execute_sub_command_buffers(std::span<const view::CommandBuffer> commandbuffers) const noexcept
              -> const CommandBuffer&;

            auto pipeline_barrier(PipelineStageFlag                    src_mask,
                                  PipelineStageFlag                    dst_mask,
                                  DependencyFlag                       dependency,
                                  std::span<const MemoryBarrier>       memory_barriers,
                                  std::span<const BufferMemoryBarrier> buffer_memory_barriers,
                                  std::span<const ImageMemoryBarrier>  image_memory_barriers) const noexcept
              -> const CommandBuffer&;

            auto push_constants(view::PipelineLayout  pipeline_layout,
                                ShaderStageFlag       stage,
                                std::span<const Byte> data,
                                u32                   offset = 0u) const noexcept -> const CommandBuffer&;

            auto submit(view::Queue                        queue,
                        std::span<view::Semaphore>         wait_semaphores   = {},
                        std::span<const PipelineStageFlag> wait_dst_stages   = {},
                        std::span<view::Semaphore>         signal_semaphores = {},
                        std::optional<view::Fence>         fence             = std::nullopt) const noexcept -> Expected<void>;

          private:
            CommandBufferLevel m_level = CommandBufferLevel::PRIMARY;
        };
    } // namespace view

    class STORMKIT_GPU_API CommandPool: public OwnedByDevice<CommandPool> {
      public:
        ~CommandPool() noexcept;

        CommandPool(const CommandPool&)    = delete;
        auto operator=(const CommandPool&) = delete;

        CommandPool(CommandPool&&) noexcept;
        auto operator=(CommandPool&&) noexcept -> CommandPool&;

        auto create_command_buffer(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<CommandBuffer>;

        template<usize COUNT, template<typename, usize> class Out = std::array>
        auto create_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<CommandBuffer, COUNT>>;

        template<template<typename> class Out = std::vector>
        auto create_command_buffers(usize count, CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<CommandBuffer>>;

        auto allocate_command_buffer(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Heap<CommandBuffer>>;

        template<usize COUNT, template<typename, usize> class Out = std::array>
        auto allocate_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Heap<CommandBuffer>>;

        template<template<typename> class Out = std::vector>
        auto allocate_command_buffers(usize count, CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<Heap<CommandBuffer>>>;

        // clang-format off
  // private:
        // clang-format on
        CommandPool(PrivateTag, view::Device&& device) noexcept;
        auto do_init(PrivateTag) noexcept -> Expected<void>;

      private:
        auto create_vk_command_buffers(usize, CommandBufferLevel) const noexcept -> Expected<std::vector<VkCommandBuffer>>;

        static auto delete_vk_command_buffers(Device, CommandPool, VkCommandBuffer) noexcept -> void;
    };

    namespace view {
        class CommandPool: DeviceObject<gpu::CommandPool> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Buffer>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            using DeviceObject<CommandPool>::DeviceObject();
            ~CommandPool() noexcept;

            CommandPool(const CommandPool&) noexcept;
            auto operator=(const CommandPool&) noexcept -> CommandPool&;

            CommandPool(CommandPool&&) noexcept;
            auto operator=(CommandPool&&) noexcept -> CommandPool&;

            auto create_command_buffer(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
              -> Expected<CommandBuffer>;

            template<usize COUNT, template<typename, usize> class Out = std::array>
            auto create_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
              -> Expected<Out<CommandBuffer, COUNT>>;

            template<template<typename> class Out = std::vector>
            auto create_command_buffers(usize count, CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
              -> Expected<Out<CommandBuffer>>;

            auto allocate_command_buffer(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
              -> Expected<Heap<CommandBuffer>>;

            template<usize COUNT, template<typename, usize> class Out = std::array>
            auto allocate_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
              -> Expected<Heap<CommandBuffer>>;

            template<template<typename> class Out = std::vector>
            auto allocate_command_buffers(usize count, CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
              -> Expected<Out<Heap<CommandBuffer>>>;

          private:
            auto create_vk_command_buffers(usize, CommandBufferLevel) const noexcept -> Expected<std::vector<VkCommandBuffer>>;

            static auto delete_vk_command_buffers(Device, CommandPool, VkCommandBuffer) noexcept -> void;
        };
    } // namespace view

} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Queue::Queue(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<Queue> { std::move(device), cmonadic::noop() } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Queue::~Queue() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Queue::Queue(Queue&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Queue::operator=(Queue&&) noexcept -> Queue& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Queue::submit(const SubmitInfo& submit_info, std::optional<view::Fence> fence) const noexcept -> Expected<void> {
        return submit({ &submit_info, 1 }, std::move(fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Queue::entry() const noexcept -> const Device::QueueEntry& {
        return m_entry;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Queue::Queue(const gpu::Queue& of) noexcept
            : DeviceObject<gpu::Queue> { of }, m_entry { of.entry() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::ContainedOrPointerOf<gpu::Queue> T>
        STORMKIT_FORCE_INLINE
        inline Queue::Queue(const T& of) noexcept
            : DeviceObject<gpu::Queue> { of }, m_entry { of->entry() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Queue::~Queue() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Queue::Queue(const Queue&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Queue::operator=(const Queue&) noexcept -> Queue& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Queue::Queue(const Queue&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Queue::operator=(const Queue&) noexcept -> Queue& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Queue::Queue(Queue&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Queue::operator=(Queue&&) noexcept -> Queue& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Queue::submit(const SubmitInfo& submit_info, std::optional<view::Fence> fence) const noexcept
          -> Expected<void> {
            return submit({ &submit_info, 1 }, std::move(fence));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Queue::entry() const noexcept -> const gpu::Device::QueueEntry& {
            return m_entry;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline CommandBuffer::CommandBuffer(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<CommandBuffer> { std::move(device), cmonadic::noop() } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandBuffer::create(view::Device       device,
                                      view::CommandPool  pool,
                                      CommandBufferLevel level,
                                      VkCommandBuffer&&  cmb,
                                      Deleter            deleter) noexcept -> CommandBuffer {
        auto out = CommandBuffer { PrivateTag, std::move(device) };
        out.do_init(std::move(pool), level, std::move(cmb), std::move(deleter));
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandBuffer::create(view::Device       device,
                                      view::CommandPool  pool,
                                      CommandBufferLevel level,
                                      VkCommandBuffer&&  cmb,
                                      Deleter            deleter) noexcept -> CommandBuffer {
        auto out = Heap<CommandBuffer> { PrivateTag, std::move(device) };
        out->do_init(std::move(pool), level, std::move(cmb), std::move(deleter));
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline CommandBuffer::~CommandBuffer() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline CommandBuffer::CommandBuffer(CommandBuffer&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandBuffer::operator=(CommandBuffer&&) noexcept -> CommandBuffer& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandBuffer::submit(view::Queue                        queue,
                                      std::span<const view::Semaphore>   wait_semaphores,
                                      std::span<const PipelineStageFlag> wait_dst_stages,
                                      std::span<const view::Semaphore>   signal_semaphores,
                                      std::optional<view::Fence>         fence) const noexcept -> Expected<void> {
        auto cmbs         = to_views<std::array>(*this);
        auto submit_infos = into_array(SubmitInfo {
          .wait_semaphores   = wait_semaphores,
          .wait_dst_stages   = wait_dst_stages,
          .command_buffers   = cmbs,
          .signal_semaphores = signal_semaphores });

        return queue.submit(submit_infos, std::move(fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandBuffer::state() const noexcept -> State {
        return m_state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandBuffer::level() const noexcept -> CommandBufferLevel {
        return m_level;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandBuffer::CommandBuffer(const gpu::CommandBuffer& of) noexcept
            : DeviceObject<gpu::CommandBuffer> { of }, m_state { of.state() }, m_level { of.level() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::ContainedOrPointerOf<gpu::CommandBuffer> T>
        STORMKIT_FORCE_INLINE
        inline CommandBuffer::CommandBuffer(const T& of) noexcept
            : DeviceObject<gpu::CommandBuffer> { of }, m_state { of->state() }, m_level { of->level() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandBuffer::~CommandBuffer() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandBuffer::CommandBuffer(const CommandBuffer&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandBuffer::operator=(const CommandBuffer&) noexcept -> CommandBuffer& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandBuffer::CommandBuffer(CommandBuffer&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandBuffer::operator=(CommandBuffer&&) noexcept -> CommandBuffer& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandBuffer::submit(Queue                              queue,
                                          std::span<const Semaphore>         wait_semaphores,
                                          std::span<const PipelineStageFlag> wait_dst_stages,
                                          std::span<const Semaphore>         signal_semaphores,
                                          std::optional<view::Fence>         fence) const noexcept -> Expected<void> {
            auto cmbs         = to_views<std::array>(*this);
            auto submit_infos = into_array(SubmitInfo {
              .wait_semaphores   = wait_semaphores,
              .wait_dst_stages   = wait_dst_stages,
              .command_buffers   = cmbs,
              .signal_semaphores = signal_semaphores });

            return queue.submit(submit_infos, std::move(fence));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandBuffer::state() const noexcept -> State {
            return m_state;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandBuffer::level() const noexcept -> CommandBufferLevel {
            return m_level;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline CommandPool::CommandPool(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<CommandPool> { std::move(device), &VolkDeviceTable::vkDestroyCommandPool } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline CommandPool::~CommandPool() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline CommandPool::CommandPool(CommandPool&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandPool::operator=(CommandPool&&) noexcept -> CommandPool& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandPool::create_command_buffer(CommandBufferLevel level) const noexcept -> Expected<CommandBuffer> {
        auto   cmbs = Try(create_command_buffers(1, level));
        Return std::move(cmbs.front());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    template<usize COUNT, template<typename, usize> class Out = std::array>
    inline auto CommandPool::create_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
      -> Expected<Out<CommandBuffer, COUNT>> {
        Return Try(create_vk_command_buffers(count, level))
          | stdv::transform([this, &count, &level](auto vk_handle) static noexcept {
                return CommandBuffer::create(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
            })
          | stdr::to<std::array>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    template<template<typename> class Out = std::vector>
    inline auto CommandPool::create_command_buffers(usize              count,
                                                    CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
      -> Expected<Out<CommandBuffer>> {
        Return transform(Try(create_vk_command_buffers(count, level)), [this, &count, &level](auto vk_handle) static noexcept {
            return CommandBuffer::create(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto CommandPool::allocate_command_buffer(CommandBufferLevel level) const noexcept -> Expected<Heap<CommandBuffer>> {
        auto   cmbs = Try(allocate_command_buffers(1, level));
        Return std::move(cmbs.front());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    template<usize COUNT, template<typename, usize> class Out = std::array>
    inline auto CommandPool::allocate_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
      -> Expected<Out<Heap<CommandBuffer>, COUNT>> {
        Return Try(create_vk_command_buffers(count, level))
          | stdv::transform([this, &count, &level](auto vk_handle) static noexcept {
                return CommandBuffer::allocate(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
            })
          | stdr::to<std::array>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    template<template<typename> class Out = std::vector>
    inline auto CommandPool::allocate_command_buffers(usize              count,
                                                      CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
      -> Expected<Out<CommandBuffer>> {
        Return transform(Try(create_vk_command_buffers(count, level)), [this, &count, &level](auto vk_handle) static noexcept {
            return CommandBuffer::allocate(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
        });
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandPool::~CommandPool() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandPool::CommandPool(const CommandPool&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandPool::operator=(const CommandPool&) noexcept -> CommandPool& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline CommandPool::CommandPool(CommandPool&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandPool::operator=(CommandPool&&) noexcept -> CommandPool& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandPool::create_command_buffer(CommandBufferLevel level) const noexcept -> Expected<gpu::CommandBuffer> {
            auto   cmbs = Try(create_command_buffers(1, level));
            Return std::move(cmbs.front());
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        template<usize COUNT, template<typename, usize> class Out = std::array>
        inline auto CommandPool::create_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<gpu::CommandBuffer, COUNT>> {
            Return Try(create_vk_command_buffers(count, level))
              | stdv::transform([this, &count, &level](auto vk_handle) static noexcept {
                    return gpu::CommandBuffer::create(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
                })
              | stdr::to<std::array>();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        template<template<typename> class Out = std::vector>
        inline auto CommandPool::create_command_buffers(usize              count,
                                                        CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<gpu::CommandBuffer>> {
            Return
              transform(Try(create_vk_command_buffers(count, level)), [this, &count, &level](auto vk_handle) static noexcept {
                  return gpu::CommandBuffer::create(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
              });
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto CommandPool::allocate_command_buffer(CommandBufferLevel level) const noexcept
          -> Expected<Heap<gpu::CommandBuffer>> {
            auto   cmbs = Try(allocate_command_buffers(1, level));
            Return std::move(cmbs.front());
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        template<usize COUNT, template<typename, usize> class Out = std::array>
        inline auto CommandPool::allocate_command_buffers(CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<Heap<gpu::CommandBuffer>, COUNT>> {
            Return Try(create_vk_command_buffers(count, level))
              | stdv::transform([this, &count, &level](auto vk_handle) static noexcept {
                    return gpu::CommandBuffer::allocate(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
                })
              | stdr::to<std::array>();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        template<template<typename> class Out = std::vector>
        inline auto CommandPool::allocate_command_buffers(usize              count,
                                                          CommandBufferLevel level = CommandBufferLevel::PRIMARY) const noexcept
          -> Expected<Out<gpu::CommandBuffer>> {
            Return
              transform(Try(create_vk_command_buffers(count, level)), [this, &count, &level](auto vk_handle) static noexcept {
                  return gpu::CommandBuffer::allocate(device(), *this, level, std::move(vk_handle), delete_vk_command_buffers);
              });
        }
    } // namespace view
} // namespace stormkit::gpu
