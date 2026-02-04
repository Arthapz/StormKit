// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core:utils.time;

import std;

import :typesafe.floating_point;

export namespace stormkit { inline namespace core {
    using fsecond = std::chrono::duration<f32, std::chrono::seconds::period>;
}} // namespace stormkit::core
