// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core.containers.hash_map;

import std;

import ankerl.unordered_dense;
import frozen;

import stormkit.core.types;

export namespace stormkit { inline namespace core {
    template<class Key,
             class T,
             class Hash                 = ankerl::unordered_dense::hash<Key>,
             class KeyEqual             = std::equal_to<Key>,
             class AllocatorOrContainer = std::allocator<std::pair<Key, T>>>
    using hash_map = ankerl::unordered_dense::map<Key, T, Hash, KeyEqual, AllocatorOrContainer>;

    template<class Key,
             class Hash                 = ankerl::unordered_dense::hash<Key>,
             class KeyEqual             = std::equal_to<Key>,
             class AllocatorOrContainer = std::allocator<Key>>
    using hash_set = ankerl::unordered_dense::set<Key, Hash, KeyEqual, AllocatorOrContainer>;

    template<class Key, class Value, std::size_t N, typename Hash = frozen::anna<Key>, class KeyEqual = std::equal_to<Key>>
    using static_hash_map = frozen::unordered_map<Key, Value, N, Hash, KeyEqual>;

    template<class Key, std::size_t N, typename Hash = frozen::elsa<Key>, class KeyEqual = std::equal_to<Key>>
    using static_hash_set = frozen::unordered_set<Key, N, Hash, KeyEqual>;
}} // namespace stormkit::core
