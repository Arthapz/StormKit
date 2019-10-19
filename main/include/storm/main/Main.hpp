// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

#if defined(STORM_OS_IOS)

    #define main iosMain

#elif defined(STORM_OS_MACOS)

    #define main macosMain

#endif
