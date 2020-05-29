// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "private/robin_hood.h"

namespace storm::core {
    template<typename Key,
             typename T,
             typename Hash     = robin_hood::hash<Key>,
             typename KeyEqual = std::equal_to<Key>>
    using HashMap = robin_hood::unordered_map<Key, T, Hash, KeyEqual>;

    template<typename Key,
             typename Hash     = robin_hood::hash<Key>,
             typename KeyEqual = std::equal_to<Key>>
    using HashSet = robin_hood::unordered_set<Key, Hash, KeyEqual>;
} // namespace storm::core
