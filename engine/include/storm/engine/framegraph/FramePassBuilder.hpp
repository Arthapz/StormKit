// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/framegraph/FramePassResource.hpp>

namespace storm::engine {
    class STORM_PUBLIC FramePassBuilder: public core::NonCopyable {
      public:
        FramePassBuilder(FrameGraph &frame_graph, FramePassBase &frame_pass) noexcept;
        ~FramePassBuilder();

        FramePassBuilder(FramePassBuilder &&);
        FramePassBuilder &operator=(FramePassBuilder &&);

        template<typename T>
        [[nodiscard]] FramePassResourceID<T> create(std::string name,
                                                    typename T::Descriptor descriptor);
        template<typename T>
        [[nodiscard]] FramePassResourceID<T> read(FramePassResourceID<T> input);
        template<typename T>
        [[nodiscard]] FramePassResourceID<T> write(FramePassResourceID<T> input);

        [[nodiscard]] FramePassTextureID sample(FramePassTextureID input);
        [[nodiscard]] FramePassTextureID resolve(FramePassTextureID input);

      private:
        FrameGraphObserverPtr m_frame_graph;
        FramePassBaseObserverPtr m_frame_pass;
    };
} // namespace storm::engine
