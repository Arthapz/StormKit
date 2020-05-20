// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

namespace storm::engine {
    struct FrameGraphTexture;
    struct FrameGraphBuffer;

    class FrameGraph;
    DECLARE_PTR_AND_REF(FrameGraph)

    class FramePassBuilder;
    DECLARE_PTR_AND_REF(FramePassBuilder)

    class FramePassBase;
    DECLARE_PTR_AND_REF(FramePassBase)

    template<typename PassData>
    class FramePass;

    class FramePassResourceBase;
    DECLARE_PTR_AND_REF(FramePassResourceBase);

    template<typename T>
    class FramePassResource;

    using FramePassTexture = FramePassResource<FrameGraphTexture>;
    using FramePassBuffer  = FramePassResource<FrameGraphBuffer>;

    DECLARE_PTR_AND_REF(FramePassTexture);
    DECLARE_PTR_AND_REF(FramePassBuffer);

    class FramePassResourceHandle;
    DECLARE_PTR_AND_REF(FramePassResourceHandle);

    template<typename T>
    class FramePassResourceID;

    using FramePassTextureID = FramePassResourceID<FrameGraphTexture>;
    using FramePassBufferID  = FramePassResourceID<FrameGraphBuffer>;

    DECLARE_PTR_AND_REF(FramePassTextureID);
    DECLARE_PTR_AND_REF(FramePassBufferID);

    class FramePassResources;
    DECLARE_PTR_AND_REF(FramePassResources);
} // namespace storm::engine
