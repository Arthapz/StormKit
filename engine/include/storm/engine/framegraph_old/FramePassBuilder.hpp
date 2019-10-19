// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/framegraph/FramePassResource.hpp>
#include <storm/engine/framegraph/Fwd.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassBuilder: public core::NonCopyable {
      public:
        FramePassBuilder(FrameGraph &frame_graph, FramePassBase &frame_pass) noexcept;
        ~FramePassBuilder();

        FramePassBuilder(FramePassBuilder &&);
        FramePassBuilder &operator=(FramePassBuilder &&);

        FramePassTextureResourceObserverPtr create(std::string name,
                                                   FramePassTextureDescription description);
        FramePassBufferResourceObserverPtr create(std::string name,
                                                  FramePassBufferDescription description);

        FramePassTextureResourceObserverPtr read(FramePassTextureResource &texture);
        FramePassBufferResourceObserverPtr read(FramePassBufferResource &buffer);

        FramePassTextureResourceObserverPtr write(FramePassTextureResource &texture);
        FramePassBufferResourceObserverPtr write(FramePassBufferResource &buffer);

      private:
        FrameGraphObserverPtr m_frame_graph;
        FramePassBaseObserverPtr m_frame_pass;
    };
} // namespace storm::engine

#include "FramePassBuilder.inl"
