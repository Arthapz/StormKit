// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/core/contract_macro.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.execution:pipeline;

import std;

import stormkit.core;
import stormkit.gpu.core;

import :raster_pipeline;
import :render_pass;

namespace stdfs = std::filesystem;

namespace cmeta = stormkit::core::meta;

export namespace stormkit::gpu {
    class PipelineCache;
    class Pipeline;
    class PipelineLayout;

    namespace view {
        class PipelineCache;
        class Pipeline;
        class PipelineLayout;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<PipelineCache> {
            using Of          = PipelineCache;
            using ElementType = VkFramebuffer;
            using DeleterType = PFN_vkDestroyFramebuffer VolkDeviceTable::*;
            using ViewType    = view::PipelineCache;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::PIPELINE_CACHE;
        };

        template<>
        struct ObjectInfo<Pipeline> {
            using Of          = Pipeline;
            using ElementType = VkPipeline;
            using DeleterType = PFN_vkDestroyPipeline VolkDeviceTable::*;
            using ViewType    = view::Pipeline;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::PIPELINE;
        };

        template<>
        struct ObjectInfo<PipelineLayout> {
            using Of          = PipelineLayout;
            using ElementType = VkPipelineLayout;
            using DeleterType = PFN_vkDestroyPipelineLayout VolkDeviceTable::*;
            using ViewType    = view::PipelineLayout;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::PIPELINE_LAYOUT;
        };
    } // namespace meta

    class STORMKIT_GPU_API PipelineCache: public OwnedByDevice<Device> {
      public:
        using LoadSaveError = DecoratedError<std::variant<core::SystemError, Result>>;
        template<typename T>
        using LoadSaveExpected = core::Expected<T, LoadSaveError>;

        static auto load_from_file(const Device& device, stdfs::path cache_path) noexcept -> LoadSaveExpected<PipelineCache>;
        ~PipelineCache() noexcept;

        PipelineCache(const PipelineCache&)                    = delete;
        auto operator=(const PipelineCache&) -> PipelineCache& = delete;

        PipelineCache(PipelineCache&&) noexcept;
        auto operator=(PipelineCache&&) noexcept -> PipelineCache&;

        PipelineCache(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, stdfs::path&&) noexcept -> LoadSaveExpected<void>;

      private:
        auto create_new_pipeline_cache(const Device&) noexcept -> LoadSaveExpected<void>;
        auto read_pipeline_cache(const Device&) noexcept -> LoadSaveExpected<void>;
        auto save_cache() noexcept -> LoadSaveExpected<void>;

        static constexpr auto MAGIC   = u32 { 0xDEADBEEF };
        static constexpr auto VERSION = u32 { 1u };

        struct SerializedCache {
            struct {
                u32   magic;
                usize data_size;
                u64   data_hash;
            } guard;

            struct {
                u32 version;
                u64 vendor_id;
                u64 device_id;
            } infos;

            struct {
                std::array<u8, VK_UUID_SIZE> value;
            } uuid;
        } m_serialized;

        stdfs::path m_path;
    };

    namespace view {
        using PipelineCache = DeviceObject<PipelineCache>;
    }

    class STORMKIT_GPU_API PipelineLayout {
      public:
        ~PipelineLayout() noexcept;

        PipelineLayout(const PipelineLayout&)                    = delete;
        auto operator=(const PipelineLayout&) -> PipelineLayout& = delete;

        PipelineLayout(PipelineLayout&&) noexcept;
        auto operator=(PipelineLayout&&) noexcept -> PipelineLayout&;

        [[nodiscard]]
        auto raster_layout() const noexcept -> const RasterPipelineLayout&;

        // clang-format off
  // private:
        // clang-format on
        PipelineLayout(view::Device&& device) noexcept;
        auto do_init(PrivateTag, const RasterPipelineLayout&) noexcept -> Expected<void>;

      private:
        RasterPipelineLayout m_layout;
    };

    namespace view {
        using PipelineLayout = DeviceObject<PipelineLayout>;
    }

    class STORMKIT_GPU_API Pipeline {
      public:
        enum class Type {
            RASTER,
            COMPUTE,
            RAYTRACING,
        };

        ~Pipeline() noexcept;

        Pipeline(const Pipeline&)                    = delete;
        auto operator=(const Pipeline&) -> Pipeline& = delete;

        Pipeline(Pipeline&&) noexcept;
        auto operator=(Pipeline&&) noexcept -> Pipeline&;

        [[nodiscard]]
        auto type() const noexcept -> Type;
        [[nodiscard]]
        auto raster_state() const noexcept -> const RasterPipelineState&;

        // clang-format off
  // private:
        // clang-format on
        Pipeline(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag,
                     const RasterPipelineState&,
                     const PipelineLayout&,
                     std::optional<RasterPipelineRenderingInfo>,
                     std::optional<view::PipelineCache>) noexcept -> Expected<void>;
        auto do_init(PrivateTag,
                     const RasterPipelineState&,
                     const PipelineLayout&,
                     std::optional<view::RenderPass>,
                     std::optional<view::PipelineCache>) noexcept -> Expected<void>;

      private:
        Type                              m_type;
        std::variant<RasterPipelineState> m_state;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineCache::PipelineCache(const Device& device, stdfs::path path, PrivateFuncTag) noexcept
        : m_path { std::move(path) },
          m_vk_device { device.native_handle() },
          m_vk_device_table { as_ref(device.device_table()) },
          m_vk_handle { { [vk_device_table = *m_vk_device_table, vk_device = m_vk_device](auto handle) noexcept {
              vk_device_table.vkDestroyPipelineCache(vk_device, handle, nullptr);
          } } } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineCache::PipelineCache(PipelineCache&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCache::load_from_file(const Device& device, stdfs::path cache_path) noexcept
      -> LoadSaveExpected<PipelineCache> {
        auto cache = PipelineCache { device, std::move(cache_path), PrivateFuncTag {} };
        Try(cache.do_init(device));
        Return cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCache::native_handle() const noexcept -> VkPipelineCache {
        return m_vk_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCache::do_init(const Device& device) noexcept -> LoadSaveExpected<void> {
        Return read_pipeline_cache(device);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayout::PipelineLayout(const Device& device, const RasterPipelineLayout& layout, PrivateFuncTag) noexcept
        : m_layout { layout },
          m_vk_device { device.native_handle() },
          m_vk_device_table { as_ref(device.device_table()) },
          m_vk_handle { { [vk_device_table = *m_vk_device_table, vk_device = m_vk_device](auto handle) noexcept {
              vk_device_table.vkDestroyPipelineLayout(vk_device, handle, nullptr);
          } } } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayout::~PipelineLayout() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayout::operator=(PipelineLayout&& other) noexcept -> PipelineLayout& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayout::create(const Device& device, const RasterPipelineLayout& layout) noexcept
      -> Expected<PipelineLayout> {
        auto pipeline_layout = PipelineLayout { device, layout, PrivateFuncTag {} };
        Try(pipeline_layout.do_init());
        Return pipeline_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayout::native_handle() const noexcept -> VkPipelineLayout {
        return m_vk_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayout::do_init() noexcept -> Expected<void> {
        namespace stdv         = std::views;
        namespace stdr         = std::ranges;
        const auto set_layouts = m_layout.descriptor_set_layouts
                                 | stdv::transform(core::monadic::unref())
                                 | stdv::transform(vk::monadic::to_vk())
                                 | stdr::to<std::vector>();

        const auto push_constant_ranges = transform(m_layout.push_constant_ranges, [](auto&& push_constant_range) noexcept {
            return VkPushConstantRange {
                .stageFlags = vk::to_vk<VkShaderStageFlags>(push_constant_range.stages),
                .offset     = push_constant_range.offset,
                .size       = as<u32>(push_constant_range.size),
            };
        });

        const auto create_info = VkPipelineLayoutCreateInfo {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext                  = nullptr,
            .flags                  = 0,
            .setLayoutCount         = as<u32>(stdr::size(set_layouts)),
            .pSetLayouts            = stdr::data(set_layouts),
            .pushConstantRangeCount = as<u32>(stdr::size(push_constant_ranges)),
            .pPushConstantRanges    = stdr::data(push_constant_ranges),
        };

        m_vk_handle = Try(vk::call_checked<VkPipelineLayout>(m_vk_device_table->vkCreatePipelineLayout,
                                                             m_vk_device,
                                                             &create_info,
                                                             nullptr));

        Return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Pipeline::Pipeline(const Device& device, const RasterPipelineState& state, PrivateFuncTag) noexcept
        : m_type { Type::RASTER },
          m_state { state },
          m_vk_device { device.native_handle() },
          m_vk_device_table { as_ref(device.device_table()) },
          m_vk_handle { { [vk_device_table = *m_vk_device_table, vk_device = m_vk_device](auto handle) noexcept {
              vk_device_table.vkDestroyPipeline(vk_device, handle, nullptr);
          } } } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Pipeline::create(const Device&                    device,
                                 const RasterPipelineState&       state,
                                 const PipelineLayout&            layout,
                                 const RenderPass&                render_pass,
                                 OptionalRef<const PipelineCache> cache) noexcept -> Expected<Pipeline> {
        auto pipeline = Pipeline { device, state, PrivateFuncTag {} };
        Try(pipeline.do_init(layout, as_opt_ref(render_pass), std::nullopt, std::move(cache)));
        Return pipeline;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Pipeline::create(const Device&                      device,
                                 const RasterPipelineState&         state,
                                 const PipelineLayout&              layout,
                                 const RasterPipelineRenderingInfo& rendering_info,
                                 OptionalRef<const PipelineCache>   cache) noexcept -> Expected<Pipeline> {
        auto pipeline = Pipeline { device, state, PrivateFuncTag {} };
        Try(pipeline.do_init(layout, std::nullopt, as_opt_ref(rendering_info), std::move(cache)));
        Return pipeline;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Pipeline::~Pipeline() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Pipeline::Pipeline(Pipeline&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Pipeline::operator=(Pipeline&& other) noexcept -> Pipeline& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Pipeline::type() const noexcept -> Type {
        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Pipeline::raster_state() const noexcept -> const RasterPipelineState& {
        EXPECTS(m_type == Type::RASTER);
        EXPECTS(is<RasterPipelineState>(m_state));
        return as<RasterPipelineState>(m_state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Pipeline::native_handle() const noexcept -> VkPipeline {
        return m_vk_handle;
    }
} // namespace stormkit::gpu
