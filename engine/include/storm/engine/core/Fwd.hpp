// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Memory.hpp>

namespace storm::engine {
    class Geometry;
    DECLARE_PTR_AND_REF(Geometry);

    class Renderer;
    DECLARE_PTR_AND_REF(Renderer);

    class Transform;
    DECLARE_PTR_AND_REF(Transform);

    class RingHardwareBuffer;
    DECLARE_PTR_AND_REF(RingHardwareBuffer);

    class VertexArray;
    DECLARE_PTR_AND_REF(VertexArray);

    class DebugGUI;
    DECLARE_PTR_AND_REF(DebugGUI);

    class Profiler;
    DECLARE_PTR_AND_REF(Profiler);

    class PipelineBuilder;
    DECLARE_PTR_AND_REF(PipelineBuilder);
} // namespace storm::engine
