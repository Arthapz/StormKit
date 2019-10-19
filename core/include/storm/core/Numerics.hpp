// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Numerics.hpp
/// \author Arthapz

#pragma once

#include <array>
#include <cfloat>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace storm::core {
	/// \brief storm::core::seed is an utility function wich initialize the seed
	/// for the random generator \param seed the seed
	inline void seed(std::uint32_t seed) noexcept;

	/// \brief storm::core::rand<T> is an utility function wich generate a
	/// floating point number borned between [min, max[ \tparam T the type of
	/// the result \param min the min of the value \param max the max of the
	/// value \returns a value between [min, max[
	template <typename T> T rand(T &&min, T &&max) noexcept;

	/// \exclude
	template <typename T, typename V>
	using IfIsFloatingPoint = typename std::enable_if_t<
		std::is_floating_point_v<std::decay_t<std::remove_cv_t<V>>> &&
			std::is_floating_point_v<std::decay_t<std::remove_cv_t<T>>>,
		bool>;

	/// \brief storm::core::realIsEqual<T, V> is an utility function wich check
	/// if two reals are equals \tparam T the type of first real \tparam T the
	/// type of second real \param a the first real \param a the second real
	/// \returns a boolean at true if a == b and false if a != b
	template <typename T, typename V, typename = IfIsFloatingPoint<T, V>>
	inline constexpr auto realIsEqual(T &&a, V &&b) noexcept;

	// TODO write conf of map function
	/// \exclude
	template <typename T>
	inline constexpr T map(T x, T in_min, T in_max, T out_min,
						   T out_max) noexcept;

	/// \brief storm::core::pi<T> is a templated variable which store PI
	/// \tparam T the required type for representing PI
	/// \see [piLD](standardese://storm::core::piLD/)
	/// \see [piD](standardese://storm::core::piD/)
	/// \see [piF](standardese://storm::core::piF/)
	template <typename T>
	static constexpr const T pi = T(
		3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211L);

	/// \brief storm::core::piLD is a PI representation stored on long double
	/// \see [pi](standardese://storm::core::pi/)
	static constexpr const auto piLD = pi<long double>;

	/// \brief storm::core::piD is a PI representation stored on double
	/// \see [pi](standardese://storm::core::pi/)
	static constexpr const auto piD = pi<double>;

	/// \brief storm::core::piF is a PI representation stored on float
	/// \see [pi](standardese://storm::core::pi/)
	static constexpr const auto piF = pi<float>;

	/// \brief storm::core::degToRad<T> is an utility function witch convert
	/// degree to radians \tparam T the result type \param angle the angle
	/// exprimed in degree \returns the input angle exprimed in radians
	template <typename T> inline constexpr auto degToRad(T angle) noexcept;

	/// \brief storm::core::radToDeg<T> is an utility function witch convert
	/// radians to degree \tparam T the result type \param angle the angle
	/// exprimed in radians \returns the input angle exprimed in degree
	template <typename T> inline constexpr auto radToDeg(T angle) noexcept;

	/// \brief storm::core::genRange<Integer, start, stop> is an utility
	/// function witch generate a compile time range \tparam Integer the value
	/// type \tparam start the start value \tparam stop the stop value \returns
	/// the generated range
	template <class Integer, Integer start, Integer stop>
	constexpr decltype(auto) genRange() noexcept;

	/// \brief storm::core::genRange<Integer> is an utility function witch
	/// generate a compile time range \tparam Integer the value type \param
	/// start the start value \param stop the stop value \returns the generated
	/// range
	template <class Integer>
	constexpr decltype(auto) genRange(Integer start, Integer stop) noexcept;

	/// \brief storm::core::extractOwnedPtr<T> is an utility function wich
	/// extract the address value from a pointer \tparam T the pointer
	/// underlying type \param ptr the pointer \returns the pointer address
	/// value as integer
	template <class T> inline std::intptr_t extractOwnedPtr(T *ptr) noexcept;
} // namespace storm::core

#include "Numerics.inl"
