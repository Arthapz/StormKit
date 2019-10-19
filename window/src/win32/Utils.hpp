// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <Windows.h>
#include <storm/window/Key.hpp>

storm::window::Key win32KeyToStormKey(WPARAM key, LPARAM flags);
