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
    struct FramePassTextureDescription {
        core::Extentu extent;

        render::PixelFormat format;

        render::TextureType type             = render::TextureType::T2D;
        render::SampleCountFlag sample_count = render::SampleCountFlag::C1_BIT;

        render::TextureLayout source_layout      = render::TextureLayout::Undefined;
        render::TextureLayout destination_layout = render::TextureLayout::Color_Attachment_Optimal;
    };
    struct FramePassBufferDescription {
        core::ArraySize size;
    };

    class FramePassBaseResource: public core::NonCopyable {
      public:
        explicit FramePassBaseResource(std::string name, bool transient = true) noexcept;
        ~FramePassBaseResource();

        FramePassBaseResource(FramePassBaseResource &&);
        FramePassBaseResource &operator=(FramePassBaseResource &&);

        inline bool isTransient() const noexcept;

        inline std::string_view name() const noexcept;

        inline const FramePassBaseConstObserverPtr &createdBy() const noexcept;
        inline FramePassBaseConstObserverPtrConstSpan readIn() const noexcept;
        inline FramePassBaseConstObserverPtrConstSpan writeIn() const noexcept;

      private:
        bool m_is_transient = true;

        std::string m_name;

        FramePassBaseConstObserverPtr m_create_by;
        FramePassBaseConstObserverPtrArray m_read_in;
        FramePassBaseConstObserverPtrArray m_write_in;

        friend class FramePassBuilder;
    };

    template<typename Description>
    class FramePassResource: public FramePassBaseResource {
      public:
        FramePassResource(std::string name,
                          Description &&description,
                          bool transient = true) noexcept;
        ~FramePassResource();

        FramePassResource(FramePassResource &&);
        FramePassResource &operator=(FramePassResource &&);

        inline const Description &description() const noexcept;

      private:
        Description m_description;
    };
} // namespace storm::engine

#include "FramePassResource.inl"
