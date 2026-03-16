// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.execution:render_pass;

import std;

import stormkit.core;
import stormkit.gpu.core;
import stormkit.gpu.resource;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    class FrameBuffer;
    class RenderPass;

    namespace view {
        class FrameBuffer;
        class RenderPass;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<FrameBuffer> {
            using Of          = FrameBuffer;
            using ValueType   = VkFramebuffer;
            using DeleterType = PFN_vkDestroyFramebuffer VolkDeviceTable::*;
            using ViewType    = view::FrameBuffer;
            using OwnedBy     = Device;

            static constexpr auto DISABLE_CREATE_ALLOCATE = true;
            static constexpr auto DEBUG_TYPE              = DebugObjectType::FRAMEBUFFER;
        };

        template<>
        struct ObjectInfo<RenderPass> {
            using Of          = RenderPass;
            using ValueType   = VkRenderPass;
            using DeleterType = PFN_vkDestroyRenderPass VolkDeviceTable::*;
            using ViewType    = view::RenderPass;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::RENDER_PASS;
        };
    } // namespace meta

    class RenderPass;

    class STORMKIT_GPU_API FrameBuffer: public OwnedByDevice<FrameBuffer> {
      public:
        ~FrameBuffer() noexcept;

        FrameBuffer(const FrameBuffer&)                    = delete;
        auto operator=(const FrameBuffer&) -> FrameBuffer& = delete;

        FrameBuffer(FrameBuffer&&) noexcept;
        auto operator=(FrameBuffer&&) noexcept -> FrameBuffer&;

        [[nodiscard]]
        auto extent() const noexcept -> const math::uextent2&;
        [[nodiscard]]
        auto attachments() const noexcept -> std::span<const view::ImageView>;

        FrameBuffer(PrivateTag, view::Device&&) noexcept;

      private:
        static auto create(view::Device&&                 device,
                           view::RenderPass               render_pass,
                           const math::uextent2&          extent,
                           std::vector<view::ImageView>&& attachments) noexcept -> Expected<FrameBuffer>;
        static auto allocate(view::Device&&                 device,
                             view::RenderPass               render_pass,
                             const math::uextent2&          extent,
                             std::vector<view::ImageView>&& attachments) noexcept -> Expected<Heap<FrameBuffer>>;

        auto do_init(view::RenderPass&&, const math::uextent2&, std::vector<view::ImageView>&&) noexcept -> Expected<void>;

        math::uextent2               m_extent = { 0, 0 };
        std::vector<view::ImageView> m_attachments;

        friend class RenderPass;
        friend class view::RenderPass;
    };

    namespace view {
        class STORMKIT_GPU_API FrameBuffer: public DeviceObject<gpu::FrameBuffer> {
          public:
            using ObjectInfo = typename meta::ObjectInfo<gpu::FrameBuffer>;
            using ValueType  = ObjectInfo::ValueType;
            using ViewType   = ObjectInfo::ViewType;

            FrameBuffer(const gpu::FrameBuffer& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::FrameBuffer> T>
            FrameBuffer(const T& of) noexcept;
            ~FrameBuffer() noexcept;

            FrameBuffer(const FrameBuffer&) noexcept;
            auto operator=(const FrameBuffer&) noexcept -> FrameBuffer&;

            FrameBuffer(FrameBuffer&&) noexcept;
            auto operator=(FrameBuffer&&) noexcept -> FrameBuffer&;

            [[nodiscard]]
            auto extent() const noexcept -> const math::uextent2&;
            [[nodiscard]]
            auto attachments() const noexcept -> std::span<const view::ImageView>;

          private:
            math::uextent2                   m_extent = { 0, 0 };
            std::span<const view::ImageView> m_attachments;
        };
    } // namespace view

    struct AttachmentDescription {
        PixelFormat     format;
        SampleCountFlag samples = SampleCountFlag::C1;

        AttachmentLoadOperation  load_op  = AttachmentLoadOperation::CLEAR;
        AttachmentStoreOperation store_op = AttachmentStoreOperation::STORE;

        AttachmentLoadOperation  stencil_load_op  = AttachmentLoadOperation::DONT_CARE;
        AttachmentStoreOperation stencil_store_op = AttachmentStoreOperation::DONT_CARE;

        ImageLayout source_layout      = ImageLayout::UNDEFINED;
        ImageLayout destination_layout = ImageLayout::PRESENT_SRC;

        bool resolve = false;
    };

    using AttachmentDescriptions = std::vector<AttachmentDescription>;

    struct Subpass {
        struct Ref {
            u32 attachment_id;

            ImageLayout layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        };

        PipelineBindPoint  bind_point;
        std::vector<Ref>   color_attachment_refs   = {};
        std::vector<Ref>   resolve_attachment_refs = {};
        std::optional<Ref> depth_attachment_ref    = {};
    };

    using Subpasses = std::vector<Subpass>;

    struct RenderPassDescription {
        AttachmentDescriptions attachments;
        Subpasses              subpasses;

        auto is_compatible(const RenderPassDescription& description) const noexcept -> bool;
    };

    class STORMKIT_GPU_API RenderPass: public OwnedByDevice<RenderPass> {
      public:
        ~RenderPass() noexcept;

        RenderPass(const RenderPass&)                    = delete;
        auto operator=(const RenderPass&) -> RenderPass& = delete;

        RenderPass(RenderPass&&) noexcept;
        auto operator=(RenderPass&&) noexcept -> RenderPass&;

        auto create_framebuffer(view::Device                 device,
                                const math::uextent2&        extent,
                                std::vector<view::ImageView> attachments) const noexcept -> Expected<FrameBuffer>;
        auto allocate_framebuffer(view::Device                 device,
                                  const math::uextent2&        extent,
                                  std::vector<view::ImageView> attachments) const noexcept -> Expected<Heap<FrameBuffer>>;

        [[nodiscard]]
        auto is_compatible(view::RenderPass render_pass) const noexcept -> bool;
        [[nodiscard]]
        auto is_compatible(const RenderPassDescription& description) const noexcept -> bool;

        [[nodiscard]]
        auto description() const noexcept -> const RenderPassDescription&;

        // clang-format off
  // private:
        // clang-format on
        RenderPass(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, const RenderPassDescription&) noexcept -> Expected<void>;

      private:
        RenderPassDescription m_description = {};
    };

    namespace view {
        class STORMKIT_GPU_API RenderPass: public DeviceObject<gpu::RenderPass> {
          public:
            using ObjectInfo = typename meta::ObjectInfo<gpu::RenderPass>;
            using ValueType  = ObjectInfo::ValueType;
            using ViewType   = ObjectInfo::ViewType;

            // RenderPass(const gpu::RenderPass& of) noexcept;
            // template<cmeta::IsContainerOrPointerOf<gpu::RenderPass> T>
            // RenderPass(const T& of) noexcept;
            using DeviceObject<gpu::RenderPass>::DeviceObject;
            ~RenderPass() noexcept;

            RenderPass(const RenderPass&) noexcept;
            auto operator=(const RenderPass&) noexcept -> RenderPass&;

            RenderPass(RenderPass&&) noexcept;
            auto operator=(RenderPass&&) noexcept -> RenderPass&;

            auto create_framebuffer(Device                 device,
                                    const math::uextent2&  extent,
                                    std::vector<ImageView> attachments) const noexcept -> Expected<gpu::FrameBuffer>;
            auto allocate_framebuffer(view::Device           device,
                                      const math::uextent2&  extent,
                                      std::vector<ImageView> attachments) const noexcept -> Expected<Heap<gpu::FrameBuffer>>;
        };
    } // namespace view

    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const AttachmentDescription& value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const Subpass::Ref& value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const Subpass& value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const RenderPassDescription& value) noexcept -> Ret;
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline FrameBuffer::FrameBuffer(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<FrameBuffer> { std::move(device), &VolkDeviceTable::vkDestroyFramebuffer } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto FrameBuffer::create(view::Device&&                 device,
                                    view::RenderPass               render_pass,
                                    const math::uextent2&          extent,
                                    std::vector<view::ImageView>&& attachments) noexcept -> Expected<FrameBuffer> {
        auto frame_buffer = FrameBuffer { PRIVATE, std::move(device) };
        Try(frame_buffer.do_init(std::move(render_pass), extent, std::move(attachments)));
        Return frame_buffer;
    }

    ////////////////////////////////////p/
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto FrameBuffer::allocate(view::Device&&                 device,
                                      view::RenderPass               render_pass,
                                      const math::uextent2&          extent,
                                      std::vector<view::ImageView>&& attachments) noexcept -> Expected<Heap<FrameBuffer>> {
        auto frame_buffer = core::allocate_unsafe<FrameBuffer>(PRIVATE, std::move(device));
        Try(frame_buffer->do_init(std::move(render_pass), extent, std::move(attachments)));
        Return frame_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline FrameBuffer::~FrameBuffer() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline FrameBuffer::FrameBuffer(FrameBuffer&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto FrameBuffer::operator=(FrameBuffer&&) noexcept -> FrameBuffer& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto FrameBuffer::extent() const noexcept -> const math::uextent2& {
        return m_extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto FrameBuffer::attachments() const noexcept -> std::span<const view::ImageView> {
        return m_attachments;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline FrameBuffer::FrameBuffer(const gpu::FrameBuffer& of) noexcept
            : DeviceObject<gpu::FrameBuffer> { of }, m_extent { of.extent() }, m_attachments { of.attachments() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::FrameBuffer> T>
        STORMKIT_FORCE_INLINE
        inline FrameBuffer::FrameBuffer(const T& of) noexcept
            : DeviceObject<gpu::FrameBuffer> { of }, m_extent { of->extent() }, m_attachments { of->attachments() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline FrameBuffer::~FrameBuffer() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline FrameBuffer::FrameBuffer(const FrameBuffer&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto FrameBuffer::operator=(const FrameBuffer&) noexcept -> FrameBuffer& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline FrameBuffer::FrameBuffer(FrameBuffer&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto FrameBuffer::operator=(FrameBuffer&&) noexcept -> FrameBuffer& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto FrameBuffer::extent() const noexcept -> const math::uextent2& {
            return m_extent;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto FrameBuffer::attachments() const noexcept -> std::span<const view::ImageView> {
            return m_attachments;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline RenderPass::RenderPass(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<RenderPass> { std::move(device), &VolkDeviceTable::vkDestroyRenderPass } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline RenderPass::~RenderPass() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline RenderPass::RenderPass(RenderPass&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto RenderPass::operator=(RenderPass&&) noexcept -> RenderPass& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto RenderPass::create_framebuffer(view::Device                 device,
                                               const math::uextent2&        extent,
                                               std::vector<view::ImageView> attachments) const noexcept -> Expected<FrameBuffer> {
        return FrameBuffer::create(std::move(device), *this, extent, std::move(attachments));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto RenderPass::allocate_framebuffer(view::Device                 device,
                                                 const math::uextent2&        extent,
                                                 std::vector<view::ImageView> attachments) const noexcept
      -> Expected<Heap<FrameBuffer>> {
        return FrameBuffer::allocate(std::move(device), *this, extent, std::move(attachments));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto RenderPass::is_compatible(view::RenderPass) const noexcept -> bool {
        // TODO implement proper compatibility check
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap7.html#renderpass-compatibility

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto RenderPass::description() const noexcept -> const RenderPassDescription& {
        return m_description;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline RenderPass::~RenderPass() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline RenderPass::RenderPass(const RenderPass&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto RenderPass::operator=(const RenderPass&) noexcept -> RenderPass& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline RenderPass::RenderPass(RenderPass&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto RenderPass::operator=(RenderPass&&) noexcept -> RenderPass& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto RenderPass::create_framebuffer(Device                 device,
                                                   const math::uextent2&  extent,
                                                   std::vector<ImageView> attachments) const noexcept
          -> Expected<gpu::FrameBuffer> {
            return gpu::FrameBuffer::create(std::move(device), *this, extent, std::move(attachments));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto RenderPass::allocate_framebuffer(Device                 device,
                                                     const math::uextent2&  extent,
                                                     std::vector<ImageView> attachments) const noexcept
          -> Expected<Heap<gpu::FrameBuffer>> {
            return gpu::FrameBuffer::allocate(std::move(device), *this, extent, std::move(attachments));
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const AttachmentDescription& value) noexcept -> Ret {
        return hash<Ret>(value.format,
                         value.samples,
                         value.load_op,
                         value.store_op,
                         value.stencil_load_op,
                         value.stencil_store_op,
                         value.source_layout,
                         value.destination_layout,
                         value.resolve);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const Subpass::Ref& value) noexcept -> Ret {
        return hash<Ret>(value.attachment_id, value.layout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const Subpass& value) noexcept -> Ret {
        return hash<Ret>(value.bind_point,
                         value.color_attachment_refs,
                         value.depth_attachment_ref,
                         value.resolve_attachment_refs);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const RenderPassDescription& value) noexcept -> Ret {
        return hash<Ret>(value.attachments, value.subpasses);
    }
} // namespace stormkit::gpu
