// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file NamedType.hpp
/// \author Arthapz

#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <storm/core/CRTP.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/TypeTraits.hpp>

namespace storm::core {
	/// \brief storm::core::NamedTypeTrait is a type trait for Namedtype class
	///
	/// This struct permit detection of a named type
	class NamedTypeTrait {};

	/// \brief storm::core::is_named_type<T> NamedTypeTrait is a type trait
	/// detector for Namedtype class \tparam T the type which you want to check
	template <typename T>
	using is_named_type = std::is_base_of<NamedTypeTrait, T>;

	/// \brief storm::core::is_named_type_v<T> is an aliases to the value of
	/// is_named_type \tparam T the type which you want to check
	template <typename T>
	inline constexpr bool is_named_type_v = is_named_type<T>::value;

	/// \brief storm::core::NamedType<T, Parameter, Skills> is an utility class
	/// wich define named type
	///
	/// This class permit to declare strongs types
	/// \tparam T the type you want to strongify
	/// \tparam Tag a unique tag
	/// \tparam Skills the skills wich NamedType will have
	template <typename T, typename Tag, template <typename> class... Skills>
	class NamedType : public NamedTypeTrait,
					  public Skills<NamedType<T, Tag, Skills...>>... {
	  public:
		using UnderlyingType = T;

		/// \output_section Public Special Member Functions
		/// \brief Construct a NameType with underlying type built in place
		/// \tparam Args underlying type constructor parameters
		template <typename... Args>
		explicit constexpr NamedType(Args &&... args) noexcept
			: m_value{std::forward<Args>(args)...} {
		}

		/// \brief Construct a NameType with copying / moving / referencing an
		/// existing value
		explicit constexpr NamedType(T &&value) noexcept
			: m_value{std::forward<T>(value)} {
		}

		/// \output_section Public Member Functions
		/// \brief Get the underlying value as reference
		/// \returns a reference to the underlying value
		constexpr inline T &get() noexcept {
			return m_value;
		}

		/// \brief Get the underlying value as const reference
		/// \returns a const reference to the underlying value
		constexpr inline T const &get() const noexcept {
			return m_value;
		}

		/// \brief cast to the underlying value as reference
		/// \returns a reference to the underlying value
		constexpr inline operator T &() noexcept {
			return get();
		}

		/// \brief cast to the underlying value as const reference
		/// \returns a const reference to the underlying value
		constexpr inline operator const T &() const noexcept {
			return get();
		}

		/// \brief cast to the underlying value as copy
		/// \returns a copy of the underlying value
		constexpr inline operator const T() const noexcept {
			return m_value;
		}

		constexpr inline const T &operator*() const noexcept {
			return get();
		}

		constexpr inline T &operator*() noexcept {
			return get();
		}

		constexpr inline const T *operator->() const noexcept {
			return &get();
		}

		constexpr inline T *operator->() noexcept {
			return &get();
		}

	  private:
		T m_value;
	};

	/// \brief storm::core::makeNamed<StrongType, Args> is an utility function
	/// to create a NamedType \tparam NamedType the result type \tparam Args
	/// arguments to construct the underlying type \returns the namedtype result
	template <class NamedType, typename... Args>
	constexpr NamedType makeNamed(Args &&... args) noexcept {
		return NamedType(
			typename NamedType::UnderlyingType(std::forward<Args>(args)...));
	}
} // namespace storm::core

/// \exclude
namespace std {
	/// \exclude
	template <typename T, typename Tag, template <typename> class... Skills>
	struct hash<storm::core::NamedType<T, Tag, Skills...>> {
		using NamedType = storm::core::NamedType<T, Tag, Skills...>;
		using checkIfHashable =
			typename std::enable_if<NamedType::is_hashable, void>::type;

		size_t
			operator()(storm::core::NamedType<T, Tag, Skills...> const &x) const
			noexcept {
			return std::hash<T>()(*x);
		}
	};
} // namespace std
