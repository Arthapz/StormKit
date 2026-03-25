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

import :objects;
import :raster_pipeline;
import :render_pass;

namespace stdfs = std::filesystem;

namespace cmeta = stormkit::core::meta;

namespace stormkit::gpu {
    export template<typename Base>
    class PipelineCacheInterface final: public DeviceObject<Base> {
      public:
        using DeviceObject<Base>::DeviceObject;
        using DeviceObject<Base>::operator=;
        using TagType = PipelineCacheTag;
    };

    export template<typename Base>
    class PipelineLayoutInterface final: public DeviceObject<Base> {
      public:
        using DeviceObject<Base>::DeviceObject;
        using DeviceObject<Base>::operator=;
        using TagType = PipelineLayoutTag;

        [[nodiscard]]
        auto raster_layout() const noexcept -> const RasterPipelineLayout&;
    };

    struct PipelineInterfaceBase {
        using StateVariant = std::variant<RasterPipelineState>;

        enum class Type {
            RASTER,
            COMPUTE,
            RAYTRACING,
        };
    };

    export template<typename Base>
    class PipelineInterface final: public DeviceObject<Base>, protected PipelineInterfaceBase {
      public:
        using DeviceObject<Base>::DeviceObject;
        using DeviceObject<Base>::operator=;
        using TagType = PipelineTag;

        using PipelineInterfaceBase::Type;

        [[nodiscard]]
        auto type() const noexcept -> Type;
        [[nodiscard]]
        auto raster_state() const noexcept -> const RasterPipelineState&;
    };

    class STORMKIT_GPU_API PipelineCacheImplementation: public GpuObjectImplementation<PipelineCacheTag> {
      public:
        PipelineCacheImplementation(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, stdfs::path&&) noexcept -> LoadSaveExpected<void>;
        ~PipelineCacheImplementation() noexcept;

        PipelineCacheImplementation(const PipelineCacheImplementation&)                    = delete;
        auto operator=(const PipelineCacheImplementation&) -> PipelineCacheImplementation& = delete;

        PipelineCacheImplementation(PipelineCacheImplementation&&) noexcept;
        auto operator=(PipelineCacheImplementation&&) noexcept -> PipelineCacheImplementation&;

        static auto load_from_file(view::Device device, stdfs::path cache_path) noexcept -> LoadSaveExpected<PipelineCache>;
        static auto allocate_load_from_file(view::Device device, stdfs::path cache_path) noexcept
          -> LoadSaveExpected<Heap<PipelineCache>>;

      protected:
        using UseNamedConstructors::allocate;
        using UseNamedConstructors::create;

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

    namespace view {
        class PipelineCacheImplementation: public GpuObjectViewImplementation<PipelineCacheTag> {
          public:
            using GpuObjectViewImplementation<PipelineCacheTag>::GpuObjectViewImplementation;
            using GpuObjectViewImplementation<PipelineCacheTag>::operator=;
        };
    } // namespace view

    class STORMKIT_GPU_API PipelineLayoutImplementation: public GpuObjectImplementation<PipelineLayoutTag> {
      public:
        PipelineLayoutImplementation(PrivateTag, view::Device&& device) noexcept;
        auto do_init(PrivateTag, const RasterPipelineLayout&) noexcept -> Expected<void>;
        ~PipelineLayoutImplementation() noexcept;

        PipelineLayoutImplementation(const PipelineLayoutImplementation&)                    = delete;
        auto operator=(const PipelineLayoutImplementation&) -> PipelineLayoutImplementation& = delete;

        PipelineLayoutImplementation(PipelineLayoutImplementation&&) noexcept;
        auto operator=(PipelineLayoutImplementation&&) noexcept -> PipelineLayoutImplementation&;

      protected:
        Heap<RasterPipelineLayout> m_layout;

        friend class view::PipelineLayoutImplementation;
    };

    namespace view {
        class PipelineLayoutImplementation: public GpuObjectViewImplementation<PipelineLayoutTag> {
          public:
            PipelineLayoutImplementation(const gpu::PipelineLayout& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::PipelineLayout> TContainerOrPointer>
            PipelineLayoutImplementation(const TContainerOrPointer&) noexcept;
            ~PipelineLayoutImplementation() noexcept;

            PipelineLayoutImplementation(const PipelineLayoutImplementation&) noexcept;
            auto operator=(const PipelineLayoutImplementation&) noexcept -> PipelineLayoutImplementation&;

            PipelineLayoutImplementation(PipelineLayoutImplementation&&) noexcept;
            auto operator=(PipelineLayoutImplementation&&) noexcept -> PipelineLayoutImplementation&;

          protected:
            ref<const RasterPipelineLayout> m_layout;
        };
    } // namespace view

    class STORMKIT_GPU_API PipelineImplementation: public GpuObjectImplementation<PipelineTag> {
        using StateVariant = PipelineInterfaceBase::StateVariant;

      public:
        using Type = PipelineInterfaceBase::Type;

        PipelineImplementation(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag,
                     const RasterPipelineState&,
                     view::PipelineLayout,
                     const RasterPipelineRenderingInfo&,
                     std::optional<view::PipelineCache>&& = std::nullopt) noexcept -> Expected<void>;
        auto do_init(PrivateTag,
                     const RasterPipelineState&,
                     view::PipelineLayout,
                     view::RenderPass,
                     std::optional<view::PipelineCache>&& = std::nullopt) noexcept -> Expected<void>;
        ~PipelineImplementation() noexcept;

        PipelineImplementation(const PipelineImplementation&)                    = delete;
        auto operator=(const PipelineImplementation&) -> PipelineImplementation& = delete;

        PipelineImplementation(PipelineImplementation&&) noexcept;
        auto operator=(PipelineImplementation&&) noexcept -> PipelineImplementation&;

      protected:
        Type               m_type;
        Heap<StateVariant> m_state;

        friend class view::PipelineImplementation;
    };

    namespace view {
        class PipelineImplementation: public GpuObjectViewImplementation<PipelineTag> {
            using StateVariant = PipelineInterfaceBase::StateVariant;

          public:
            using Type = PipelineInterfaceBase::Type;

            PipelineImplementation(const gpu::Pipeline& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Pipeline> TContainerOrPointer>
            PipelineImplementation(const TContainerOrPointer&) noexcept;
            ~PipelineImplementation() noexcept;

            PipelineImplementation(const PipelineImplementation&) noexcept;
            auto operator=(const PipelineImplementation&) noexcept -> PipelineImplementation&;

            PipelineImplementation(PipelineImplementation&&) noexcept;
            auto operator=(PipelineImplementation&&) noexcept -> PipelineImplementation&;

          protected:
            Type                    m_type;
            ref<const StateVariant> m_state;
        };
    } // namespace view
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayoutInterface<Base>::raster_layout() const noexcept -> const RasterPipelineLayout& {
        EXPECTS(Base::m_layout != nullptr);
        return *Base::m_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PipelineInterface<Base>::type() const noexcept -> Type {
        return Base::m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Base>
    STORMKIT_FORCE_INLINE
    inline auto PipelineInterface<Base>::raster_state() const noexcept -> const RasterPipelineState& {
        EXPECTS(Base::m_type == Type::RASTER);
        EXPECTS(Base::m_state != nullptr);
        EXPECTS(is<RasterPipelineState>(*Base::m_state));
        return as<RasterPipelineState>(*Base::m_state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineCacheImplementation::PipelineCacheImplementation(PrivateTag, view::Device&& device) noexcept
        : GpuObjectImplementation { std::move(device), &VolkDeviceTable::vkDestroyPipelineCache } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineCacheImplementation::PipelineCacheImplementation(PipelineCacheImplementation&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCacheImplementation::operator=(PipelineCacheImplementation&& other) noexcept
      -> PipelineCacheImplementation& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCacheImplementation::load_from_file(view::Device device, stdfs::path cache_path) noexcept
      -> LoadSaveExpected<PipelineCache> {
        Return UseNamedConstructors::create(std::move(device), std::move(cache_path));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineCacheImplementation::allocate_load_from_file(view::Device device, stdfs::path cache_path) noexcept
      -> LoadSaveExpected<Heap<PipelineCache>> {
        Return UseNamedConstructors::allocate(std::move(device), std::move(cache_path));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayoutImplementation::PipelineLayoutImplementation(PrivateTag, view::Device&& device) noexcept
        : GpuObjectImplementation { std::move(device), &VolkDeviceTable::vkDestroyPipelineLayout } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayoutImplementation::~PipelineLayoutImplementation() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineLayoutImplementation::PipelineLayoutImplementation(PipelineLayoutImplementation&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineLayoutImplementation::operator=(PipelineLayoutImplementation&&) noexcept
      -> PipelineLayoutImplementation& = default;

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineLayoutImplementation::PipelineLayoutImplementation(const gpu::PipelineLayout& of) noexcept
            : GpuObjectViewImplementation { of }, m_layout { as_ref(of.raster_layout()) } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::PipelineLayout> TContainerOrPointer>
        STORMKIT_FORCE_INLINE
        inline PipelineLayoutImplementation::PipelineLayoutImplementation(const TContainerOrPointer& of) noexcept
            : PipelineLayoutImplementation { *of } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineLayoutImplementation::~PipelineLayoutImplementation() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineLayoutImplementation::PipelineLayoutImplementation(const PipelineLayoutImplementation&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PipelineLayoutImplementation::operator=(const PipelineLayoutImplementation& other) noexcept
          -> PipelineLayoutImplementation& {
            if (&other == this) [[unlikely]]
                return *this;

            m_layout = as_ref(other.m_layout);

            return *this;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineLayoutImplementation::PipelineLayoutImplementation(PipelineLayoutImplementation&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PipelineLayoutImplementation::operator=(PipelineLayoutImplementation&&) noexcept
          -> PipelineLayoutImplementation& = default;
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineImplementation::PipelineImplementation(PrivateTag, view::Device&& device) noexcept
        : GpuObjectImplementation { std::move(device), &VolkDeviceTable::vkDestroyPipeline } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineImplementation::~PipelineImplementation() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline PipelineImplementation::PipelineImplementation(PipelineImplementation&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto PipelineImplementation::operator=(PipelineImplementation&& other) noexcept -> PipelineImplementation& = default;

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineImplementation::PipelineImplementation(const gpu::Pipeline& of) noexcept
            : GpuObjectViewImplementation { of }, m_type { of.type() }, m_state { as_ref(of.m_state) } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Pipeline> TContainerOrPointer>
        STORMKIT_FORCE_INLINE
        inline PipelineImplementation::PipelineImplementation(const TContainerOrPointer& of) noexcept
            : PipelineImplementation { *of } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineImplementation::~PipelineImplementation() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineImplementation::PipelineImplementation(const PipelineImplementation&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PipelineImplementation::operator=(const PipelineImplementation& other) noexcept -> PipelineImplementation& {
            if (&other == this) [[unlikely]]
                return *this;

            GpuObjectViewImplementation::operator=(other);

            m_type  = other.m_type;
            m_state = as_ref(other.m_state);

            return *this;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline PipelineImplementation::PipelineImplementation(PipelineImplementation&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto PipelineImplementation::operator=(PipelineImplementation&&) noexcept -> PipelineImplementation& = default;
    } // namespace view
} // namespace stormkit::gpu
