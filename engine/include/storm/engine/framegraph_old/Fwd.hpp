// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::engine {
    struct FramePassTextureDescription;
    struct FramePassBufferDescription;

    class FrameGraph;
    DECLARE_PTR_AND_REF(FrameGraph)

    class FramePassBase;
    DECLARE_PTR_AND_REF(FramePassBase)

    template<typename PassData>
    class FramePass;

    class FramePassResources;
    DECLARE_PTR_AND_REF(FramePassResources)

    class FramePassBuilder;
    DECLARE_PTR_AND_REF(FramePassBuilder)

    template<typename Description>
    class FramePassResource;

    using FramePassTextureResource = FramePassResource<FramePassTextureDescription>;
    using FramePassBufferResource  = FramePassResource<FramePassBufferDescription>;

    DECLARE_PTR_AND_REF(FramePassTextureResource)
    DECLARE_PTR_AND_REF(FramePassBufferResource)
    /*class FrameGraphBlackboard;
    DECLARE_PTR_AND_REF(FrameGraphBlackboard)*/
} // namespace storm::engine
