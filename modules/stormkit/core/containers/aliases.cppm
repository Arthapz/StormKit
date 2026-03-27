// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core:containers.aliases;

import std;

namespace stdr = std::ranges;
namespace stdp = std::pmr;

export namespace stormkit { inline namespace core {
    using std::array;

    template<typename T, class Allocator = std::allocator<T>>
    using dyn_array = std::vector<T, Allocator>;

    template<typename T, std::size_t Extent = std::dynamic_extent>
    using array_view = std::span<T, Extent>;

    template<class T, class Extents, class LayoutPolicy = std::layout_right, class AccessorPolicy = std::default_accessor<T>>
    using mdarray_view = std::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>;

    namespace pmr {
        template<typename T>
        using dyn_array = std::vector<T, std::type_identity_t<stdp::polymorphic_allocator<T>>>;
    } // namespace pmr
}} // namespace stormkit::core
