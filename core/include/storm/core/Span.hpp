// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file FileSystem.hpp
/// \author Arthapz
/// \brief this file map std::filesystem or std::filesystem

#pragma once

#include <storm/core/Configure.hpp>
#include <storm/core/Platform.hpp>

//#if defined(HAVE_FILESYSTEM)
#if __has_include(<span>)
#include <span>
namespace storm::core {
	template <class T, auto Extent = std::dynamic_extent>
	using span = std::span<T, Extent>;
} // namespace storm::core
#elif __has_include(<experimental/span>)
#include <experimental/span>
namespace storm::core {
	template <class T, auto Extent = std::dynamic_extent>
	using span = std::experimental::span<T, Extent>;
}
#elif __has_include(<gsl/span>)
#include <gsl/span>
namespace storm::core {
	template <class T, auto Extent = gsl::dynamic_extent>
	using span = gsl::span<T, Extent>;
}
#endif
