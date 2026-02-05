// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

import std;

#define NO_CONSTANTS
#include <stormkit/core/config.hpp>

export module stormkit;

export import stormkit.core;

#if STORMKIT_LIB_LOG_ENABLED
export import stormkit.log;
#endif
#if STORMKIT_LIB_ENTITIES_ENABLED
export import stormkit.entities;
#endif
#if STORMKIT_LIB_IMAGE_ENABLED
export import stormkit.image;
#endif
#if STORMKIT_LIB_WSI_ENABLED
export import stormkit.wsi;
#endif
#if STORMKIT_LIB_GPU_ENABLED
export import stormkit.gpu;
#endif
#if STORMKIT_LIB_LUA_ENABLED
export import stormkit.lua;
#endif
