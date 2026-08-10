// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/as_casts_macro.hpp>
#include <stormkit/core/format_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.heap;

import std;

import stormkit.core.meta;
import stormkit.core.types;

export namespace stormkit { inline namespace core {
    struct MemoryAllocationError {
        string_view type;
        usize       size;
    };

    template<typename FormatContext>
    auto format_as(const MemoryAllocationError&, FormatContext&) noexcept -> FormatContext::iterator;

    template<typename T>
    using heap_ptr = std::unique_ptr<T>;
    using std::shared_ptr;

    template<class T, class... Ts>
    auto allocate(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...)))
      -> std::expected<heap_ptr<T>, MemoryAllocationError>;

    template<class T, class... Ts>
    auto allocate_unsafe(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...))) -> heap_ptr<T>;

    template<class T, class... Ts>
    auto allocate_shared(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...)))
      -> std::expected<shared_ptr<T>, MemoryAllocationError>;

    template<class T, class... Ts>
    auto allocate_shared_unsafe(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...))) -> shared_ptr<T>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename FormatContext>
    auto format_as(const MemoryAllocationError& error, FormatContext& ctx) noexcept -> FormatContext::iterator {
        auto&& out = ctx.out();
        return std::format_to(out,
                              "Failed to allocate type {} of {} byte{}",
                              error.type,
                              error.size,
                              error.size > 1 ? "s!" : "!");
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, class... Ts>
    STORMKIT_FORCE_INLINE
    auto allocate(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...)))
      -> std::expected<heap_ptr<T>, MemoryAllocationError> {
        auto value = heap_ptr<T> { new (std::nothrow) T(std::forward<Ts>(args)...) };
        if (not value) [[unlikely]]
            return std::unexpected(MemoryAllocationError { .type = typeid(T).name(), .size = sizeof(T) });
        return std::expected<heap_ptr<T>, MemoryAllocationError> { std::in_place, std::move(value) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, class... Ts>
    STORMKIT_FORCE_INLINE
    auto allocate_unsafe(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...))) -> heap_ptr<T> {
        return heap_ptr<T> { new (std::nothrow) T(std::forward<Ts>(args)...) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, class... Ts>
    STORMKIT_FORCE_INLINE
    auto allocate_shared(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...)))
      -> std::expected<shared_ptr<T>, MemoryAllocationError> {
        auto value = shared_ptr<T> { new (std::nothrow) T(std::forward<Ts>(args)...) };
        if (not value) [[unlikely]]
            return std::unexpected(MemoryAllocationError { .type = typeid(T).name(), .size = sizeof(T) });
        return std::expected<shared_ptr<T>, MemoryAllocationError> { std::in_place, std::move(value) };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, class... Ts>
    STORMKIT_FORCE_INLINE
    auto allocate_shared_unsafe(Ts&&... args) noexcept(noexcept(T(std::forward<Ts>(args)...))) -> shared_ptr<T> {
        return shared_ptr<T> { new (std::nothrow) T(std::forward<Ts>(args)...) };
    }
}} // namespace stormkit::core
