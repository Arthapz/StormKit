// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.parallelism.threadutils;

import std;

import stormkit.core.types;
import stormkit.core.meta;

export namespace stormkit { inline namespace core {
    STORMKIT_CORE_API
    auto set_current_thread_name(string_view name) noexcept -> void;
    STORMKIT_CORE_API
    auto set_thread_name(std::thread& thread, string_view name) noexcept -> void;
    STORMKIT_CORE_API
    auto set_thread_name(std::jthread& thread, string_view name) noexcept -> void;
    STORMKIT_CORE_API
    auto get_current_thread_name() noexcept -> string;
    STORMKIT_CORE_API
    auto get_thread_name(const std::thread& thread) noexcept -> string;
    STORMKIT_CORE_API
    auto get_thread_name(const std::jthread& thread) noexcept -> string;

    template<std::ranges::input_range Range>
        requires(meta::IsSpecializationOf<meta::RangeType<Range>, std::future>)
    inline auto wait_all(Range&& futures) noexcept {
        for (auto&& future : std::forward<Range>(futures)) future.wait();
    }
}} // namespace stormkit::core
