// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file Assert.hpp
/// \author Arthapz

#pragma once

#include <exception>
#include <iostream>

#include <gsl/gsl_assert>

#include <storm/core/Platform.hpp>

#ifdef ASSERT
#undef ASSERT
#endif

#if defined(__clang__) || defined(__GNUC__)
/// \brief FUNCTION is a macro which get the C++ symbol of current function as
/// string
#define FUNCTION __PRETTY_FUNCTION__
#else
#define FUNCTION __FUNCSIG__
#endif

#ifdef STORM_BUILD_DEBUG
/// \brief ASSERT is a macro which define an assertion
/// \param condition the condition of the assertion
/// \param message the message which will displayed on failed assertion
#define ASSERT(condition, message)                                             \
	do {                                                                       \
		if (GSL_LIKELY(!(condition))) {                                        \
			std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
					  << " line " << __LINE__ << "\n " << FUNCTION << ": "     \
					  << message << std::endl;                                 \
			std::terminate();                                                  \
		}                                                                      \
	} while (false)
#else
#define ASSERT(condition, message)
#endif

#ifdef STORM_BUILD_DEBUG
#define STORM_ENSURES(condition)                                               \
	do {                                                                       \
		if (GSL_LIKELY(!(condition))) {                                        \
			std::cerr << "A postcondition `" #condition "` failed in "         \
					  << __FILE__ << " line " << __LINE__ << "\n " << FUNCTION \
					  << std::endl;                                            \
		}                                                                      \
		Ensures(condition);                                                    \
	} while (false)

#define STORM_EXPECTS(condition)                                               \
	do {                                                                       \
		if (GSL_LIKELY(!(condition))) {                                        \
			std::cerr << "A precondition `" #condition "` failed in "          \
					  << __FILE__ << " line " << __LINE__ << "\n " << FUNCTION \
					  << std::endl;                                            \
		}                                                                      \
		Expects(condition);                                                    \
	} while (false)
#else
#define STORM_ENSURES(condition) Ensures(condition)
#define STORM_EXPECTS(condition) Expects(condition)
#endif
