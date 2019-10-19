#pragma once

#include <type_traits>

template <typename E> struct enable_bitmask_operators {
	constexpr enable_bitmask_operators() = default;
	static const bool enable			 = false;
};

template <typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
	operator|(E lhs, E rhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(static_cast<underlying>(lhs) |
						  static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
	operator&(E lhs, E rhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(static_cast<underlying>(lhs) &
						  static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
	operator^(E lhs, E rhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(static_cast<underlying>(lhs) ^
						  static_cast<underlying>(rhs));
}

template <typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
	operator~(E lhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	return static_cast<E>(~static_cast<underlying>(lhs));
}

template <typename E>
constexpr
	typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
	operator|=(E &lhs, E rhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(static_cast<underlying>(lhs) |
						 static_cast<underlying>(rhs));
	return lhs;
}

template <typename E>
constexpr
	typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
	operator&=(E &lhs, E rhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(static_cast<underlying>(lhs) &
						 static_cast<underlying>(rhs));
	return lhs;
}

template <typename E>
constexpr
	typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
	operator^=(E &lhs, E rhs) noexcept {
	typedef typename std::underlying_type<E>::type underlying;
	lhs = static_cast<E>(static_cast<underlying>(lhs) ^
						 static_cast<underlying>(rhs));
	return lhs;
}
