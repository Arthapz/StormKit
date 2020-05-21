// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/TextureView.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    struct FrameGraphRenderTarget {
        struct Descriptor {};
    };

    template<typename T>
    using PassHandle = std::variant<std::unique_ptr<T>, _std::observer_ptr<T>>;

    struct STORM_PUBLIC FrameGraphTexture {
        struct Descriptor {
            render::TextureType type;
            render::PixelFormat format;

            core::Extentu extent;

            core::UInt8 levels              = 1;
            render::SampleCountFlag samples = render::SampleCountFlag::C1_BIT;

            render::TextureUsage usage = static_cast<render::TextureUsage>(0);
        };

        void realize(const render::Device &device, const Descriptor &descriptor);
        void derealize();

        inline render::Texture &texture() noexcept;
        inline const render::Texture &texture() const noexcept;

        PassHandle<render::Texture> storage;
    };

    struct STORM_PUBLIC FrameGraphBuffer {
        struct Descriptor {
            core::ArraySize size;
        };

        void realize(const render::Device &device, const Descriptor &descriptor);
        void derealize();

        inline render::HardwareBuffer &buffer() noexcept;
        inline const render::HardwareBuffer &buffer() const noexcept;

        PassHandle<render::HardwareBuffer> storage;
    };

    class STORM_PUBLIC FramePassResourceBase: public core::NonCopyable {
      public:
        FramePassResourceBase(FramePassResourceBase &&);
        FramePassResourceBase &operator=(FramePassResourceBase &&);

        virtual ~FramePassResourceBase();

        inline std::string_view name() const noexcept;
        inline core::UInt32 id() const noexcept;
        inline bool isTransient() const noexcept;

        inline core::UInt32 createdBy() const noexcept;
        inline core::UInt32 destroyedIn() const noexcept;

        inline core::span<const core::UInt32> readIn() const noexcept;
        inline core::span<const core::UInt32> writeIn() const noexcept;

        virtual void realize(const render::Device &device) noexcept = 0;
        virtual void derealize() noexcept                           = 0;

      protected:
        explicit FramePassResourceBase(std::string name,
                                       core::UInt32 id,
                                       bool transient = true) noexcept;

        std::string m_name;
        core::UInt32 m_id;

        bool m_is_transient = true;

        core::UInt32 m_created_by;
        core::UInt32 m_destroyed_in;

        std::vector<core::UInt32> m_read_in;
        std::vector<core::UInt32> m_write_in;

        core::UInt32 m_ref_count;

        friend class FramePassBuilder;
        friend class FrameGraph;
    };

    template<typename Resource>
    class STORM_PUBLIC FramePassResource: public FramePassResourceBase {
      public:
        using Descriptor = typename Resource::Descriptor;

        FramePassResource(std::string name, core::UInt32 id, Descriptor &&descriptor) noexcept;
        FramePassResource(std::string name,
                          core::UInt32 id,
                          Descriptor &&descriptor,
                          Resource &&resource) noexcept;
        ~FramePassResource() override;

        FramePassResource(FramePassResource &&);
        FramePassResource &operator=(FramePassResource &&);

        inline const Descriptor &descriptor() const noexcept;

        inline Resource &resource() noexcept;
        inline const Resource &resource() const noexcept;

        inline void realize(const render::Device &device) noexcept override;
        inline void derealize() noexcept override;

      private:
        Descriptor m_descriptor;

        Resource m_resource;
    };
} // namespace storm::engine

#include "FramePassResource.inl"
