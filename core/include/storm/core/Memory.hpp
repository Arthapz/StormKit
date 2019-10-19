// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Memory.hpp
/// \author Arthapz

#pragma once

#include <memory>
#include <optional>
#include <type_traits>

#include <storm/core/NamedType.hpp>
#include <storm/core/Platform.hpp>

#if __has_include(<experimental/memory>)
#include <experimental/memory>
namespace _std {
	template <typename T>
	using observer_ptr = std::experimental::observer_ptr<T>;

	using std::experimental::make_observer;
} // namespace _std
#else
namespace _std {
	template <typename T> class observer_ptr {
	  public:
		using element_type = T;

		constexpr observer_ptr() noexcept = default;
		constexpr observer_ptr(std::nullptr_t) noexcept {
		}

		template <typename U, typename = std::enable_if<
								  !std::is_same_v<element_type, U> &&
								  std::is_convertible_v<U *, element_type *>>>
		observer_ptr(observer_ptr<U> const &other)
			: observer_ptr(static_cast<element_type *>(other.get())) {
		}

		explicit observer_ptr(element_type *ptr) : _data(ptr) {
		}

		constexpr element_type *release() noexcept {
			auto *ptr = _data;
			_data	 = nullptr;
			return ptr;
		}

		constexpr void reset(element_type *p = nullptr) noexcept {
			_data = p;
		}

		constexpr void swap(observer_ptr &other) noexcept {
			using std::swap;
			swap(_data, other._data);
		}

		constexpr friend void swap(observer_ptr &lhs,
								   observer_ptr &rhs) noexcept {
			lhs.swap(rhs);
		}

		[[nodiscard]] constexpr element_type *get() const noexcept {
			return _data;
		}

		[[nodiscard]] constexpr std::add_lvalue_reference_t<element_type>
			operator*() const {
			return *get();
		}

		[[nodiscard]] constexpr element_type *operator->() const noexcept {
			return get();
		}

		[[nodiscard]] constexpr explicit operator bool() const noexcept {
			return _data == nullptr;
		}

		[[nodiscard]] constexpr explicit operator element_type *() const
			noexcept {
			return get();
		}

	  private:
		element_type *_data = nullptr;
	};

	template <typename T>
	[[nodiscard]] observer_ptr<T> make_observer(T *ptr) noexcept {
		return observer_ptr<T>(ptr);
	}

	template <typename T1, typename T2>
	[[nodiscard]] bool operator==(observer_ptr<T1> const &p1,
								  observer_ptr<T2> const &p2) {
		return p1.get() == p2.get();
	}

	template <typename T1, typename T2>
	[[nodiscard]] bool operator!=(observer_ptr<T1> const &p1,
								  observer_ptr<T2> const &p2) {
		return !(p1 == p2);
	}

	template <typename T>
	[[nodiscard]] bool operator==(observer_ptr<T> const &p,
								  std::nullptr_t) noexcept {
		return static_cast<bool>(p);
	}

	template <typename T>
	[[nodiscard]] bool operator==(std::nullptr_t,
								  observer_ptr<T> const &p) noexcept {
		return static_cast<bool>(p);
	}

	template <typename T>
	[[nodiscard]] bool operator!=(observer_ptr<T> const &p,
								  std::nullptr_t) noexcept {
		return !p;
	}

	template <typename T>
	[[nodiscard]] bool operator!=(std::nullptr_t,
								  observer_ptr<T> const &p) noexcept {
		return !p;
	}

	template <typename T1, typename T2>
	[[nodiscard]] bool operator<(observer_ptr<T1> const &p1,
								 observer_ptr<T2> const &p2) {
		return p1.get() < p2.get();
	}

	template <typename T1, typename T2>
	[[nodiscard]] bool operator>(observer_ptr<T1> const &p1,
								 observer_ptr<T2> const &p2) {
		return p2 < p1;
	}

	template <typename T1, typename T2>
	[[nodiscard]] bool operator<=(observer_ptr<T1> const &p1,
								  observer_ptr<T2> const &p2) {
		return !(p2 < p1);
	}

	template <typename T1, typename T2>
	[[nodiscard]] bool operator>=(observer_ptr<T1> const &p1,
								  observer_ptr<T2> const &p2) {
		return !(p1 < p2);
	}
} // namespace _std

namespace std {
	template <typename T> struct hash<_std::observer_ptr<T>> {
		[[nodiscard]] std::size_t
			operator()(const _std::observer_ptr<T> &ptr) const noexcept {
			return hash<typename _std::observer_ptr<T>::element_type *>{}(
				ptr.get());
		}
	};
} // namespace std

#endif

// from https://github.com/joboccara/NamedType
// Enable empty base class optimization with multiple inheritance on Visual
// Studio.
#if defined(_MSC_VER) && _MSC_VER >= 1910
#define STORM_EBCO __declspec(empty_bases)
#else
#define STORM_EBCO
#endif

namespace storm::core {
	template <typename T> auto makeObserver(const std::unique_ptr<T> &ptr) {
		return _std::make_observer<T>(ptr.get());
	}

    template <typename T> auto makeObserver(const _std::observer_ptr<T> &ptr) {
        return ptr;
    }

	template <typename T> auto makeObserver(T *ptr) {
		return _std::make_observer<T>(ptr);
	}

	template <typename... Args> inline auto makeObserverArray(Args &... args) {
		return std::array{(makeObserver(args), ...)};
	}

	template <typename T>
	auto makeConstObserver(const std::unique_ptr<T> &ptr) {
		return _std::make_observer<const T>(ptr.get());
    }

    template <typename T>
    auto makeConstObserver(const _std::observer_ptr<T> &ptr) {
        return ptr;
    }

	template <typename T> auto makeConstObserver(const T *const ptr) {
		return _std::make_observer<const T>(ptr);
	}

	template <typename... Args>
	inline auto makeConstObservers(const Args &... args) {
		return std::initializer_list{(makeConstObserver(args), ...)};
	}

	template <typename... Args>
	inline auto makeConstObserverArray(const Args &... args) {
		return std::array{(makeConstObserver(args), ...)};
	}

#define DECLARE_PTR_AND_REF_(x)                                                \
	using x##ObserverPtr	  = _std::observer_ptr<x>;                         \
	using x##ConstObserverPtr = _std::observer_ptr<const x>;                   \
	using x##OwnedPtr		  = std::unique_ptr<x>;                            \
	using x##SharedPtr		  = std::shared_ptr<x>;                            \
	using x##WeakPtr		  = std::weak_ptr<x>;                              \
	using x##Ref			  = std::reference_wrapper<x>;                     \
	using x##CRef			  = std::reference_wrapper<const x>;               \
	using x##Opt			  = std::optional<const x>;

/// \brief DECLARE_PTR_AND_REF is a macro which define some pointer and ref
/// aliases to a type \param type the type which will get aliases
#define DECLARE_PTR_AND_REF(type) DECLARE_PTR_AND_REF_(type)

/// \brief PADDING is a macro which insert padding within a struct or class
/// \param size the size of padding
#define PADDING(size) std::byte private____padding[size];

#if defined(STORM_COMPILER_MSVC)
#define BEGIN_PACKED_STRUCT_A ___pragma(pack(push, 1))
#define BEGIN_PACKED_STRUCT(x) BEGIN_PACKED_STRUCT_A struct x
#define END_PACKET_STRUCT ___pragma(pack(pop))
#elif defined(STORM_COMPILER_CLANG) || defined(STORM_COMPILER_GCC)
	/// \exclude
#define BEGIN_PACKED_STRUCT_A __attribute__((__packed__))
	/// \brief BEGIN_PACKED_STRUCT is a macro which start definition of a packed
/// struct \param type the packed struct type
#define BEGIN_PACKED_STRUCT(type) struct BEGIN_PACKED_STRUCT_A type
/// \brief BEGIN_PACKED_STRUCT is a macro which end definition of a packed
/// struct
#define END_PACKED_STRUCT
#else
#warning Packed struct is not avalaible
#define BEGIN_PACKED_STRUCT_A
#define BEGIN_PACKED_STRUCT(x) BEGIN_PACKED_STRUCT_A struct x
#define END_PACKED_STRUCT
#endif

} // namespace storm::core

#include "private/incbin.h"
