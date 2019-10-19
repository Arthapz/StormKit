// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file NonMovable.hpp
/// \author Arthapz

#pragma once

#include <storm/core/Platform.hpp>

namespace storm::core {
	/// \brief storm::core::NonMovable<T> is a utility class to declare non
	/// movable type
	///
	/// This class ensure that the child class will be non movable
	class STORM_PUBLIC NonMovable {
	  public:
		/// \output_section Public Special Member Functions
		/// \brief Default constructor
		constexpr NonMovable() noexcept = default;

		/// \brief Destructor
		~NonMovable() noexcept = default;

		/// \brief Copy constructor
		NonMovable(NonMovable &) noexcept = default;

		/// \brief Copy assignement
		NonMovable &operator=(const NonMovable &) noexcept = default;

		/// \exclude
		NonMovable(NonMovable &&) = delete;

		/// \exclude
		NonMovable &operator=(NonMovable &&) = delete;
	};
} // namespace storm::core
