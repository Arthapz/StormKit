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
        using PipelineCache  = DeviceObject<PipelineCache>;
        using PipelineLayout = DeviceObject<PipelineLayout>;
        class Pipeline;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<PipelineCache> {
            using Of          = PipelineCache;
            using ElementType = VkPipelineCache;
            using DeleterType = PFN_vkDestroyPipelineCache VolkDeviceTable::*;
            using ViewType    = view::PipelineCache;
            using OwnedBy     = Device;

            static constexpr auto DISABLE_CREATE_ALLOCATE = true;
            static constexpr auto DEBUG_TYPE              = DebugObjectType::PIPELINE_CACHE;
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

    class STORMKIT_GPU_API PipelineCache: public OwnedByDevice<PipelineCache> {
      public:
        using LoadSaveError = DecoratedError<std::variant<core::SystemError, Result>>;
        template<typename T>
        using LoadSaveExpected = core::Expected<T, LoadSaveError>;

        static auto load_from_file(view::Device device, stdfs::path cache_path) noexcept -> LoadSaveExpected<PipelineCache>;
        static auto allocate_load_from_file(view::Device device, stdfs::path cache_path) noexcept
          -> LoadSaveExpected<Heap<PipelineCache>>;
        ~PipelineCache() noexcept;

        PipelineCache(const PipelineCache&)                    = delete;
        auto operator=(const PipelineCache&) -> PipelineCache& = delete;

        PipelineCache(PipelineCache&&) noexcept;
        auto operator=(PipelineCache&&) noexcept -> PipelineCache&;

        // clang-format off
  // private:
        // clang-format on
        PipelineCache(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, stdfs::path&&) noexcept -> LoadSaveExpected<void>;

      private:
        auto create_new_pipeline_cache() noexcept -> LoadSaveExpected<void>;
        auto read_pipeline_cache() noexcept -> LoadSaveExpected<void>;
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

    class STORMKIT_GPU_API PipelineLayout: public OwnedByDevice<PipelineLayout> {
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
        PipelineLayout(PrivateTag, view::Device&& device) noexcept;
        auto do_init(PrivateTag, const RasterPipelineLayout&) noexcept -> Expected<void>;

      private:
        RasterPipelineLayout m_layout;
    };

    class STORMKIT_GPU_API Pipeline: public OwnedByDevice<Pipeline> {
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
                     view::PipelineLayout&&,
                     const RasterPipelineRenderingInfo&,
                     std::optional<view::PipelineCache>&& = std::nullopt) noexcept -> Expected<void>;
        auto do_init(PrivateTag,
                     const RasterPipelineState&,
                     view::PipelineLayout&&,
                     view::RenderPass&&,
                     std::optional<view::PipelineCache>&& = std::nullopt) noexcept -> Expected<void>;

      private:
        Type                              m_type;
        std::variant<RasterPipelineState> m_state;
    };

    namespace view {
        class STORMKIT_GPU_API Pipeline: public DeviceObject<gpu::Pipeline> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::Pipeline>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            Pipeline(const gpu::Pipeline& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Pipeline> T>
            Pipeline(const T& of) noexcept;
            ~Pipeline() noexcept;

            Pipeline(const Pipeline&) noexcept;
            auto operator=(const Pipeline&) noexcept -> Pipeline&;

            Pipeline(Pipeline&&) noexcept;
            auto operator=(Pipeline&&) noexcept -> Pipeline&;

            [[nodiscard]]
            auto type() const noexcept -> gpu::Pipeline::Type;

          private:
            gpu::Pipeline::Type m_type;
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
    inline PipelineCache::PipelineCache(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<PipelineCache> { std::move(device), &VolkDeviceTable::vkDestroyPipelineCache } {
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
    inline auto PipelineCache::load_from_file(view::Device device, stdfs::path cache_path) noexcept
      -> LoadSaveExpected<PipelineCache> {
        auto cache = PipelineCache { PRIVATE, std::move(device) };
        Try(cache.do_init(PRIVATE, std::move(cache_path)));
        Return cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCache::allocate_load_from_file(view::Device device, stdfs::path cache_path) noexcept
      -> LoadSaveExpected<Heap<PipelineCache>> {
        auto cache = core::allocate_unsafe<PipelineCache>(PRIVATE, std::move(device));
        Try(cache->do_init(PRIVATE, std::move(cache_path)));
        Return cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayout::PipelineLayout(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<PipelineLayout> { std::move(device), &VolkDeviceTable::vkDestroyPipelineLayout } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayout::~PipelineLayout() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayout::PipelineLayout(PipelineLayout&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayout::operator=(PipelineLayout&&) noexcept -> PipelineLayout& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Pipeline::Pipeline(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<Pipeline> { std::move(device), &VolkDeviceTable::vkDestroyPipeline } {
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

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Pipeline::Pipeline(const gpu::Pipeline& of) noexcept
            : DeviceObject<gpu::Pipeline> { of }, m_type { of.type() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Pipeline> T>
        STORMKIT_FORCE_INLINE
        inline Pipeline::Pipeline(const T& of) noexcept
            : DeviceObject<gpu::Pipeline> { of }, m_type { of->type() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Pipeline::~Pipeline() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Pipeline::Pipeline(const Pipeline& other) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Pipeline::operator=(const Pipeline& other) noexcept -> Pipeline& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Pipeline::Pipeline(Pipeline&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Pipeline::operator=(Pipeline&&) noexcept -> Pipeline& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Pipeline::type() const noexcept -> gpu::Pipeline::Type {
            return m_type;
        }
    } // namespace view
} // namespace stormkit::gpu
