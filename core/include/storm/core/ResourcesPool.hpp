// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once
/**
 * @file ResourcesPool.hpp
 * @brief Thread safe resource pool managment
 * @author Arthapz
 * @version 0.1
 **/

#include <unordered_map>

#include <storm/core/Assert.hpp>
#include <storm/core/NonCopyable.hpp>

namespace storm::core {
    template<typename _Key, typename _Value>
    class STORM_PUBLIC ResourcesPool: public NonCopyable {
      public:
        using Key   = _Key;
        using Value = _Value;

        ResourcesPool()  = default;
        ~ResourcesPool() = default;

        ResourcesPool(ResourcesPool &&) = default;
        ResourcesPool &operator=(ResourcesPool &&) = default;

        template<typename... Args>
        Value &create(Key &&key, Args &&... args);
        template<typename... Args>
        Value &create(const Key &key, Args &&... args);

        Value &get(const Key &key);
        const Value &get(const Key &key) const;

        void remove(const Key &key);

        inline bool has(const Key &key) const noexcept;

        void clear();

      private:
        using Map = std::unordered_map<Key, Value>;

        inline auto getIt(const Key &key) noexcept;
        inline auto getIt(const Key &key) const noexcept;

        Map m_resources;
    };
} // namespace storm::core

#include "ResourcesPool.inl"
