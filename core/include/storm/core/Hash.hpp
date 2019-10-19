// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Hash.hpp
/// \author Arthapz

#pragma once

#include <functional>
#include <gsl/string_span>
#include <storm/core/Platform.hpp>
#include <string>
#include <type_traits>

namespace storm::core {
	/// \brief storm::core::enum_hash<T> is a callable object which compute the
	/// hash of an enum \tparam T the enum type \requires Type 'T' shall be an
	/// enum or an enum class
	template <typename T> struct enum_hash {
		/// \group Public Special Member Functions
		/// \brief Compute the hash of an enum
		/// \returns the hash of the enum
		constexpr std::size_t operator()(T val) const noexcept {
			using underlying_type = std::underlying_type_t<T>;

			auto val_c = static_cast<underlying_type>(val);

			return std::hash<underlying_type>{}(val_c);
		}

	  private:
		using sfinae = std::enable_if_t<std::is_enum_v<T>>;
	};

	/// \brief storm::core::hash_combine<T> is a function which compute the hash
	/// of data \tparam T the data type \param hash the hash which will be
	/// combined \param v the data which will be hashed and combined
	template <class T>
	constexpr inline void hash_combine(std::size_t &hash, const T &v) noexcept {
		std::hash<T> hasher;
		hash ^= hasher(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	/// \exclude
	template <typename Result, std::size_t size>
	constexpr inline Result
		ComponentHash(gsl::czstring_span<size> str) noexcept {
		return size == 0 ? 0xcbf29ce484222325UL
						 : (static_cast<std::size_t>(str[0]) ^
							ComponentHash<Result, size - 1>(str[1])) *
							   0x100000001b3UL;
	}

	/// \exclude
	template <typename Result>
	constexpr inline Result ComponentHash(const char *str,
										  std::size_t size) noexcept {
		return size == 0 ? 0xcbf29ce484222325UL
						 : (static_cast<std::size_t>(str[0]) ^
							ComponentHash<Result>(str + 1, size - 1)) *
							   0x100000001b3UL;
	}

	/// \exclude
	template <typename Result>
	inline Result ComponentHash(const std::string &str) noexcept {
		return ComponentHash<Result>(str.c_str(), std::size(str));
	}

	/// \exclude
	constexpr inline std::uint64_t operator"" _hash(const char *str,
													std::size_t size) noexcept {
		return ComponentHash<std::uint64_t>(str, size);
	}
} // namespace storm::core

/// \exclude
namespace std {
	/// \exclude
	template <typename T> struct hash<std::vector<T>> {
		std::size_t operator()(const std::vector<T> &in) const noexcept {
			auto size		 = std::size(in);
			std::size_t seed = 0;
			for (size_t i = 0; i < size; i++)
				storm::core::hash_combine(seed, in[i]);

			return seed;
		}
	};
} // namespace std

/// \brief HASH_FUNC is a macro helper which declare std::hash<> and
/// std::equal_to to a type \param x the type
#define HASH_FUNC(x)                                                           \
	namespace std {                                                            \
		template <> struct STORM_PUBLIC hash<x> {                              \
			[[nodiscard]] std::size_t operator()(const x &) const noexcept;    \
		};                                                                     \
		template <> struct equal_to<x> {                                       \
			[[nodiscard]] inline bool operator()(const x &first,               \
												 const x &second) const        \
				noexcept {                                                     \
				static auto hasher = hash<x>{};                                \
				return hasher(first) == hasher(second);                        \
			}                                                                  \
		};                                                                     \
	}
