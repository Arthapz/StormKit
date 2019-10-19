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
#if __has_include(<filesystem>)
#include <filesystem>
namespace storm::core {
#if defined(__has_cpp_attribute) && defined(__cpp_lib_filesystem)
	namespace filesystem = std::filesystem;
#else
	namespace filesystem = std::experimental::filesystem;
#endif
} // namespace storm::core
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace storm::core {
#if defined(__has_cpp_attribute) && defined(__cpp_lib_filesystem)
	namespace filesystem = std::filesystem;
#else
	namespace filesystem = std::experimental::filesystem;
#endif
} // namespace storm::core
#endif
