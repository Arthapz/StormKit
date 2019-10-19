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

#include <algorithm>
#include <memory>
#include <mutex>
#include <storm/core/Assert.hpp>
#include <storm/core/NonCopyable.hpp>
#include <unordered_map>
#include <vector>

namespace storm::tools {
	template <typename Key, class Resource,
			  class Deleter = std::default_delete<Resource>>
	class ResourcesPool : public core::NonCopyable {
		struct ResourcePoolDeleter {
			explicit ResourcePoolDeleter(
				Key key,
				std::weak_ptr<ResourcesPool<Key, Resource, Deleter> *> owner)
				: m_key(key), m_owner(owner) {
			}

			void operator()(Resource *ptr) {
				if (auto scoped_ptr = m_owner.lock()) {
					try {
						(*(scoped_ptr.get()))
							->add(m_key, ResourceOwnedPtr(ptr));
						return;
					} catch (const std::bad_alloc &) {
					}
				}

				Deleter{}(ptr);
			}

		  private:
			std::weak_ptr<ResourcesPool<Key, Resource, Deleter> *> m_owner;
			Key m_key;
		};

	  public:
		using ResourceOwnedPtr = std::unique_ptr<Resource, Deleter>;
		using ScopedResourceOwnedPtr =
			std::unique_ptr<Resource, ResourcePoolDeleter>;

		explicit ResourcesPool()
			: m_shared_this(new ResourcesPool<Key, Resource, Deleter> *(this)) {
		}
		ResourcesPool(ResourcesPool &&) = default;
		ResourcesPool &operator=(ResourcesPool &&) = default;

		~ResourcesPool() = default;

		inline void add(Key &&key, ResourceOwnedPtr &&resource) {
			std::lock_guard<std::mutex> lock(m_sync);
			Key _key = std::move(key);
			m_pool.emplace(_key, std::move(resource));
			m_keys.emplace_back(std::move(_key));
		}
		inline void add(const Key &key, ResourceOwnedPtr &&resource) {
			std::lock_guard<std::mutex> lock(m_sync);
			m_pool.emplace(std::move(Key(key)), std::move(resource));
			m_keys.emplace_back(std::move(Key(key)));
		}

		inline void remove(Key &&key) {
			std::lock_guard<std::mutex> lock(m_sync);
			auto value_it = m_pool.find(key);
			ASSERT(value_it != std::end(m_pool), "Invalid key");

			auto key_it = std::find(std::begin(m_keys), std::end(m_keys), key);
			ASSERT(key_it != std::end(m_keys), "Invalid key");

			m_pool.erase(value_it);
			m_keys.erase(key_it);
		}
		inline void remove(const Key &key) {
			std::lock_guard<std::mutex> lock(m_sync);
			auto value_it = m_pool.find(key);
			ASSERT(value_it != std::end(m_pool), "Invalid key");

			auto key_it = std::find(std::begin(m_keys), std::end(m_keys), key);
			ASSERT(key_it != std::end(m_keys), "Invalid key");

			m_pool.erase(value_it);
			m_keys.erase(key_it);
		}

		ScopedResourceOwnedPtr acquire(const Key &key) {
			ASSERT(!empty(), "Empty pool");

			std::lock_guard<std::mutex> lock(m_sync);

			auto value_it = m_pool.find(key);
			ASSERT(value_it != std::end(m_pool), "Invalid key");

			auto key_it = std::find(std::begin(m_keys), std::end(m_keys), key);
			ASSERT(key_it != std::end(m_keys), "Invalid key");

			ScopedResourceOwnedPtr ptr{
				value_it->second.release(),
				ResourcePoolDeleter{*key_it, m_shared_this}};
			m_pool.erase(value_it);
			m_keys.erase(key_it);

			return std::move(ptr);
		}

		inline bool empty() const noexcept {
			return m_pool.empty();
		}

		inline std::size_t size() const noexcept {
			return m_pool.size();
		}

		inline const std::vector<Key> &keys() const {
			return m_keys;
		}

		void clear() {
			std::lock_guard<std::mutex> lock(m_sync);

			m_pool.clear();
		}

	  private:
		std::shared_ptr<ResourcesPool<Key, Resource, Deleter> *> m_shared_this;
		std::unordered_map<Key, ResourceOwnedPtr> m_pool;
		std::vector<Key> m_keys;
		std::mutex m_sync;
	};
} // namespace storm::tools
