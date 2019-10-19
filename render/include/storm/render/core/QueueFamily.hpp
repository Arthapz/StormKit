// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct QueueFamily {
        QueueFlag flags;
        core::UInt32 count;

        bool has_present_support = false;
    };
} // namespace storm::render
